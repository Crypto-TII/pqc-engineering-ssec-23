#include "test_declarations.h"
#include "test_utils.h"
#include <fp2.h>

/*
 * Test cases
 */

static MunitResult is_zero(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp2_t a;
    fp2_set_to_zero(&a);
    assert_true(fp2_is_zero(a));
    RANDOM_FP2_ELEMENT(&a);
    assert_false(fp2_is_zero(a));

    return MUNIT_OK;
}


static MunitResult locate_zero(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    uint8_t length = 255;
    fp2_t a[length], b;
    uint8_t i;

    for(i = 0; i < length; i++) {
        RANDOM_FP2_ELEMENT(&a[i]);
    }

    for(i = 0; i < length; i++) {
        fp2_copy(&b, a[i]);
        fp2_set_to_zero(&a[i]);
        assert_true(fp2_locate_zero(a, length) == i);
        fp2_copy(&a[i], b);
    }

    return MUNIT_OK;
}

static MunitResult linear_pass(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    uint8_t length = 255;
    fp2_t a[length], b, c[length];
    uint8_t i, j;

    for(i = 0; i < length; i++) {
        RANDOM_FP2_ELEMENT(&a[i]);
        fp2_set_to_zero(&c[i]);
    }

    // Linear pass to store at the ith position
    for(i = 0; i < length; i++) {
        fp2_linear_pass_in(&b, a, length, i);
        assert_memory_equal(sizeof(fp2_t), &b, &a[i]);
    }

    // Linear pass to store at the (i ^ index)-th position 
    for(i = 0; i < length; i++) {
        RANDOM_FP2_ELEMENT(&b);
        fp2_linear_pass_out(c, b, length, i);
        assert_memory_equal(sizeof(fp2_t), &c[i], &b);
        for(j = 0; j < length; j++) {
            if(j != i) { assert_true(fp2_is_zero(c[j])); }
        }
        fp2_set_to_zero(&c[i]);
    }

    return MUNIT_OK;
}


static MunitResult add_and_sub(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp2_t a, b, c, d, e, f;

    RANDOM_FP2_ELEMENT(&a);
    fp2_set_to_zero(&b);
    fp2_set_to_zero(&c);
    fp2_set_to_zero(&d);

    assert_memory_not_equal(sizeof(fp2_t), &a, &d);  // Random value different from zero

    fp2_add(&c, a, b);
    assert_memory_equal(sizeof(fp2_t), &a, &c);      // a + 0 = a
    fp2_set_to_zero(&c);
    fp2_add(&c, b, a);
    assert_memory_equal(sizeof(fp2_t), &a, &c);      // 0 + a = a

    fp2_neg(&b, a);
    assert_memory_not_equal(sizeof(fp2_t), &a, &b);  // -a != a
    fp2_add(&c, a, b);
    assert_memory_equal(sizeof(fp2_t), &c, &d);      // a - a = 0
    fp2_add(&c, b, a);
    assert_memory_equal(sizeof(fp2_t), &c, &d);      // (-a) + a = 0

    fp2_add(&b, a, a);
    fp2_half(&c, b);
    assert_memory_equal(sizeof(fp2_t), &c, &a);      // (2a)/2 = a

    RANDOM_FP2_ELEMENT(&b);
    assert_memory_not_equal(sizeof(fp2_t), &b, &d);  // Random value different from zero

    fp2_add(&c, a, b);
    fp2_sub(&d, a, b);
    assert_memory_not_equal(sizeof(fp2_t), &b, &d);  // a + b != a - b

    fp2_sub(&e, c, b);
    assert_memory_equal(sizeof(fp2_t), &e, &a);      // (a + b) - b = a

    fp2_add(&f, d, b);
    assert_memory_equal(sizeof(fp2_t), &f, &a);      // (a - b) + b = a

    fp2_add(&e, c, d);
    fp2_half(&e, e);
    assert_memory_equal(sizeof(fp2_t), &e, &a);      // ([a + b] + [a - b])/2 = a

    fp2_sub(&f, c, d);
    fp2_half(&f, f);
    assert_memory_equal(sizeof(fp2_t), &f, &b);      // ([a + b] - [a - b])/2 = b

    fp2_sub(&c, b, a);
    fp2_neg(&c, c);
    assert_memory_equal(sizeof(fp2_t), &c, &d);      // (a - b) = -(b - a)

    return MUNIT_OK;
}


static MunitResult mul_and_sqr(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp2_t a, b, c, d, e;

    fp2_set_to_zero(&a);
    fp2_set_to_zero(&b);
    fp2_set_to_zero(&c);
    fp2_set_to_zero(&d);
    fp2_set_to_one(&e);

    a.re[0] = 1;
    fp2_copy(&b, a);

    fp2_to_mont(&b, b);
    assert_memory_equal(sizeof(fp2_t), &b, &e);    // 1 -> R

    fp2_from_mont(&b, b);
    assert_memory_equal(sizeof(fp2_t), &b, &a);       // R -> 1

    RANDOM_FP2_ELEMENT(&a);
    fp2_mul(&b, a, e);
    assert_memory_equal(sizeof(fp2_t), &b, &a);       // a * 1 = a

    fp2_mul(&b, e, a);
    assert_memory_equal(sizeof(fp2_t), &b, &a);       // 1 * a = a

    fp2_set_to_zero(&c);
    fp2_mul(&b, a, c);
    assert_memory_equal(sizeof(fp2_t), &b, &c);       // a * 0 = 0

    fp2_mul(&b, c, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c);       // 0 * a = 0

    c.re[0] = 2;
    fp2_to_mont(&c, c);
    fp2_add(&b, a, a);
    fp2_mul(&d, a, c);
    assert_memory_equal(sizeof(fp2_t), &b, &d);       // 2 * a = a + a

    fp2_set_to_zero(&b);
    fp2_set_to_zero(&c);
    fp2_sqr(&b, b);
    assert_memory_equal(sizeof(fp2_t), &b, &c);       // 0² = 0

    fp2_set_to_one(&b);
    fp2_sqr(&b, b);
    assert_memory_equal(sizeof(fp2_t), &b, &e);       // 1² = 1

    fp2_sqr(&b, a);
    fp2_mul(&c, a, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c);       // a * a = a²

    return MUNIT_OK;
}


static MunitResult inv(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp2_t a, b, c, one;

    RANDOM_FP2_ELEMENT(&a);
    fp2_set_to_zero(&b);
    fp2_set_to_zero(&c);
    fp2_set_to_one(&one);

    assert_memory_not_equal(sizeof(fp2_t), &a, &c);  // Random value different from zero

    fp2_inv(&b, a);
    fp2_mul(&c, a, b);
    assert_memory_equal(sizeof(fp2_t), &c, &one);     // a * a⁻¹ = 1

    fp2_inv(&b, b);
    assert_memory_equal(sizeof(fp2_t), &a, &b);       // (a⁻¹)⁻¹ = a

    fp2_set_to_one(&a);
    fp2_inv(&b, a);
    assert_memory_equal(sizeof(fp2_t), &a, &b);       // (1⁻¹) = 1

    fp2_mul(&c, a, b);
    assert_memory_equal(sizeof(fp2_t), &c, &a);       // 1 * 1 = 1

    return MUNIT_OK;
}

static MunitResult batchinv(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    int length;

    for(length = 1; length < 16; length++) {
        fp2_t a[length], b[length], c, one;

        for(int i = 0; i < length; i++) {
            RANDOM_FP2_ELEMENT(&a[i]);
            fp2_set_to_zero(&b[i]);
            assert_memory_not_equal(sizeof(fp2_t), &a[i], &b[i]);  // Random value different from zero
        }
        fp2_set_to_one(&one);

        fp2_batchinv(b, a, length);

        for(int i = 0; i < length; i++) {
            fp2_mul(&c, a[i], b[i]);
            assert_memory_equal(sizeof(fp2_t), &c, &one);     // a * a⁻¹ = 1
        }
    }

    return MUNIT_OK;
}

static MunitResult square_root_slow(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp2_t a, b, c;

    RANDOM_FP2_ELEMENT(&a);
    fp2_sqr(&a, a);
    fp2_set_to_zero(&b);
    fp2_set_to_zero(&c);

    assert_memory_not_equal(sizeof(fp2_t), &a, &c);  // Random value different from zero

    // GF(p)-elements always have sqrt
    fp2_set_to_zero(&b);
    fp_copy(b.re, a.re);
    fp2_sqrt_slow(&c, b);
    fp2_sqr(&c, c);
    assert_memory_equal(sizeof(fp2_t), &c, &b);     // c ^ 2 = b

    fp2_sqrt_slow(&b, a);
    fp2_sqr(&c, b);
    assert_memory_equal(sizeof(fp2_t), &c, &a);     // c ^ 2 = a

    fp2_set_to_one(&a);
    fp2_sqrt_slow(&b, a);
    fp2_sqr(&c, b);
    assert_memory_equal(sizeof(fp2_t), &c, &a);     // c ^ 2 = 1

    fp2_set_to_zero(&a);
    fp2_sqrt_slow(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &a);     // b = 0

    return MUNIT_OK;
}

static MunitResult square_root_fast(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp2_t a, b, c;

    RANDOM_FP2_ELEMENT(&a);
    fp2_sqr(&a, a);
    fp2_set_to_zero(&b);
    fp2_set_to_zero(&c);

    assert_memory_not_equal(sizeof(fp2_t), &a, &c);  // Random value different from zero

    // GF(p)-elements always have sqrt but sqrt_fast does not work always (1/2 of probability of returning zero)
    // So we skip tests over GF(p)

    fp2_sqrt_fast(&b, a);
    fp2_sqr(&c, b);
    assert_memory_equal(sizeof(fp2_t), &c, &a);     // c ^ 2 = a

    fp2_set_to_one(&a);
    fp2_sqrt_fast(&b, a);
    fp2_sqr(&c, b);
    assert_memory_equal(sizeof(fp2_t), &c, &a);     // c ^ 2 = 1

    fp2_set_to_zero(&a);
    fp2_sqrt_fast(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &a);     // b = 0

    return MUNIT_OK;
}

static MunitResult cube_root(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp2_t a, b, c, t;

    RANDOM_FP2_ELEMENT(&a);
    fp2_sqr(&t, a);
    fp2_mul(&a, t, a);
    fp2_set_to_zero(&b);
    fp2_set_to_zero(&c);

    assert_memory_not_equal(sizeof(fp2_t), &a, &c);  // Random value different from zero

    fp2_curt(&b, a);
    fp2_sqr(&t, b);
    fp2_mul(&c, t, b);
    assert_memory_equal(sizeof(fp2_t), &c, &a);     // c ^ 3 = a

    fp2_set_to_one(&a);
    fp2_copy(&b, *((fp2_t *)&CUBE_ROOT_OF_UNITY));
    assert_memory_not_equal(sizeof(fp2_t), &a, &b);  // Cube root of unity different from 1
    fp2_curt(&c, b);
    fp2_sqr(&t, c);
    fp2_mul(&t, t, c);
    assert_memory_equal(sizeof(fp2_t), &t, &a);     // c ^ 3 = 1

    fp2_set_to_zero(&a);
    fp2_curt(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &a);     // b = 0

    return MUNIT_OK;
}

/*
 * Register test cases
 */

MunitTest test_fq[] = {
        TEST_CASE(is_zero),
        TEST_CASE(locate_zero),
        TEST_CASE(linear_pass),
        TEST_CASE(add_and_sub),
        TEST_CASE(mul_and_sqr),
        TEST_CASE(inv),
        TEST_CASE(batchinv),
        TEST_CASE(square_root_slow),
        TEST_CASE(square_root_fast),
        TEST_CASE(cube_root),
        TEST_END
};
