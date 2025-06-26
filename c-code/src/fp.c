//
// Prime field arithmetic GF(p)
//

#include <string.h>
#include "fp.h"
#include "utilities.h"

void fp_neg(fp_t output, const fp_t input) {
    fp_t zero = {0};
    fp_sub(output, zero, input);
}

void fp_set_to_one(fp_t input_output) {
    fp_copy(input_output, MONTGOMERY_CONSTANT_ONE);
}


void fp_set_to_zero(fp_t input_output) {
    memset(input_output, 0, sizeof(fp_t));
}


void fp_copy(fp_t output, const fp_t input) {
    memcpy(output, input, sizeof(fp_t));
}

void fp_cset(fp_t output, const fp_t input, uint64_t input_mask) {
    // If mask = 0 then output is not modified, else if input = 0xFF...FF then output <- input
    uint64_t temp;
    unsigned int i;

    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        temp = input_mask & (output[i] ^ input[i]);
        output[i] = temp ^ output[i];
    }
}

void fp_cswap(fp_t input_a, fp_t input_b, uint64_t input) {
    // If input = 0 then a <- a and b <- b, else if input = 0xFF...FF then a <- b and b <- a
    uint64_t temp;
    unsigned int i;

    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        temp = input & (input_a[i] ^ input_b[i]);
        input_a[i] = temp ^ input_a[i];
        input_b[i] = temp ^ input_b[i];
    }
}

void fp_sample(fp_t output) {
    randombytes((uint8_t *) output, FIELD_BYTES);
    output[FIELD_64BITS_WORDS - 1] &= MASK_FIELD_ELEMENT;
    while (!multiprecision_is_smaller(output, FIELD_CHARACTERISTIC, FIELD_64BITS_WORDS)) {
        randombytes((uint8_t *) output, FIELD_BYTES);
        output[FIELD_64BITS_WORDS - 1] &= MASK_FIELD_ELEMENT;
    }
}


// We should use Bernstein & Yang algorithm
void fp_inv(fp_t output, const fp_t input) {

    fp_t temp, out;
    fp_set_to_one(out);
    fp_copy(temp, input);

    int i, j, k;
    uint64_t flag;

    for (i = 0, k = 0; i < FIELD_64BITS_WORDS && k < FIELD_BITS; i++, k++) {
        flag = 1;
        for (j = 0; j < 64; j++, k++) {
            if ((flag & FIELD_INVERSION_EXPONENT[i]) != 0)
                fp_mul(out, temp, out);
            fp_sqr(temp, temp);
            flag <<= 1;
        }
    }
    fp_copy(output, out);
}


void fp_half(fp_t output, const fp_t input) {
    uint8_t carry = 0;
    int i;
    uint64_t mask;

    mask = 0 - (uint64_t) (input[0] & 1);
    // If a is odd compute a + p
    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        addition_with_carry_u64(output[i], carry, carry, input[i], FIELD_CHARACTERISTIC[i] & mask);
    }

    multiprecision_shift_to_right(output, FIELD_64BITS_WORDS);
}


void fp_to_mont(fp_t output, const fp_t input) {
    fp_mul(output, input, (uint64_t *) &MONTGOMERY_CONSTANT_R_SQUARED);
}


void fp_from_mont(fp_t output, const fp_t input) {
    fp_t one = {0};
    one[0] = 1;
    fp_mul(output, input, one);
}


// constant-time check of a == 0
int64_t fp_is_zero(const fp_t input) {
    int i;
    uint64_t output = 0;
    for (i = FIELD_64BITS_WORDS - 1; i >= 0; i--)
        output |= input[i];
 
    return ~(((int64_t)output >> 63) | (-(int64_t)output >> 63));
}


uint8_t fp_is_equal(const fp_t input_a, const fp_t input_b) {
    int i;
    uint8_t r = 0;
    uint64_t t;

    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        t = 0;
        uint8_t *ta = (uint8_t *) &input_a[i];
        uint8_t *tb = (uint8_t *) &input_b[i];
        t = (ta[0] ^ tb[0]) | (ta[1] ^ tb[1]) | (ta[2] ^ tb[2]) | (ta[3] ^ tb[3]) |
            (ta[4] ^ tb[4]) | (ta[5] ^ tb[5]) | (ta[6] ^ tb[6]) | (ta[7] ^ tb[7]);

        t = (-t);
        t = t >> 63;
        r |= t;
    }

    return (uint8_t) (1 - r);
}


uint8_t fp_is_smaller(const fp_t input1, const fp_t input2) {
    return multiprecision_is_smaller(input1, input2, FIELD_64BITS_WORDS);
}


uint8_t fp_is_square(const fp_t input) {
    fp_t temp, input_1;
    uint64_t flag;
    int i, j;
    // input_1 <- input ^ ([p - 1] / 2)
    fp_set_to_one(input_1);
    fp_copy(temp, input);
    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        flag = 1;
        for (j = 0; j < 64; j++) {
            if ((flag & SQUARE_ROOT_EXPONENT_12[i]) != 0)   
                fp_mul(input_1, temp, input_1);
            fp_sqr(temp, temp);
            flag <<= 1;
        }
    }

    return fp_is_equal(input_1, MONTGOMERY_CONSTANT_ONE);
}

void fp_sqrt(fp_t output, const fp_t input) {
    fp_t temp, input_1;
    uint64_t flag;
    int i, j;
    // input_1 <- input ^ ([p - 3] / 4)
    fp_set_to_one(input_1);
    fp_copy(temp, input);
    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        flag = 1;
        for (j = 0; j < 64; j++) {
            if ((flag & SQUARE_ROOT_EXPONENT_34[i]) != 0)
                fp_mul(input_1, temp, input_1);
            fp_sqr(temp, temp);
            flag <<= 1;
        }
    }
    fp_mul(output, input_1, input);
}

#ifdef SSEC_CUBE_ROOT_OVER_FP
// Cube root when p = 2 mod 3
void fp_curt(fp_t output, const fp_t input) {
    fp_t temp, input_1;
    uint64_t flag;
    int i, j;
    // input_1 <- input ^ -([p - 2] / 3)
    fp_set_to_one(input_1);
    fp_copy(temp, input);
    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        flag = 1;
        for (j = 0; j < 64; j++) {
            if ((flag & CUBE_ROOT_EXPONENT_213[i]) != 0)
                fp_mul(input_1, temp, input_1);
            fp_sqr(temp, temp);
            flag <<= 1;
        }
    }
    fp_copy(output, input_1);
}
#else
// Cube root not implemented! (Not required)
void fp_curt(fp_t output, const fp_t input) {
    fp_copy(output, input);
}
#endif
