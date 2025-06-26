//
// 3-isogeny chain calculations via radical computations (over Fp)
//
#include <stdint.h>
#include "isogeny_walks.h"

// Cube root when p = 2 mod 3
static void fp_curt(fp output, const fp input) {
    fp temp, input_1;
    uint64_t flag;
    int i, j;
    fp_set1(input_1);
    fp_copy(temp, input);
    for (i = 0; i < NUMBER_OF_WORDS; i++) {
        flag = 1;
        for (j = 0; j < 64; j++) {
            if ((flag & CUBE_ROOT_EXPONENT_213[i]) != 0)
                fp_mul(input_1, temp, input_1);
            fp_sqr(temp, temp);
            flag <<= 1;
        }
    }
    fp_copy(output, input_1);
}

void isogeny_walks_get_points_3_fp(fp *output, const proj input_A){
    fp A_times_one_third, A_squared, C_squared;
    fp t, r, s, u, aux, tmp, y;
    fp s0, s1, s2, s0_squared, v;

    // input_A.x is A
    // input_A.z is C
    // A_times_one_third = A * one_third                    #               M𝒸
    fp_mul(A_times_one_third, ONE_THIRD, input_A.x);
    // A_squared = A**2                                     #               S
    fp_sqr(A_squared, input_A.x);
    // C_squared = C**2                                     #               S
    fp_sqr(C_squared, input_A.z);

    // t = (3 * C_squared - A_squared) * one_ninth          #               M𝒸 + 3A
    fp_add(t, C_squared, C_squared);
    fp_add(t, t, C_squared);
    fp_sub(t, t, A_squared);
    fp_mul(t, ONE_NINTH, t);

    // r = 16 * (A_squared * A) * one_by_27                 #               M + M𝒸 + 4A
    fp_mul(r, A_squared, input_A.x);
    fp_mul(r, ONE_BY_27, r);
    fp_add(r, r, r);
    fp_add(r, r, r);
    fp_add(r, r, r);
    fp_add(r, r, r);

    // s = 8 * A_times_one_third                            #                 3A
    fp_add(s, A_times_one_third, A_times_one_third);
    fp_add(s, s, s);
    fp_add(s, s, s);

    // u = r - s * C_squared                                #                M  +  A
    fp_mul(u, s, C_squared);
    fp_sub(u, r, u);

    // aux = 2*C_squared**2                                         #               S + A
    fp_sqr(aux, C_squared);
    fp_add(aux, aux, aux);
    // y = 4 * C_squared                                            #               2A
    fp_add(y, C_squared, C_squared);
    fp_add(y, y, y);
    // y = y - A_squared                                            #               A
    fp_sub(y, y, A_squared);
    // y = aux * y                                                  #               M
    fp_mul(y, aux, y);

    // y = t + one_third * setup['curt'](y)                         #            M𝒸 + A + E   (TOTAL = S + M + Mc + 5A + E)
    fp_curt(y, y);
    fp_mul(y, y, ONE_THIRD);
    fp_add(y, y, t);

    // r = 2 * y                                                    #                  A
    fp_add(r, y, y);
    // s = 6 * t                                                    #                 3A
    fp_add(s, t, t);
    fp_add(t, s, s);
    fp_add(s, s, t);

    // s0 = setup['sqrt'](r - s)                                    #                  A + E
    fp_sub(s0, r, s);
    fp_sqrt(&s0);

    // s0_squared = s0**2                                           #                 S
    fp_sqr(s0_squared, s0);
    // v = -(r + s)                                                 #                 2A
    fp_add(v, r, s);
    fp_neg1(&v);

    fp_mul(tmp, v, s0_squared); //               #                  M
    fp_mul(u, u, s0);           //               #                  M
    // s1 = setup['sqrt'](tmp + u)                                  #                A + E
    fp_add(t, tmp, u);
    fp_copy(s1, t);
    fp_sqrt(&s1);
    // s2 = setup['sqrt'](tmp - u)                                  #                A + E
    fp_sub(s, tmp, u);
    fp_copy(s2, s);
    fp_sqrt(&s2);

    // Test to know if s1 and s2 are square roots                   #                 2S
    fp_sqr(tmp, s1);
    fp_sqr(aux, s2);
    uint64_t test_s1 = fp_isequal(tmp, t);
    uint64_t test_s2 = fp_isequal(aux, s);

    // Compute z                                                    #                 4Mc + 5A
    fp_sub(t, s1, s0_squared);
    fp_mul(t, t, ONE_HALF);
    fp_neg1(&s1);
    fp_sub(r, s1, s0_squared);
    fp_mul(r, r, ONE_HALF);

    fp_add(s, s2, s0_squared);
    fp_mul(s, s, ONE_HALF);
    fp_sub(u, s0_squared, s2);
    fp_mul(u, u, ONE_HALF);

    // Select z in constant time
    fp_cmov(&output[0], (const fp*)t, test_s1);
    fp_cmov(&output[1], (const fp*)r, test_s1);
    fp_cmov(&output[0], (const fp*)s, test_s2);
    fp_cmov(&output[1], (const fp*)u, test_s2);

    // Compute num and den                                          #                  2M + 2A
    fp_mul(output[2], s0, input_A.z);
    fp_mul(aux, s0, A_times_one_third);
    fp_sub(output[0], output[0], aux);
    fp_sub(output[1], output[1], aux);

    //                                                              #                 2S + 7M + 2A
    fp_sqr(r, output[0]); // x**2
    fp_sqr(s, output[2]); // den**2
    fp_mul(v, input_A.z, output[0]); // C*x
    fp_mul(t, output[2], input_A.x); // A*den
    fp_mul(u, v, s); // C*x*den**2
    fp_mul(aux, s, output[2]); // den**3
    fp_mul(aux, aux, input_A.z); // C*den**3
    fp_add(t, t, v); // C*x + A*den
    fp_mul(t, t, r); // x**2 * (C*x + A*den)
    fp_add(t, t, u); // (x**2 * (C*x + A*den)) + C*x * den**2
    fp_mul(t, t, aux);

    uint64_t test_sqr = (uint64_t)!fp_issquare(t);//          #                     E
    fp_cswap(output[0], output[1], test_sqr);
}

void isogeny_walks_to_montgomery_model_3_fp(fp *output_A, const fp *input_t){
    fp r_squared, s_squared, r_cubed;
    fp aux, alpha_cubed, alpha, rd;

    // input_t[0] is r
    // input_t[1] is s
    // r_squared = r**2
    fp_sqr(r_squared, input_t[0]);
    // s_squared = s**2
    fp_sqr(s_squared, input_t[1]);
    // r_cubed = r_squared * r
    fp_mul(r_cubed, r_squared, input_t[0]);
    // s_cubed = s_squared * s
    fp_mul(output_A[1], s_squared, input_t[1]);
    // aux = r * s_squared
    fp_mul(aux, s_squared, input_t[0]);

    // alpha_cubed = r * (r_squared - s_squared)
    fp_sub(alpha_cubed, r_squared, s_squared);
    fp_mul(alpha_cubed, alpha_cubed, input_t[0]);
    // alpha = setup['curt'](alpha_cubed)
    fp_curt(alpha, alpha_cubed);

    // Goal rd = 3 * (r * alpha**2 + r_squared * alpha + r_cubed) - s_sqr * alpha - 2 * aux
    //rd = (r * alpha**2 + r_squared * alpha + r_cubed)
    fp_mul(rd, input_t[0], alpha);
    fp_add(rd, rd, r_squared);
    fp_mul(rd, rd, alpha);
    fp_add(rd, rd, r_cubed);
    // rd = 3 * (r * alpha**2 + r_squared * alpha + r_cubed)
    fp_add(r_squared, rd, rd);
    fp_add(rd, rd, r_squared);
    // rd = 3 * (r * alpha**2 + r_squared * alpha + r_cubed) - s_sqr * alpha - 2 * aux
    fp_sub(rd, rd, aux);
    fp_sub(rd, rd, aux);
    fp_mul(r_squared, s_squared, alpha);
    fp_sub(output_A[0], rd, r_squared);
}

void isogeny_to_montgomery(fp output_A, const fp *input_t){
    fp r_squared, r_at_four, s_squared, s_at_four;
    fp num, den, aux;

    // input_t[0] is r
    // input_t[1] is s
    fp_sqr(r_squared, input_t[0]);
    fp_sqr(s_squared, input_t[1]);
    fp_sqr(r_at_four, r_squared);
    fp_sqr(s_at_four, s_squared);

    // num = -3 * r_at_four - 6 * r_squared * s_squared + s_at_four
    fp_add(aux, r_squared, r_squared);
    fp_mul(num, aux, s_squared);
    fp_add(num, num, r_at_four);
    fp_sub(s_at_four, s_at_four, num);
    fp_sub(s_at_four, s_at_four, num);
    fp_sub(num, s_at_four, num);

    // den = 4 * r_squared * r * s
    fp_add(aux, aux, aux);
    fp_mul(den, aux, input_t[0]);
    fp_mul(den, den, input_t[1]);
    fp_inv(den);
    fp_mul(output_A, den, num);
}

#ifdef _M5_
#define BOUND   (int)(1u << 4u)
#elif defined(_M4_)
#define BOUND   (int)(1u << 3u)
#elif defined(_M3_)
#define BOUND   (int)(1u << 2u)
#elif defined(_M2_)
#define BOUND   (int)(1u << 1u)
#else
#define BOUND   (int)(1u)
#endif

static uint64_t constant_time_is_lessthan_u64(uint64_t x, uint64_t y) {
    return (uint64_t) ((x ^ ((x ^ y) | ((x - y) ^ y))) >> 63);
}

void isogeny_walks_3_fp(fp output_A, const proj input_A, size_t input_length, uint8_t direction){
    (void)input_length;
    fp aux[2], num_den[3];

    isogeny_walks_get_points_3_fp(num_den, input_A);

    fp_cmov(&aux[0], (const fp*)num_den[0], direction);
    fp_cmov(&aux[0], (const fp*)num_den[1], direction ^ 1);
    fp_copy(aux[1], num_den[2]);

    for(int i = 0; i < BOUND; i++){
        fp t[2] = {0};
        uint64_t d = constant_time_is_lessthan_u64(i, input_length);
        isogeny_walks_to_montgomery_model_3_fp(t, (const fp*)aux);
        fp_cmov(&aux[0], (const fp*)t[0], d);
        fp_cmov(&aux[1], (const fp*)t[1], d);
    }
    isogeny_to_montgomery(output_A, (const fp*)aux);
}
