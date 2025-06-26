//
// 2-isogeny walk calculations via radical computations
//

#include <string.h>
#include "isogeny_walks.h"
#include "utilities.h"
// #include <stdio.h>

// 2-isogenies
void fp2_mul_by_1488(fp2_t *output, fp2_t input) {
    fp2_t r, s, t;
    
    fp2_add(&r, input, input); //    2 × input
    fp2_add(&s, r, input);     //    3 × input
    fp2_add(&t, r, s);         //    5 × input
    fp2_add(&t, t, t);         //   10 × input

    fp2_add(&t, t, t);         //   20 × input
    fp2_add(&t, t, s);         //   23 × input
    fp2_add(&t, t, t);         //   46 × input
    fp2_add(&t, t, t);         //   92 × input

    fp2_add(&t, t, input);     //   93 × input
    fp2_add(&t, t, t);         //  186 × input
    fp2_add(&t, t, t);         //  372 × input
    fp2_add(&t, t, t);         //  744 × input

    fp2_add(output, t, t);     // 1488 × input
}

void fp2_mul_by_2976(fp2_t *output, fp2_t input) {
    fp2_mul_by_1488(output, input);    // 1488 × input
    fp2_add(output, *output, *output); // 2976 × input
}

void fp2_mul_by_162000(fp2_t *output, fp2_t input) {
    fp2_t r, s, t;
    fp2_add(&r, input, input); //      2 × input
    fp2_add(&r, r, r);         //      4 × input
    fp2_add(&r, r, r);         //      8 × input
    fp2_add(&r, r, input);     //      9 × input

    fp2_add(&s, r, r);         //     18 × input
    fp2_add(&s, s, s);         //     36 × input
    fp2_add(&s, s, s);         //     72 × input
    fp2_add(&t, s, r);         //     81 × input

    fp2_add(&s, t, s);         //    153 × input
    fp2_add(&s, s, s);         //    306 × input
    fp2_add(&s, s, s);         //    612 × input
    fp2_add(&s, s, s);         //   1224 × input

    fp2_add(&s, s, s);         //   2448 × input
    fp2_add(&t, s, t);         //   2529 × input
    fp2_add(&t, t, t);         //   5058 × input
    fp2_add(&t, t, t);         //  10116 × input

    fp2_add(&t, t, r);         //  10125 × input
    fp2_add(&t, t, t);         //  20250 × input
    fp2_add(&t, t, t);         //  40500 × input
    fp2_add(&t, t, t);         //  81000 × input
    
    fp2_add(output, t, t);     // 162000 × input
}

void fp2_mul_by_324000(fp2_t *output, fp2_t input) {
    fp2_mul_by_162000(output, input);  // 162000 × input
    fp2_add(output, *output, *output); // 324000 × input
}

void fp2_mul_by_2532192(fp2_t *output, fp2_t input) {
    fp2_t r, s, t, u, v;

    fp2_add(&r, input, input); //       2 × input
    fp2_add(&s, r, input);     //       3 × input
    fp2_add(&t, s, s);         //       6 × input
    fp2_add(&t, t, t);         //      12 × input
    
    fp2_add(&t, t, t);         //      24 × input
    fp2_add(&u, t, s);         //      27 × input
    fp2_add(&v, u, t);         //      51 × input
    fp2_add(&t, v, v);         //     102 × input

    fp2_add(&t, t, v);         //     153 × input
    fp2_add(&t, t, t);         //     306 × input
    fp2_add(&t, t, s);         //     309 × input
    fp2_add(&t, t, t);         //     618 × input

    fp2_add(&t, t, t);         //    1236 × input
    fp2_add(&t, t, t);         //    2472 × input
    fp2_add(&t, t, t);         //    4944 × input
    fp2_add(&t, t, t);         //    9888 × input

    fp2_add(&t, t, t);         //   19776 × input
    fp2_add(&t, t, t);         //   39552 × input
    fp2_add(&t, t, t);         //   79104 × input
    fp2_add(&t, t, u);         //   79131 × input

    fp2_add(&t, t, t);         //  158262 × input
    fp2_add(&t, t, t);         //  316524 × input
    fp2_add(&t, t, t);         //  633048 × input
    fp2_add(&t, t, t);         // 1266096 × input
    
    fp2_add(output, t, t);     // 2532192 × input
}

void fp2_mul_by_645205500(fp2_t *output, fp2_t input) {

    fp2_t r, s, t, u, v;
    fp2_add(&r, input, input); //          2 × input
    fp2_add(&r, r, r);         //          4 × input
    fp2_add(&s, r, input);     //          5 × input
    fp2_add(&t, s, s);         //         10 × input

    fp2_add(&t, t, t);         //         20 × input
    fp2_add(&r, t, t);         //         40 × input
    fp2_add(&t, r, r);         //         80 × input
    fp2_add(&u, t, s);         //         85 × input

    fp2_add(&v, t, u);         //        165 × input
    fp2_add(&v, v, r);         //        205 × input
    fp2_add(&t, v, v);         //        410 × input
    fp2_add(&t, t, t);         //        820 × input

    fp2_add(&t, t, t);         //       1640 × input
    fp2_add(&t, t, t);         //       3280 × input
    fp2_add(&t, t, t);         //       6560 × input
    fp2_add(&t, t, t);         //      13120 × input

    fp2_add(&t, t, t);         //      26240 × input
    fp2_add(&t, t, t);         //      52480 × input
    fp2_add(&t, t, t);         //     104960 × input
    fp2_add(&t, t, t);         //     209920 × input

    fp2_add(&t, t, t);         //     419840 × input
    fp2_add(&t, t, v);         //     420045 × input
    fp2_add(&t, t, t);         //     840090 × input
    fp2_add(&t, t, t);         //    1680180 × input

    fp2_add(&t, t, t);         //    3360360 × input
    fp2_add(&t, t, u);         //    3360445 × input
    fp2_add(&t, t, t);         //    6720890 × input
    fp2_add(&t, t, t);         //   13441780 × input

    fp2_add(&t, t, t);         //   26883560 × input
    fp2_add(&t, t, t);         //   53767120 × input
    fp2_add(&t, t, s);         //   53767125 × input
    fp2_add(&u, t, t);         //  107534250 × input
    
    fp2_add(&t, t, u);         //  161301375 × input
    fp2_add(&t, t, t);         //  322602750 × input
    fp2_add(output, t, t);     //  645205500 × input
}

void fp2_mul_by_8748000000(fp2_t *output, fp2_t input) {
    fp2_t r, s, t, u, v;
    fp2_add(&r, input, input); //          2 × input
    fp2_add(&r, r, r);         //          4 × input
    fp2_add(&r, r, r);         //          8 × input
    fp2_add(&r, r, r);         //         16 × input

    fp2_add(&r, r, r);         //         32 × input
    fp2_add(&r, r, r);         //         64 × input
    fp2_add(&r, r, r);         //        128 × input
    fp2_add(&s, r, input);     //        129 × input

    fp2_add(&r, s, r);         //        257 × input
    fp2_add(&r, r, r);         //        514 × input
    fp2_add(&r, r, r);         //       1028 × input
    fp2_add(&t, r, r);         //       2056 × input

    fp2_add(&t, t, t);         //       4112 × input
    fp2_add(&t, t, t);         //       8224 × input
    fp2_add(&u, t, s);         //       8353 × input
    fp2_add(&v, t, r);         //       9252 × input

    fp2_add(&t, u, v);         //      17605 × input
    fp2_add(&t, t, t);         //      35210 × input
    fp2_add(&t, t, v);         //      44462 × input
    fp2_add(&t, t, t);         //      88924 × input

    fp2_add(&t, t, t);         //     177848 × input
    fp2_add(&t, t, t);         //     355696 × input
    fp2_add(&t, t, t);         //     711392 × input
    fp2_add(&t, t, t);         //    1422784 × input

    fp2_add(&t, t, t);         //    2845568 × input
    fp2_add(&t, t, t);         //    5691136 × input
    fp2_add(&t, t, t);         //   11382272 × input
    fp2_add(&t, t, u);         //   11390625 × input

    fp2_add(&u, t, t);         //   22781250 × input
    fp2_add(&t, t, u);         //   34171875 × input
    fp2_add(&t, t, t);         //   68343750 × input
    fp2_add(&t, t, t);         //  136687500 × input

    fp2_add(&t, t, t);         //  273375000 × input
    fp2_add(&t, t, t);         //  546750000 × input
    fp2_add(&t, t, t);         // 1093500000 × input
    fp2_add(&t, t, t);         // 2187000000 × input
    
    fp2_add(&t, t, t);         // 4374000000 × input
    fp2_add(output, t, t);     // 8748000000 × input
}


void isogeny_walks_2(fp2_t *output, fp2_t input_prev, fp2_t input, const uint8_t *input_path, size_t input_length) {
    size_t i, j;
    fp2_t t4, t3, t2, t1, t0, jk_next, jk, jk_prev, Dk, Dk_sqrt[2];
    fp2_t constant_162000, constant_2532192, constant_645205500, constant_8748000000;
    fp2_t jk_times_2976, jk_prev_times_2976, jk_prev_times_324000, jk_squared, jk_prev_squared;

    fp2_set_to_one(&constant_162000);
    fp2_set_to_one(&constant_2532192);
    fp2_set_to_one(&constant_645205500);
    fp2_set_to_one(&constant_8748000000);

    fp2_mul_by_162000(&constant_162000, constant_162000);
    fp2_mul_by_2532192(&constant_2532192, constant_2532192);
    fp2_mul_by_645205500(&constant_645205500, constant_645205500);
    fp2_mul_by_8748000000(&constant_8748000000, constant_8748000000);

    fp2_copy(&jk, input);
    fp2_copy(&jk_prev, input_prev);

    for (i = 0; i < input_length; i++) {
        uint8_t mask = 0x1;
        for (j = 0; j < 8; j++) {
            // jk⁴
            fp2_sqr(&jk_squared, jk);
            fp2_sqr(&t4, jk_squared);
            // - 2976 × (jk³) [= - (2976  ×  jk)  ×  (jk²) ]
            fp2_mul_by_2976(&jk_times_2976, jk);
            fp2_mul(&t3, jk_times_2976, jk_squared);
            // + 2 × (jk²) × jk_prev + 2532192 × (jk²) [= +  [2 × jk_prev + 2532192]  ×  (jk²)]
            fp2_add(&t2, jk_prev, jk_prev);
            fp2_add(&t2, t2, constant_2532192);
            fp2_mul(&t2, t2, jk_squared);
            // - 2976 × jk × jk_prev - 645205500 × jk      [= -(2976 × jk_prev + 645205500)  ×  jk ]
            fp2_mul_by_2976(&jk_prev_times_2976, jk_prev);
            fp2_add(&t1, jk_prev_times_2976, constant_645205500);
            fp2_mul(&t1, t1, jk);
            // - 3 × (jk_prev²) + 324000 × jk_prev [= (-3  ×  jk_prev + 324000)  ×  jk_prev]
            fp2_mul_by_324000(&jk_prev_times_324000, jk_prev);
            fp2_sqr(&jk_prev_squared, jk_prev);
            fp2_add(&t0, jk_prev_squared, jk_prev_squared);
            fp2_add(&t0, t0, jk_prev_squared);
            fp2_sub(&t0, jk_prev_times_324000, t0);
            //  - 8748000000

            // Dk = jk⁴ - 2976 × (jk³) + 2 × (jk²) × jk_prev + 2532192 × (jk²) - 2976 × jk × jk_prev - 645205500 × jk - 3 × (jk_prev²) + 324000 × jk_prev - 8748000000
            fp2_sub(&Dk, t4, t3);
            fp2_add(&Dk, Dk, t2);
            fp2_sub(&Dk, Dk, t1);
            fp2_add(&Dk, Dk, t0);
            fp2_sub(&Dk, Dk, constant_8748000000);

            fp2_sqrt_fast(&Dk_sqrt[0], Dk);   // 1st square-root (deterministic)
            fp2_neg(&Dk_sqrt[1], Dk_sqrt[0]); // 2nd square-root (negative)
            fp2_linear_pass_in(&Dk, Dk_sqrt , 2, (input_path[i] & mask) >> j);

            // jk_next = (jk² - 1488 × jk - jk_prev + 162000 + Dk_sqrt[path[k]]) / 2
            fp2_half(&t0, jk_times_2976); // 2976 = 2 × 1488
            fp2_sub(&t1, jk_squared, t0);
            fp2_sub(&t1, t1, jk_prev);
            fp2_add(&t1, t1, constant_162000);
            fp2_add(&t1, t1, Dk);
            fp2_half(&jk_next, t1);

            fp2_copy(&jk_prev, jk);
            fp2_copy(&jk, jk_next);
            
            mask <<= 1;
        }
    }
    fp2_copy(output, jk);
}

void isogeny_walks_from_montgomery_model_2(fp2_t *output_j, fp2_t input_A) {
    // Computes the j-invariant of a Montgomery curve E : y² = x³ + Ax² + x
    fp2_t r, s, t;

    fp2_set_to_one(&t);

    fp2_add(&r, t, t);    // 2
    fp2_add(&r, r, t);    // 3
    fp2_sqr(&s, input_A); // A²

    fp2_sub(&s, s, r);    // A² - 3
    fp2_sub(&r, s, t);    // A² - 4
    fp2_add(&s, s, s);    // 2(A² - 3)
    fp2_add(&s, s, s);    // 4(A² - 3)

    fp2_sqr(&t, s);       // 16(A² - 3)²
    fp2_mul(&s, s, t);    // 64(A² - 3)³
    fp2_add(&s, s, s);    // 128(A² - 3)³
    fp2_add(&s, s, s);    // 256(A² - 3)³
    fp2_inv(&t, r);       // 1 / (A² - 4)
    fp2_mul(output_j, s, t);
}

void isogeny_walks_get_previous_step_2(fp2_t *output_j, fp2_t input_A) {
    // Computes the j-invariant of a Montgomery curve E : y² = x³ + (A + 6)x² + (4[2 + A])x
    fp2_t r, s, t;

    fp2_set_to_one(&t);

    fp2_add(&r, t, t);    //  2
    fp2_add(&t, r, r);    //  4
    fp2_add(&r, t, t);    //  8
    fp2_add(&r, t, r);    // 12

    fp2_sqr(&s, input_A); // A²
    fp2_add(&s, s, r);    // A² + 12
    fp2_sub(&r, s, t);    // A² - 4
    fp2_add(&s, s, s);    // 2(A² + 12)

    fp2_sqr(&r, r);       // (A² - 4)²
    fp2_sqr(&t, s);       //  4(A² + 12)²
    fp2_mul(&s, s, t);    //  8(A² + 12)³
    fp2_add(&s, s, s);    // 16(A² + 12)³

    fp2_inv(&t, r);       // 1 / (A² - 4)²
    fp2_mul(output_j, s, t);
}


void isogeny_walks_sample_bit_string(uint8_t *output) {
    randombytes(output, BIT_LENGTH_PATH / 8);
}

void isogeny_walks_2_slow(fp2_t *j0, fp2_t *j1, fp2_t input_prev, fp2_t input, const uint8_t *input_path, size_t input_length) {
    size_t i, j;
    fp2_t t4, t3, t2, t1, t0, jk_next, jk, jk_prev, Dk, Dk_sqrt[2];
    fp2_t constant_162000, constant_2532192, constant_645205500, constant_8748000000;
    fp2_t jk_times_2976, jk_prev_times_2976, jk_prev_times_324000, jk_squared, jk_prev_squared;

    fp2_set_to_one(&constant_162000);
    fp2_set_to_one(&constant_2532192);
    fp2_set_to_one(&constant_645205500);
    fp2_set_to_one(&constant_8748000000);

    fp2_mul_by_162000(&constant_162000, constant_162000);
    fp2_mul_by_2532192(&constant_2532192, constant_2532192);
    fp2_mul_by_645205500(&constant_645205500, constant_645205500);
    fp2_mul_by_8748000000(&constant_8748000000, constant_8748000000);

    fp2_copy(&jk, input);
    fp2_copy(&jk_prev, input_prev);

    for (i = 0; i < input_length; i++) {
        uint8_t mask = 0x1;
        for (j = 0; j < 8; j++) {
            // jk⁴
            fp2_sqr(&jk_squared, jk);
            fp2_sqr(&t4, jk_squared);
            // - 2976 × (jk³) [= - (2976  ×  jk)  ×  (jk²) ]
            fp2_mul_by_2976(&jk_times_2976, jk);
            fp2_mul(&t3, jk_times_2976, jk_squared);
            // + 2 × (jk²) × jk_prev + 2532192 × (jk²) [= +  [2 × jk_prev + 2532192]  ×  (jk²)]
            fp2_add(&t2, jk_prev, jk_prev);
            fp2_add(&t2, t2, constant_2532192);
            fp2_mul(&t2, t2, jk_squared);
            // - 2976 × jk × jk_prev - 645205500 × jk      [= -(2976 × jk_prev + 645205500)  ×  jk ]
            fp2_mul_by_2976(&jk_prev_times_2976, jk_prev);
            fp2_add(&t1, jk_prev_times_2976, constant_645205500);
            fp2_mul(&t1, t1, jk);
            // - 3 × (jk_prev²) + 324000 × jk_prev [= (-3  ×  jk_prev + 324000)  ×  jk_prev]
            fp2_mul_by_324000(&jk_prev_times_324000, jk_prev);
            fp2_sqr(&jk_prev_squared, jk_prev);
            fp2_add(&t0, jk_prev_squared, jk_prev_squared);
            fp2_add(&t0, t0, jk_prev_squared);
            fp2_sub(&t0, jk_prev_times_324000, t0);
            //  - 8748000000

            // Dk = jk⁴ - 2976 × (jk³) + 2 × (jk²) × jk_prev + 2532192 × (jk²) - 2976 × jk × jk_prev - 645205500 × jk - 3 × (jk_prev²) + 324000 × jk_prev - 8748000000
            fp2_sub(&Dk, t4, t3);
            fp2_add(&Dk, Dk, t2);
            fp2_sub(&Dk, Dk, t1);
            fp2_add(&Dk, Dk, t0);
            fp2_sub(&Dk, Dk, constant_8748000000);

            fp2_sqrt_slow(&Dk_sqrt[0], Dk);   // 1st square-root (deterministic)
            fp2_neg(&Dk_sqrt[1], Dk_sqrt[0]); // 2nd square-root (negative)
            fp2_linear_pass_in(&Dk, Dk_sqrt , 2, (input_path[i] & mask) >> j);

            // jk_next = (jk² - 1488 × jk - jk_prev + 162000 + Dk_sqrt[path[k]]) / 2
            fp2_half(&t0, jk_times_2976); // 2976 = 2 × 1488
            fp2_sub(&t1, jk_squared, t0);
            fp2_sub(&t1, t1, jk_prev);
            fp2_add(&t1, t1, constant_162000);
            fp2_add(&t1, t1, Dk);
            fp2_half(&jk_next, t1);

            fp2_copy(&jk_prev, jk);
            fp2_copy(&jk, jk_next);
            
            mask <<= 1;
        }
    }
    fp2_copy(j0, jk_prev);
    fp2_copy(j1, jk);
}
