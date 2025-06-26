//
// Utility functions: 64-bits built-in functions
//

#include "utilities.h"

void multiplication_u64(uint64_t *output_low, uint64_t *output_high, uint64_t input_a, uint64_t input_b) {
    register uint64_t al, ah, bl, bh, temp;
    uint64_t albl, albh, ahbl, ahbh, res1, res2, res3, carry;
    uint64_t mask_low = (uint64_t) (-1) >> (sizeof(uint64_t) * 4), mask_high =
            (uint64_t) (-1) << (sizeof(uint64_t) * 4);

    al = input_a & mask_low;                         // Low part
    ah = input_a >> (sizeof(uint64_t) * 4);          // High part
    bl = input_b & mask_low;
    bh = input_b >> (sizeof(uint64_t) * 4);

    albl = al * bl;
    albh = al * bh;
    ahbl = ah * bl;
    ahbh = ah * bh;
    *output_low = albl & mask_low;                   // C00

    res1 = albl >> (sizeof(uint64_t) * 4);
    res2 = ahbl & mask_low;
    res3 = albh & mask_low;
    temp = res1 + res2 + res3;
    carry = temp >> (sizeof(uint64_t) * 4);
    *output_low ^= temp << (sizeof(uint64_t) * 4);    // C01

    res1 = ahbl >> (sizeof(uint64_t) * 4);
    res2 = albh >> (sizeof(uint64_t) * 4);
    res3 = ahbh & mask_low;
    temp = res1 + res2 + res3 + carry;
    *output_high = temp & mask_low;                   // C10
    carry = temp & mask_high;
    *output_high ^= (ahbh & mask_high) + carry;       // C11
}

void conditional_move_u64(uint64_t *output, uint8_t input_decision, uint64_t input_a, uint64_t input_b) {
    uint8_t x1;
    uint64_t x2;
    uint64_t x3;
    x1 = (!(!input_decision));
    x2 = ((int8_t) (0x0 - x1) & UINT64_C(0xffffffffffffffff));
    x3 = ((x2 & input_b) | ((~x2) & input_a));
    *output = x3;
}

uint64_t constant_time_is_nonzero_u64(uint64_t x) {
    return (uint64_t) ((x | (0 - x)) >> 63);
}

uint64_t constant_time_is_zero_u64(uint64_t x) {
    return (uint64_t) (1 ^ constant_time_is_nonzero_u64(x));
}

uint64_t constant_time_is_lessthan_u64(uint64_t x, uint64_t y) {
    return (uint64_t) ((x ^ ((x ^ y) | ((x - y) ^ y))) >> 63);
}

uint8_t constant_time_compare(const uint8_t *input_a, const uint8_t *input_b, uint64_t input_length) {
    uint64_t r = 0;
    uint64_t i;

    for (i = 0; i < input_length; i++)
        r |= input_a[i] ^ input_b[i];

    r = (-r);
    r = r >> 63;
    return (uint8_t)(1 - r);
}

void constant_time_conditional_mov(uint8_t *output, const uint8_t *input, uint64_t input_length, uint8_t input_selector) {
    uint64_t i;
    for (i = 0; i < input_length; i++)
        output[i] ^= input_selector & (input[i] ^ output[i]);
}

void multiprecision_shift_to_right(uint64_t *input_a_output_shifted_a, uint64_t input_words_length) {
    uint64_t i;
    for (i = 0; i < input_words_length - 1; i++) {
        constant_time_shift_to_right_u64(input_a_output_shifted_a[i], input_a_output_shifted_a[i + 1],
                                         input_a_output_shifted_a[i], 1, 64);
    }
    input_a_output_shifted_a[input_words_length - 1] >>= 1;
}

void multiprecision_shift_to_left(uint64_t *input_a_output_shifted_a, uint64_t input_words_length) {
    uint64_t i;
    for (i = input_words_length - 1; i > 0; i--) {
        constant_time_shift_to_left_u64(input_a_output_shifted_a[i], input_a_output_shifted_a[i],
                                        input_a_output_shifted_a[i - 1], 1, 64);
    }
    input_a_output_shifted_a[0] <<= 1;
}

void multiprecision_addition(uint64_t *output, const uint64_t *input_a, const uint64_t *input_b, uint64_t input_length) {
    uint64_t i, carry = 0;
    for (i = 0; i < input_length; i++) {addition_with_carry_u64(output[i], carry, carry, input_a[i], input_b[i]); }
}

void multiprecision_subtraction(uint64_t *output, const uint64_t *input_a, const uint64_t *input_b, uint64_t input_length) {
    uint64_t i;
    uint8_t borrow = 0;
    for (i = 0; i < input_length; i++) {
        subtraction_with_borrow_u64(output[i], borrow, borrow, input_a[i], input_b[i]);
    }
}

uint8_t multiprecision_is_smaller(const uint64_t *input_a, const uint64_t *input_b, uint64_t input_length) {
    uint8_t ret = 0;
    uint8_t is_equal[input_length], is_smaller[input_length];
    uint64_t i, j;
    for(i = 0; i < input_length; i++) {
        uint8_t *ta = (uint8_t *) &input_a[i];
        uint8_t *tb = (uint8_t *) &input_b[i];
        is_equal[i] = constant_time_compare(ta, tb, 8);
        is_smaller[i] = constant_time_is_lessthan_u64(input_a[i], input_b[i]);
    }

    for(i = input_length - 1; (int)i >= 0; i--) {
        uint8_t t = 1;
        for(j = input_length - 1; j > i; j--) {
            t &= is_equal[j];
        }
        ret |= (is_smaller[i] & t);
    }

    return ret;
}
