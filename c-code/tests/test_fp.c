#include <fp.h>
#include "test_declarations.h"
#include "test_utils.h"
#include <stdio.h>

static MunitResult sample(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {
    fp_t seed, rnd, aux, one;
    uint8_t value_equality, expected_value;
    uint32_t num_checks = 10000;
    RANDOM_FP_ELEMENT(seed);
    fp_set_to_zero(rnd);
    fp_set_to_zero(aux);
    fp_set_to_one(one);

    assert_memory_not_equal(FIELD_BYTES, seed, rnd);  // Random value different from zero

    fp_copy(aux, FIELD_CHARACTERISTIC);
    assert_memory_equal(FIELD_BYTES, aux, FIELD_CHARACTERISTIC);

    aux[FIELD_64BITS_WORDS - 1]++;
    value_equality = fp_is_smaller(aux, FIELD_CHARACTERISTIC);
    expected_value = 0x00;
    assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);

    aux[FIELD_64BITS_WORDS - 1] -= 2;
    value_equality = fp_is_smaller(aux, FIELD_CHARACTERISTIC);
    expected_value = 0x01;
    assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);

    for (uint32_t i = 0; i < num_checks; i++) {
        fp_sample(rnd);
        value_equality = fp_is_smaller(rnd, FIELD_CHARACTERISTIC);
        assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);
    }


    return MUNIT_OK;
}


static MunitResult is_zero(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp_t a;
    fp_set_to_zero(a);
    assert_true(fp_is_zero(a));
    RANDOM_FP_ELEMENT(a);
    assert_false(fp_is_zero(a));

    return MUNIT_OK;
}


static MunitResult add_and_sub(MUNIT_UNUSED const MunitParameter params[],
                                            MUNIT_UNUSED void *user_data_or_fixture) {
    fp_t a, b, c, d, e, f;

    RANDOM_FP_ELEMENT(a);
    fp_set_to_zero(b);
    fp_set_to_zero(c);
    fp_set_to_zero(d);

    assert_memory_not_equal(FIELD_64BITS_WORDS, a, d);  // Random value different from zero

    fp_add(c, a, b);
    assert_memory_equal(FIELD_64BITS_WORDS, a, c);      // a + 0 = a
    fp_set_to_zero(c);
    fp_add(c, b, a);
    assert_memory_equal(FIELD_64BITS_WORDS, a, c);      // 0 + a = a

    fp_neg(b, a);
    assert_memory_not_equal(FIELD_64BITS_WORDS, a, b);  // -a != a
    fp_add(c, a, b);
    assert_memory_equal(FIELD_64BITS_WORDS, c, d);      // a - a = 0
    fp_add(c, b, a);
    assert_memory_equal(FIELD_64BITS_WORDS, c, d);      // (-a) + a = 0

    fp_add(b, a, a);
    fp_half(c, b);
    assert_memory_equal(FIELD_64BITS_WORDS, c, a);      // (2a)/2 = a

    RANDOM_FP_ELEMENT(b);
    assert_memory_not_equal(FIELD_64BITS_WORDS, b, d);  // Random value different from zero

    fp_add(c, a, b);
    fp_sub(d, a, b);
    assert_memory_not_equal(FIELD_64BITS_WORDS, b, d);  // a + b != a - b

    fp_sub(e, c, b);
    assert_memory_equal(FIELD_64BITS_WORDS, e, a);      // (a + b) - b = a

    fp_add(f, d, b);
    assert_memory_equal(FIELD_64BITS_WORDS, f, a);      // (a - b) + b = a

    fp_add(e, c, d);
    fp_half(e, e);
    assert_memory_equal(FIELD_64BITS_WORDS, e, a);      // ([a + b] + [a - b])/2 = a

    fp_sub(f, c, d);
    fp_half(f, f);
    assert_memory_equal(FIELD_64BITS_WORDS, f, b);      // ([a + b] - [a - b])/2 = b

    fp_sub(c, b, a);
    fp_neg(c, c);
    assert_memory_equal(FIELD_64BITS_WORDS, c, d);      // (a - b) = -(b - a)

    return MUNIT_OK;
}


static MunitResult mul_and_sqr(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {
    fp_t a, b, c, d;

    fp_set_to_zero(a);
    fp_set_to_zero(b);
    fp_set_to_zero(c);
    fp_set_to_zero(d);

    a[0] = 1;
    fp_copy(b, a);

    fp_to_mont(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, b, MONTGOMERY_CONSTANT_ONE);    // 1 -> R

    fp_from_mont(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, b, a);                          // R -> 1

    RANDOM_FP_ELEMENT(a);
    fp_mul(b, a, MONTGOMERY_CONSTANT_ONE);
    assert_memory_equal(FIELD_64BITS_WORDS, b, a);                          // a * 1 = a

    fp_mul(b, MONTGOMERY_CONSTANT_ONE, a);
    assert_memory_equal(FIELD_64BITS_WORDS, b, a);                          // 1 * a = a

    fp_set_to_zero(c);
    fp_mul(b, a, c);
    assert_memory_equal(FIELD_64BITS_WORDS, b, c);                          // a * 0 = 0

    fp_mul(b, c, a);
    assert_memory_equal(FIELD_64BITS_WORDS, b, c);                          // 0 * a = 0

    c[0] = 2;
    fp_to_mont(c, c);
    fp_add(b, a, a);
    fp_mul(d, a, c);
    assert_memory_equal(FIELD_64BITS_WORDS, b, d);                          // 2 * a = a + a

    fp_set_to_zero(b);
    fp_set_to_zero(c);
    fp_sqr(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, b, c);                          // 0^2 = 0

    fp_set_to_one(b);
    fp_sqr(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, b, MONTGOMERY_CONSTANT_ONE);    // 1^2 = 1

    fp_sqr(b, a);
    fp_mul(c, a, a);
    assert_memory_equal(FIELD_64BITS_WORDS, b, c);                          // a * a = a^2

    return MUNIT_OK;
}


static MunitResult inv(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp_t a, b, c;

    RANDOM_FP_ELEMENT(a);
    fp_set_to_zero(b);
    fp_set_to_zero(c);

    assert_memory_not_equal(FIELD_64BITS_WORDS, a, c);  // Random value different from zero

    fp_set_to_zero(b);
    assert_memory_not_equal(FIELD_64BITS_WORDS, a, b);

    fp_inv(b, a);
    fp_mul(c, a, b);
    assert_memory_equal(FIELD_64BITS_WORDS, c, MONTGOMERY_CONSTANT_ONE);    // a * a⁻¹ = 1

    fp_inv(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, a, b);                          // (a⁻¹)⁻¹ = a

    fp_set_to_one(a);
    fp_inv(b, a);
    assert_memory_equal(FIELD_64BITS_WORDS, a, b);                          // (1⁻¹) = 1

    fp_mul(c, a, b);
    assert_memory_equal(FIELD_64BITS_WORDS, c, MONTGOMERY_CONSTANT_ONE);    // 1 * 1 = 1

    return MUNIT_OK;
}

static MunitResult square_root(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp_t a, b, c, d;

    RANDOM_FP_ELEMENT(a);
    fp_set_to_zero(b);
    fp_set_to_zero(c);
    fp_set_to_zero(d);

    assert_memory_not_equal(FIELD_64BITS_WORDS, a, c);  // Random value different from zero
    assert_memory_not_equal(FIELD_64BITS_WORDS, a, b);

    // b =  a^2
    fp_sqr(b, a);

    // c = sqrt(b)
    fp_sqrt(c, b);

    // d  = c^2
    fp_sqr(d, c);
    assert_memory_equal(FIELD_64BITS_WORDS, b, d);

    return MUNIT_OK;
}

static MunitResult cube_root(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp_t a, b, c, d;

    if(FIELD_BITS == 381 || FIELD_BITS == 575 || FIELD_BITS == 765){
        return MUNIT_SKIP;
    }

    RANDOM_FP_ELEMENT(a);
    fp_set_to_zero(b);
    fp_set_to_zero(c);
    fp_set_to_zero(d);

    assert_memory_not_equal(FIELD_64BITS_WORDS, a, c);  // Random value different from zero
    assert_memory_not_equal(FIELD_64BITS_WORDS, a, b);

    // b =  a^3
    fp_sqr(b, a);
    fp_mul(b, b, a);

    // c = sqrt(b)
    fp_curt(c, b);

    // d  = c^2
    fp_sqr(d, c);
    fp_mul(d, d, c);
    assert_memory_equal(FIELD_64BITS_WORDS, b, d);

    return MUNIT_OK;
}

/*
 * Register test cases
 */

MunitTest test_fp[] = {
        TEST_CASE(sample),
        TEST_CASE(is_zero),
        TEST_CASE(add_and_sub),
        TEST_CASE(mul_and_sqr),
        TEST_CASE(inv),
        TEST_CASE(square_root),
        TEST_CASE(cube_root),
        TEST_END
};

