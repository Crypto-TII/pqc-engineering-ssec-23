#include "fp-karatsuba.h"
#include "../fp-counters.h"
// #include "framework.h"
#include "../../primes.h"

#include <gmp.h>

//#define USE_GMP_SEC_FUNCTIONS
//#define MONTGOMERY

// #if defined P2047m1l226

#define pbits 2047
#define itch_size 128

// #endif

// const fp fp_0 = {0x0};

void fp_pow(fp b, const fp e, const fp a)
{
    (void) a;
    (void) b;
    (void) e;
}

/*
    see Algorithm 14.36 "Montgomery multiplication"
    https://cacr.uwaterloo.ca/hac/about/chap14.pdf
*/
void fp_mul(fp c, const fp a, const fp b)
{
// #if defined(P2047m1l226)
    uint64_t result[64] = {0};
    fp_mult_32x32(result, a, b);
    fp_word_redc(c, result);
    CNT_FP_MUL_INC();
}

void fp_add(fp c, const fp a, const fp b)
{

    fp_add_s(c, a, b);
    CNT_FP_ADD_INC();
}

void fp_sub(fp c, const fp a, const fp b)
{

    fp_sub_s(c, a, b);
    CNT_FP_ADD_INC();
}


void fp_sqr(fp b, const fp a)
{

    uint64_t result[64];
    fp_squaring(result, a, a);
    fp_word_redc(b, result);

    CNT_FP_SQR_INC();
}


#if defined(P5119m46l244) || defined(P6143m59l262) || defined(P8191m78l338) || defined(P9215m85l389)

void fp_mont_it_redc(fp a, const uint64_t b[2 * NUMBER_OF_WORDS])
{
        uint64_t r0[2 * NUMBER_OF_WORDS] = {0};
        uint64_t r1[2 * NUMBER_OF_WORDS] = {0};

        mult_redc(r0, b, redc_alpha);

        add_redc(r0, &b[E2], r0);
      
        mult_redc(r1, r0, redc_alpha);      

        add_redc_final(a, &r0[E2], r1);
}

#else

/*
    see Algorithm 14.32 "Montgomery reduction"        mult_redc(r0, b, redc_alpha);

        // // r0 = (a− q0)/2^e2 + q0 × alpha
        add_redc(r0, &b[78], r0);

        // // // q0
        // // memcpy(q1, r0, 78 * sizeof(uint64_t));

        mult_redc(r1, r0, redc_alpha);


        add_redc_final(a, &r0[78], r1);

    https://cacr.uwaterloo.ca/hac/about/chap14.pdf
*/
void fp_mont_redc(fp a, const uint64_t b[2 * NUMBER_OF_WORDS])
{

    static __thread uint64_t tp[itch_size];
    uint64_t A[2 * NUMBER_OF_WORDS + 1] = {0x0};
    // uint64_t a_i[1] = {0x0};
    uint64_t tmp_1[NUMBER_OF_WORDS + 1] = {0x0};

    // 1. A = T
    mpn_copyd(A, b, 2 * NUMBER_OF_WORDS);

    for (int i = 0; i < NUMBER_OF_WORDS; i++)
    {
        // 2.1 u_i = a_i * m' mod b
        // since montgomery friendly m' = 1
        // a_i[0] = A[i];

        // 2.2 tmp_1 = u_i * m
        mpn_sec_mul(tmp_1, p, NUMBER_OF_WORDS, &A[i], 1, tp);

        // 2.2 A = A + u_i * m * b^i
        mpn_add(A + i, A + i, 2 * NUMBER_OF_WORDS + 1 - i, tmp_1, NUMBER_OF_WORDS + 1);

    }

    // 3. A = A/b^n
    mpn_copyd(a, A + NUMBER_OF_WORDS, NUMBER_OF_WORDS);

    // 4. If A > m then A = A - m
    mpn_cnd_sub_n(mpn_cmp(a, p, NUMBER_OF_WORDS) > 0, a, a, p, NUMBER_OF_WORDS);
}




#endif

#if defined(P2047m1l226) 

// void fp_mont_2k(fp a, const uint64_t b[2 * NUMBER_OF_WORDS])
// {

//     static __thread uint64_t tp[itch_size];
//     uint64_t A[2 * NUMBER_OF_WORDS + 1] = {0x0};
//     // uint64_t a_i[1] = {0x0};
//     uint64_t tmp_1[NUMBER_OF_WORDS + 1] = {0x0};

//     // 1. A = T
//     mpn_copyd(A, b, 2 * NUMBER_OF_WORDS);

//     for (int i = 0; i < NUMBER_OF_WORDS; i++)
//     {

//         memset(tmp_1, 0, sizeof(tmp_1));
//         // 2.2 tmp_1 = u_i * m
//         mpn_sec_mul(tmp_1, redc_alpha, NUMBER_OF_WORDS - 1, A, 1, tp);

//         // mpn_sec_mul(tmp_1, p, NUMBER_OF_WORDS, &A[i], 1, tp);

//         // mpn_sub(A, A, 1, A, 1);

//         // mpn_copyd(A, A + 1, 2 * NUMBER_OF_WORDS - i);

//         // 2.2 A = A + u_i * m * b^i
//         mpn_add(A, A + 1, 2 * NUMBER_OF_WORDS - i, tmp_1, NUMBER_OF_WORDS);

//     }

//     // 3. A = A/b^n
//     mpn_copyd(a, A, NUMBER_OF_WORDS);

//     // 4. If A > m then A = A - m
//     mpn_cnd_sub_n(mpn_cmp(a, p, NUMBER_OF_WORDS) > 0, a, a, p, NUMBER_OF_WORDS);
// }

#endif

#if defined(P4095m27l262) 

void fp_mont_4k(fp a, const uint64_t b[2 * NUMBER_OF_WORDS])
{

    static __thread uint64_t tp[itch_size];
    uint64_t A[2 * NUMBER_OF_WORDS + 1] = {0x0};
    // uint64_t a_i[1] = {0x0};
    uint64_t tmp_1[NUMBER_OF_WORDS + 1] = {0x0};

    // 1. A = T
    mpn_copyd(A, b, 2 * NUMBER_OF_WORDS);

    for (int i = 0; i < NUMBER_OF_WORDS; i++)
    {
        // 2.1 u_i = a_i * m' mod b
        // since montgomery friendly m' = 1
        // a_i[0] = A[i];

        memset(tmp_1, 0, sizeof(tmp_1));
        // 2.2 tmp_1 = u_i * m
        mpn_sec_mul(tmp_1, redc_alpha, 38, A, 1, tp);

        // mpn_sec_mul(tmp_1, p, NUMBER_OF_WORDS, &A[i], 1, tp);

        mpn_sub(A, A, 1, A, 1);

        mpn_copyd(A, A + 1, NUMBER_OF_WORDS - i);

        // 2.2 A = A + u_i * m * b^i
        mpn_add(A, A, 2 * NUMBER_OF_WORDS + 1 - i, tmp_1, 38);

    }

    // 3. A = A/b^n
    mpn_copyd(a, A, NUMBER_OF_WORDS);

    // 4. If A > m then A = A - m
    mpn_cnd_sub_n(mpn_cmp(a, p, NUMBER_OF_WORDS) > 0, a, a, p, NUMBER_OF_WORDS);
}

#endif
