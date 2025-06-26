#if GLOBAL_COUNTERS != 0
#include <time.h>
#include <math.h>

#include "csidh.h"
#include "cycle.h"

#define NUM_FUNCTIONS 7

#if defined AVX2
#define TYPE "AVX2"
#elif defined GMP
#ifdef KARATSUBA
    #define TYPE "KARATSUBA"
#else
    #define TYPE "GMP"
#endif
#else
#define TYPE "MULX"
#endif

#if defined P2047m1l226
#define PRIME "I"

#elif defined P4095m27l262
#define PRIME "II"

#elif defined P5119m46l244
#define PRIME "III"

#elif defined P6143m59l262
#define PRIME "IV"

#elif defined P8191m78l338
#define PRIME "V"

#elif defined P9215m85l389
#define PRIME "VI"
#else
#error BITS must be 2047m1l226 or 4095m27l262 or 5119m46l244 or 6143m59l262 or 8191m78l338 or 9215m85l389
#endif

static inline void fp_inv_wrapper(fp c, const fp a, const fp b)
{
    (void)a;
    (void)b;
    fp_inv(c);
}

static inline void fp_sqr_wrapper(fp c, const fp a, const fp b)
{
    (void)b;
    fp_sqr(c, a);
}

static inline void fp_issquare_wrapper(fp c, const fp a, const fp b)
{
    (void)a;
    (void)b;
    fp_issquare(c);
}

#if defined AVX2
static inline void carry32_wrapper(fp c, const fp a, const fp b)
{
    int32_t carryover = 0;
    (void) carryover;
    (void)a;
    (void)b;
    carry32(c, NUMBER_OF_LIMBS, RADIX, &carryover);
    // for(int i=0; i<NUMBER_OF_LIMBS; i++)
    //     c[i] = 67219975;
    // for(int i=0; i<8; i++)
    //     c[i] += i << 26;        
    // carry32_avx2(c, 32);

}
#else
static inline void carry32_wrapper(fp c, const fp a, const fp b)
{
    (void)a;
    (void)b;  
    (void)c;
}
#endif

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <iters>\n", argv[0]);
        return 1;
    }            

    int i, its = atoi(argv[1]); // number of iterations
    if (its < 2)
    {
        printf("Number of iterations needs to be > 1\n");
        return 1;
    }

    // --------------------------
    // For measuring clock cycles
    ticks cc_mean = 0;
    ticks cc_sample[its], cc0, cc1;

    uint64_t add_mean = 0, sqr_mean = 0, mul_mean = 0;

    uint64_t add_sample[its],
        sqr_sample[its],
        mul_sample[its];

    // todo : "struct it"
    void (*fptr[])(fp c, const fp a, const fp b) = {fp_add, fp_sub, fp_mul, fp_sqr_wrapper, carry32_wrapper, fp_inv_wrapper, fp_issquare_wrapper};
    char * fname[NUM_FUNCTIONS] = {"add", "sub", "mul", "sqr", "carry", "inv", "issquare"};

    for (int j = 0; j < NUM_FUNCTIONS; j++)
    {
        for (i = 0; i < its; i++)
        {
            fp t0, t1, t2;
            fp_random(t1);
            fp_random(t2);
            fp_random(t0);
            init_counters();

            cc0 = getticks();
            (*fptr[j])(t0, t1, t2);
            cc1 = getticks();

            // ---------------------------------
            // storing the measured clock cycles
            cc_sample[i] = cc1 - cc0;
            // Average value
            cc_mean += cc_sample[i];

            // storing the meausred running-time
            add_sample[i] = fpadd;
            sqr_sample[i] = fpsqr;
            mul_sample[i] = fpmul;
            // Average values
            add_mean += add_sample[i];
            sqr_mean += sqr_sample[i];
            mul_mean += mul_sample[i];
        };
        // printf("[%3d%%] Done experiments\n", 100 * i / its);

        cc_mean = cc_mean / its;
        add_mean = add_mean / its;
        sqr_mean = sqr_mean / its;
        mul_mean = mul_mean / its;

        printf("\\newcommand{\\res" TYPE PRIME "%s}{%2.lf}\n", fname[j], (1.0 * cc_mean));
    }

    return 0;
}
#else
#include <stdio.h>

int main(int argc, char *argv[])
{
    fprintf(stderr, "Benchmarking is disabled because it requires global counters.\n");

    (void)argc; // suppress unused variable warnings
    (void)argv; // suppress unused variable warnings

    return 1;
}
#endif
