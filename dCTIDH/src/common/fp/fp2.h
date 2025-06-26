#ifndef _FP2_H
#define _FP2_H

#include "../namespace.h"

#if defined AVX2
    #include "avx2/fp-avx2.h"
#elif defined GMP
    #include "gmp/fp-gmp.h"
#elif defined KARATSUBA
    #include "karatsuba/fp-karatsuba.h"
#else
    #include "mulx/fp.h"
#endif

typedef struct fp2
{
    fp re, im;
} fp2;

#define fp2_copy COMMON(fp2_copy)
void fp2_copy(fp2 *x, const fp2 *y);

#define fp2_add COMMON(fp2_add)
void fp2_add(fp2 *x, const fp2 *y, const fp2 *z);

#define fp2_sub COMMON(fp2_sub)
void fp2_sub(fp2 *x, const fp2 *y, const fp2 *z);

#define fp2_neg COMMON(fp2_neg)
void fp2_neg(fp2 *x, const fp2 *y);

#define fp2_mul COMMON(fp2_mul)
void fp2_mul(fp2 *x, const fp2 *y, const fp2 *z);

#define fp2_sqr COMMON(fp2_sqr)
void fp2_sqr(fp2 *x, const fp2 *y);

#define fp2_inv COMMON(fp2_inv)
void fp2_inv(fp2 *x);

#define fp2_test COMMON(fp2_test)
void fp2_test(void);

#endif /* !defined(_FP2_H) */