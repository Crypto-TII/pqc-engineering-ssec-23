#include <string.h>
#include <assert.h>

#include "fp2.h"

#ifdef ENABLE_CT_TESTING
#include <valgrind/memcheck.h>
#endif

void fp2_copy(fp2 *x, const fp2 *y)
{
    fp_copy(x->re, y->re);
    fp_copy(x->im, y->im);
}

void fp2_add(fp2 *x, const fp2 *y, const fp2 *z)
{
    fp_add3(&(x->re), &(y->re), &(z->re));
    fp_add3(&(x->im), &(y->im), &(z->im));
}

void fp2_sub(fp2 *x, const fp2 *y, const fp2 *z)
{
    fp_sub3(&(x->re), &(y->re), &(z->re));
    fp_sub3(&(x->im), &(y->im), &(z->im));
}

void fp2_neg(fp2 *x, const fp2 *y)
{
    fp_neg2(&(x->re), &(y->re));
    fp_neg2(&(x->im), &(y->im));
}

void fp2_mul(fp2 *x, const fp2 *y, const fp2 *z)
{
    fp t0, t1;

    fp_add3(&t0, &(y->re), &(y->im));
    fp_add3(&t1, &(z->re), &(z->im));
    fp_mul3(&t0, (const fp *)t0, (const fp *)t1);
    fp_mul3(&t1, &(y->im), &(z->im));
    fp_mul3(&(x->re), &(y->re), &(z->re));
    fp_sub3(&(x->im), (const fp *)t0, (const fp *)t1);
    fp_sub3(&(x->im), (const fp *)(x->im), (const fp *)(x->re));
    fp_sub3(&(x->re), (const fp *)(x->re), (const fp *)t1);
}

void fp2_sqr(fp2 *x, const fp2 *y)
{
    fp sum, diff;

    fp_add3(&sum, &(y->re), &(y->im));
    fp_sub3(&diff, &(y->re), &(y->im));
    fp_mul3(&(x->im), &(y->re), &(y->im));
    fp_add3(&(x->im), (const fp *)(x->im), (const fp *)(x->im));
    fp_mul3(&(x->re), (const fp *)sum, (const fp *)diff);
}

void fp2_inv(fp2 *x)
{
    fp t0, t1;

    fp_sqr(t0, x->re);
    fp_sqr(t1, x->im);
    fp_add3(&t0, (const fp *)t0, (const fp *)t1);
    fp_inv(t0);
    fp_mul3(&(x->re), (const fp *)x->re, (const fp *)t0);
    fp_mul3(&(x->im), (const fp *)x->im, (const fp *)t0);
    fp_neg1(&(x->im));
}

void fp2_test(void)
{

    fp2 a, b, c;

    for (int64_t i = 0; i <= 10000; i++)
    {
        fp_random(a.im);
        fp_random(a.re);

        fp2_copy(&b, &a);
        fp2_inv(&a);
        fp2_mul(&c, &a, &b);
        assert(fp_isone(c.re) == 1);
        assert(fp_iszero(c.im) == 1);
    }
}