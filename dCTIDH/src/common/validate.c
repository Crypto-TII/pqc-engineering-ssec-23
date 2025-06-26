#include <string.h>
#include <assert.h>

#include "../CTIDH/ctidh.h"
#include "primes.h"
#include "mont.h"
#include "elligator.h"
// #include "fp2.h"

#ifdef ENABLE_CT_TESTING
#include <valgrind/memcheck.h>
#endif

// static void fp2_print(fp2 a){
//     printf("0x%016lX,", A[i]);
    
//     fp_print(a.re);
//     fp_print(a.im);
// }

// wombat validate by checking full order point
bool validate(public_key const *in){
// #if !defined(_M1_) && !defined(_M2_) && !defined(_M3_) && !defined(_M4_) && !defined(_M5_)
//     printf("\nNOT using radical 3-isogenies.\n\n");
// #else
//     printf("\nUsing 2^%ld radical 3-isogenies.\n\n", batch_start[0] - 1);
// #endif
    proj A, A24;
    fp_copy(A.x, in->A);
    fp_copy(A.z, fp_1);
    xA24(&A24, &A);

    proj Pp, Pm;

#ifdef ENABLE_CT_TESTING    
    memset(&Pp, 0, sizeof(proj));
    memset(&Pp.z, 0, sizeof(proj));
    VALGRIND_MAKE_MEM_DEFINED(&Pp.z, sizeof(fp));      
    VALGRIND_MAKE_MEM_DEFINED(&Pp, sizeof(proj)); 
    VALGRIND_MAKE_MEM_DEFINED(&A24, sizeof(proj));  
#endif 

    fp seed;
    fp_set(seed, in->seed);
    fp_enc(seed, seed);
    elligator_seeded(&Pp, &Pm, &A24, (const fp *)seed);


    for (int64_t j = 0; j < two_cofactor; j++)
    {
        xDBL(&Pp, &Pp, &A24, 0);
    }
    // clear ells replaced by radical 3-isogenies 
    for (int16_t j = 0; j < batch_start[0]; j++)
    {
      xMUL_dac(&Pp, &A24, 0, &Pp, primes_dac[j], primes_daclen[j], primes_daclen[j]);
    }
    // remove ells and check that we do not reach the point at infinity
    for(int i = primes_num-1; i > batch_start[0]; i--)
    {
        xMUL_dac(&Pp, &A24, 0, &Pp, primes_dac[i], primes_daclen[i], primes_daclen[i]);
        if(fp_iszero(Pp.z))
            return false;
    }

    // after removing the last ell, now we should have the point at infinity
    xMUL_dac(&Pp, &A24, 0, &Pp, primes_dac[batch_start[0]], primes_daclen[batch_start[0]], primes_daclen[batch_start[0]]);
    return fp_iszero(Pp.z);
}


////
//// VALIDATE OLD 
////

// For computing [(p + 1) / l_i]P, i:=0, ..., (N - 1)
void cofactor_multiples(proj P[], proj const A, size_t lower, size_t upper)
{
    assert(lower < upper);
    if (upper - lower == 1)
        return;

    int i;
    size_t mid = lower + (upper - lower + 1) / 2;

    // proj_copy(P[mid], (const fp*)P[lower]);
    fp_copy(P[mid].x, P[lower].x);
    fp_copy(P[mid].z, P[lower].z);

    for (i = lower; i < (int)mid; i++)
        xMUL_dac(&P[mid], &A, 1, &P[mid], primes_dac[i], primes_daclen[i], primes_daclen[i]);
    // xmul(P[mid], i, (const fp*)P[mid], A);

    for (i = (int)mid; i < (int)upper; i++)
        xMUL_dac(&P[lower], &A, 1, &P[lower], primes_dac[i], primes_daclen[i], primes_daclen[i]);
    // xmul(P[lower], i, (const fp*)P[lower], A);

    cofactor_multiples(P, A, lower, mid);
    cofactor_multiples(P, A, mid, upper);
}


// #if defined(P511) || defined(P512) || defined(P2047m1l226) || defined(P4095m27l262)

// // output: true if key is valid
// // output: false if key is invalid
// bool validate(public_key const *in)
// {

//     fp x;

//     int number_of_primes = primes_num;
//     proj P[primes_num] = {0};

//     proj A;
//     // A = (a : 1)
//     fp_copy(A.x, in->A);
//     fp_set1(A.z);

//     // Coding curve coefficients as (A' + 2C : 4C)
//     fp_add(A.z, A.z, A.z); // 2C
//     fp_add(A.x, A.x, A.z); // A' + 2C
//     fp_add(A.z, A.z, A.z); // 4C

//     int i;
//     uint64_t bits;

//     do
//     {
//         bits = 0;
//         // P = (x : 1)
//         fp_random(x);
//         fp_copy(P[0].x, x);
//         fp_set1(P[0].z);

//         // Multiplying by the cofactor
//         for (int64_t j = 0; j < two_cofactor; j++)
//         {
//             xDBL(&P[0], &P[0], &A, 1);
//         } 

//         // At this step, P[0] is expected to be a torsion-([p + 1]/[2^e]) point
//         cofactor_multiples(P, A, 0, number_of_primes);
//         for (i = number_of_primes - 1; i >= 0; i--)
//         {
//             // we only gain information if [(p+1)/l] P is non-zero
//             if (fp_iszero(P[i].z) != 1)
//             {
//                 // xmul(P[i], i, (const fp *)P[i], (const fp *)A);
//                 // printf("prime[i] = %lld\n", primes[i]);
//                 xMUL_dac(&P[i], &A, 1, &P[i], primes_dac[i], primes_daclen[i], primes_daclen[i]);

//                 // P does not have order dividing p + 1?
//                 if (fp_iszero(P[i].z) != 1)
//                     return false;

//                 // If bits > UPPER_BOUND, hence definitely supersingular
//                 bits += primes_daclen[i];
//                 if (bits > UPPER_BOUND)
//                     return true;
//             };
//         };

//     } while (1);
// }

// #else

// bool validate(public_key const *in)
// {
//     proj P;
//     proj A;
// #ifndef CTIDH
//     int number_of_primes = N;  
// #else
//     int number_of_primes = primes_num;
// #endif 
//     int ord = 0;

//     fp_copy(A.x, in->A);
//     fp_set1(A.z);

//     // Coding curve coefficients as (A' + 2C : 4C)
//     fp_add(A.z, A.z, A.z); // 2C
//     fp_add(A.x, A.x, A.z); // A' + 2C
//     fp_add(A.z, A.z, A.z); // 4C

//     //fp_copy(P.x, p_minus_2);
//     fp_set1(P.x);
//     fp_add(P.x, P.x, P.x);      //x = -2
//     fp_set1(P.z);

//     // at this point, P is the point (2, -) on E_A
//     // this point should have enough 2-torsion!

//     //xmul_dac or xMUL as long as it kills all ell_i
//     //@sopmac: does this work?
//     // for any reviewer reading this carefully: a highly optimized version should perform
//     // a Montgomery ladder using X0 = 2 of length prod ell_i here
//     for (int i = 0; i < (int) number_of_primes; i++)
//         xMUL_dac(&P, &A, 1, &P, primes_dac[i], primes_daclen[i], primes_daclen[i]);
    
//     //we kill of all ell_i, so P at this point should just
//     //be a point of order 2^z. We check z by doing doublings until the point is infinity
//     // as long as z is large enough (probabilisitically always true for our primes)
//     // this verifies supersingularity, by verifying a point of order 2^z > 4 sqrt(p)

//     //@sopmac: are we sure this is the right amount of doubling
//     // and not one too many?
//     while (fp_iszero(P.z) == 0 && ord < two_cofactor)
//     {
//         xDBL(&P, &P, &A, 1);
//         ord++;
//     }

//     // now if P is inf we not that the 2-torsion of P was ord
//     if (fp_iszero(P.z) == 1 && ord > (NUMBER_OF_WORDS*64)/2 + 1)
//     {
//         return true;    
//     }

//     return false;

// }

// #endif
