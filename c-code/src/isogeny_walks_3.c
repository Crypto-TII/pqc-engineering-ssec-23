//
// 3-isogeny walk calculations via radical computations
//

#include "isogeny_walks.h"
#include "utilities.h"
#include <stdio.h>


void isogeny_walks_3(fp2_t *output_a1, fp2_t *output_a3, fp2_t input_a1, fp2_t input_a3, const uint8_t *input_path, size_t input_length) {
    // This function assumes the input and output determines elliptic
    // curves of form y² + a₁xy + a₃y = x³ − 5a₁a₃x − (a₁³)a₃ − 7(a₃)²
    size_t i, j;
    fp2_t a, a1, a3, cube_roots_of_unity[3], r, s, t;

    fp2_set_to_one(&cube_roots_of_unity[0]);
    fp2_copy(&cube_roots_of_unity[1], *((fp2_t *)&CUBE_ROOT_OF_UNITY));
    fp2_sqr(&cube_roots_of_unity[2], cube_roots_of_unity[1]);

    fp2_copy(&a1, input_a1);
    fp2_copy(&a3, input_a3);

    uint32_t trit_string[input_length];
    to_trit_string(trit_string, input_path, input_length);

    for (i = 0; i < input_length; i++) {
        uint32_t mask = 0x0000000F;
        for (j = 0; j < 5; j++) {
            fp2_neg(&a, a3);
            fp2_curt(&a, a);
            fp2_linear_pass_in(&t, cube_roots_of_unity, 3, (trit_string[i] & mask) >> (4*j));
            fp2_mul(&a, a, t);

            // radical 3-isogeny
            fp2_add(&t, a, a);   // 2α
            fp2_add(&t, t, a);   // 3α
            fp2_add(&t, t, t);   // 6α

            fp2_add(&r, a1, a1); // 2a₁
            fp2_add(&r, r, a1);  // 3a₁
            fp2_sqr(&s, a1);     // a₁²

            fp2_sub(&a1, a1, t); // a₁ <- a₁ - 6α
            
            fp2_sqr(&t, a);      // α²
            fp2_mul(&r, r, t);   // 3a₁ · α²
            fp2_mul(&s, s, a);   // a₁² · α
            fp2_add(&t, a3, a3); // 2a₃
            fp2_add(&t, t, a3);  // 3a₃
            fp2_add(&a, t, t);   // 6a₃
            fp2_add(&t, a, t);   // 9a₃
            fp2_sub(&a3, r, s);  //  a₃ <- 3a₁ · α² - a₁² · α
            fp2_add(&a3, a3, t); //  a₃ <- 3a₁ · α² - a₁² · α + 9a₃

            mask <<= 4;
        }
    }
    fp2_copy(output_a1, a1);
    fp2_copy(output_a3, a3);
}

void isogeny_walks_from_montgomery_model_3(fp2_t *output_a1, fp2_t *output_a3, fp2_t input_A, fp2_t input_xP) {
    // inputs: x-coordinate of an order-3 point P on E: y² = x³ + Ax² + x, and A
    // output: isomorphic curve of form F: y² + a₁xy + a₃y = x³
    // Cost: 2M + 2S + 7A
    fp2_t r, s, xP_squared, A_times_xP;
    
    fp2_sqr(&xP_squared, input_xP);
    fp2_add(&r, xP_squared, xP_squared);
    fp2_add(&r, xP_squared, r);
    
    fp2_mul(&A_times_xP, input_A, input_xP); 
    fp2_add(&s, A_times_xP, A_times_xP);
    fp2_add(&r, r, s);

    fp2_set_to_one(&s);
    fp2_add(output_a1, r, s); // (3xₚ² + 2Axₚ + 1)

    fp2_add(output_a3, xP_squared, A_times_xP);
    fp2_add(output_a3, *output_a3, s);
    fp2_mul(output_a3, *output_a3, input_xP);
    fp2_sqr(output_a3, *output_a3);
    fp2_add(output_a3, *output_a3, *output_a3); // 2 yₚ⁴ = 2 (xₚ³ + Axₚ² + xₚ)²
}


void isogeny_walks_switch_from_model_3(fp2_t *output_j, fp2_t input_a1, fp2_t input_a3) {
    // Computes the j-invariant of an elliptic curve E : y² + a₁xy + a₃y = x³ − 5a₁a₃x − (a₁³)a₃ − 7(a₃)²
    fp2_t aux, tmp, num, den, a1_cube, a3_cube;

    fp2_sqr(&a1_cube, input_a1);
    fp2_sqr(&a3_cube, input_a3);
    fp2_mul(&a1_cube, a1_cube, input_a1);
    fp2_mul(&a3_cube, a3_cube, input_a3);

    fp2_add(&tmp, input_a3, input_a3); //         2 × a₃
    fp2_add(&tmp, tmp, input_a3);      //         3 × a₃
    fp2_add(&aux, tmp, tmp);           //         6 × a₃
    fp2_add(&aux, aux, aux);           //        12 × a₃

    fp2_add(&aux, aux, aux);           //        24 × a₃
    fp2_add(&aux, aux, tmp);           //        27 × a₃
    fp2_add(&tmp, aux, aux);           //        54 × a₃
    fp2_add(&tmp, tmp, tmp);           //       108 × a₃
    
    fp2_add(&tmp, tmp, tmp);           //       216 × a₃
    fp2_add(&num, a1_cube, tmp);       // a₁³ + 216 × a₃
    fp2_sub(&den, a1_cube, aux);       // a₁³ -  27 × a₃

    fp2_sqr(&tmp, num);
    fp2_sqr(&aux, den);
    
    fp2_mul(&num, num, tmp);          // (a₁³ + 216 × a₃)³
    fp2_mul(&den, den, aux);          // (a₁³ -  27 × a₃)³
    fp2_mul(&num, a1_cube, num);      // (a₁³ + 216 × a₃)³ × a₁³
    fp2_mul(&den, input_a3, den);     // (a₁³ -  27 × a₃)³ × a₃

    fp2_inv(&den, den);
    fp2_mul(output_j, num, den);

}


void isogeny_walks_get_points_3(fp2_t *output, fp2_t input_A) {
    fp2_t A_times_one_third, A_squared, r, s, t, u, v, y, s0, i0, s1, s2;
    fp_t THREE, EIGHT;
    
    fp_set_to_one(THREE);        // 1

    fp_add(EIGHT, THREE, THREE); // 2
    fp_add(THREE, THREE, EIGHT); // 3
    fp_add(EIGHT, EIGHT, EIGHT); // 4
    fp_add(EIGHT, EIGHT, EIGHT); // 8
    
    fp_mul(A_times_one_third.re, input_A.re, ONE_THIRD);
    fp_mul(A_times_one_third.im, input_A.im, ONE_THIRD);
    fp2_sqr(&A_squared, input_A);
    
    // t takes the value of ⅑(3 - A²)
    fp_sub(t.re, THREE, A_squared.re);
    fp_neg(t.im, A_squared.im);
    fp_mul(t.re, t.re, ONE_NINTH);
    fp_mul(t.im, t.im, ONE_NINTH);
    
    // r takes the value of (⁸⁄₂₇)A³
    fp2_mul(&r, A_squared, input_A);
    fp_mul(r.re, r.re, ONE_BY_27);
    fp_mul(r.im, r.im, ONE_BY_27);
    fp2_add(&r, r, r); //  2r
    fp2_add(&r, r, r); //  4r
    fp2_add(&r, r, r); //  8r
    fp2_add(&r, r, r); // 16r

    // s takes the calue of (⁸⁄₃)A
    fp2_copy(&s, A_times_one_third);
    fp2_add(&s, s, s); // 2s
    fp2_add(&s, s, s); // 4s
    fp2_add(&s, s, s); // 8s
    fp2_sub(&u, r, s); // r - s

    // v takes the value of -2A² + 8
    fp2_add(&v, A_squared, A_squared);
    fp_sub(v.re, EIGHT, v.re);
    fp_neg(v.im, v.im);
    fp2_curt(&y, v); // cube-root of v

    // y takes the value of t + (¹⁄₃)∛v
    fp_mul(y.re, y.re, ONE_THIRD);
    fp_mul(y.im, y.im, ONE_THIRD);
    fp2_add(&y, t, y);
    fp2_add(&r, y, y); // 2y

    fp2_add(&s, t, t); // 2t
    fp2_add(&s, t, s); // 3t
    fp2_add(&s, s, s); // 6t
    fp2_sub(&t, r, s); // r - s

    // s₀ takes the value of √(r - s)
    fp2_sqrt_slow(&s0, t);
    fp2_inv(&i0, s0); // 1 / s₀

    fp2_add(&v, r, s);  //   r + s
    fp2_neg(&v, v);     // -(r + s)
    fp2_mul(&t, u, i0); // u / s₀

    // s₁ takes the value of √[v + (u / s₀)]
    fp2_add(&s, v, t);  // v + (u / s₀)
    fp2_sqrt_slow(&s1, s);
    // s₂ takes the value of √[v - (u / s₀)]
    fp2_sub(&s, v, t);  // v - (u / s₀)
    fp2_sqrt_slow(&s2, s);

    fp2_sub(&r, s1, s0); // -s₀ + s₁
    fp2_add(&s, s1, s0); //  s₀ + s₁
    fp2_add(&t, s0, s2); //  s₀ + s₂
    fp2_sub(&u, s0, s2); //  s₀ - s₂

    fp2_neg(&s, s);      // -(s₀ + s₁)

    fp2_half(&r, r); // (¹⁄₂)(-s₀ + s₁)
    fp2_half(&s, s); // (¹⁄₂)(-s₀ - s₁)
    fp2_half(&t, t); // (¹⁄₂)( s₀ + s₂)
    fp2_half(&u, u); // (¹⁄₂)( s₀ - s₂)

    fp2_sub(&output[3], u, A_times_one_third); // r - (¹⁄₃)A
    fp2_sub(&output[2], t, A_times_one_third); // s - (¹⁄₃)A
    fp2_sub(&output[1], s, A_times_one_third); // t - (¹⁄₃)A
    fp2_sub(&output[0], r, A_times_one_third); // u - (¹⁄₃)A
}


void isogeny_walks_sample_trit_string(uint8_t *output) {
    uint8_t bound = 0xF3;
    for(int i = 0; i < (TRITLENGTH_PATH / 5); i++) {
        output[i] = 0x00;
        randombytes(&output[i], 1);
        while (issmaller((int32_t)bound, (int32_t)(output[i]))) {
            randombytes(&output[i], 1);
        }
    }
}
