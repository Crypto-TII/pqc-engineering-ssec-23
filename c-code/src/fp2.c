//
// Quadratic field arithmetic assuming p = 3 mod 4: GF(p²)
//

#include <string.h>
#include "fp2.h"
#include "utilities.h"


void fp2_add(fp2_t *output, fp2_t input_a, fp2_t input_b) {
    fp_add(output->re, input_a.re, input_b.re);
    fp_add(output->im, input_a.im, input_b.im);
}


void fp2_sub(fp2_t *output, fp2_t input_a, fp2_t input_b) {
    fp_sub(output->re, input_a.re, input_b.re);
    fp_sub(output->im, input_a.im, input_b.im);
}

void fp2_mul(fp2_t *output, fp2_t input_a, fp2_t input_b) {
    fp_t z0, z1, z2, z3, tmp;
    fp_add(z0, input_a.re, input_a.im);
    fp_add(z1, input_b.re, input_b.im);
    fp_mul(tmp, z0, z1);
    fp_mul(z2, input_a.re, input_b.re);
    fp_mul(z3, input_a.im, input_b.im);
    fp_sub(output->re, z2, z3);
    fp_sub(output->im, tmp, z2);
    fp_sub(output->im, output->im, z3);
}

void fp2_sqr(fp2_t *output, fp2_t input) {
    fp_t z0, z1, z2;
    fp_add(z0, input.re, input.re);
    fp_add(z1, input.re, input.im);
    fp_sub(z2, input.re, input.im);
    fp_mul(output->re, z1, z2);
    fp_mul(output->im, z0, input.im);
}


void fp2_neg(fp2_t *output, fp2_t input) {
    fp_neg(output->re, input.re);
    fp_neg(output->im, input.im);
}


void fp2_set_to_one(fp2_t *output) {
    fp_set_to_one(output->re);
    fp_set_to_zero(output->im);
}

void fp2_set_to_zero(fp2_t *output) {
    fp_set_to_zero(output->re);
    fp_set_to_zero(output->im);
}

void fp2_copy(fp2_t *output, fp2_t input) {
    fp_copy(output->re, input.re);
    fp_copy(output->im, input.im);
}


void fp2_cswap(fp2_t *input_a, fp2_t *input_b, uint64_t input) {
    // If input = 0 then a <- a and b <- b, else if input = 0xFF...FF then a <- b and b <- a
    uint64_t temp;
    unsigned int i;

    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        temp = input & (input_a->re[i] ^ input_b->re[i]);
        input_a->re[i] = temp ^ input_a->re[i];
        input_b->re[i] = temp ^ input_b->re[i];
        temp = input & (input_a->im[i] ^ input_b->im[i]);
        input_a->im[i] = temp ^ input_a->im[i];
        input_b->im[i] = temp ^ input_b->im[i];
    }
}


void fp2_cset(fp2_t *output, fp2_t input, uint64_t input_mask) {
    // If mask = 0 then output is not modified, else if input = 0xFF...FF then output <- input
    uint64_t temp;
    unsigned int i;

    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        temp = input_mask & (output->re[i] ^ input.re[i]);
        output->re[i] = temp ^ output->re[i];
        temp = input_mask & (output->im[i] ^ input.im[i]);
        output->im[i] = temp ^ output->im[i];
    }
}


void fp2_inv(fp2_t *output, fp2_t input) {
    fp_t N0, N1, S1, S2, zero = {0};
    fp_sqr(N0, input.re);
    fp_sqr(N1, input.im);
    fp_add(S1, N0, N1);
    fp_inv(S1, S1);
    fp_sub(S2, zero, input.im);
    fp_mul(output->re, S1, input.re);
    fp_mul(output->im, S1, S2);
}


void fp2_half(fp2_t *output, fp2_t input) {
    fp_half(output->re, input.re);
    fp_half(output->im, input.im);
}


void fp2_to_mont(fp2_t *output, fp2_t input) {
    fp_to_mont(output->re, input.re);
    fp_to_mont(output->im, input.im);
}


void fp2_from_mont(fp2_t *output, fp2_t input) {
    fp_from_mont(output->re, input.re);
    fp_from_mont(output->im, input.im);
}


void fp2_conj(fp2_t *output, fp2_t input) {
    fp_copy(output->re, input.re);
    fp_neg(output->im, input.im);
}


void fp2_to_bytes(uint8_t *output, fp2_t input) {
    fp2_t t;

    fp2_from_mont(&t, input);
    memcpy(output, t.re, FIELD_BYTES);
    memcpy(&output[FIELD_BYTES], t.im, FIELD_BYTES);
}


void fp2_element_from_bytes(fp2_t *output, const uint8_t *input) {
    output->re[FIELD_64BITS_WORDS - 1] = 0;
    memcpy(output->re, input, FIELD_BYTES);
    output->im[FIELD_64BITS_WORDS - 1] = 0;
    memcpy(output->im, &input[FIELD_BYTES], FIELD_BYTES);
    fp2_to_mont(output, *output);
}


int64_t fp2_is_zero(fp2_t input) {
    return fp_is_zero(input.re) & fp_is_zero(input.im);
}


uint8_t fp2_is_equal(fp2_t input_a, fp2_t input_b) {
    return fp_is_equal(input_a.re, input_b.re) & fp_is_equal(input_a.im, input_b.im);
}


void fp2_linear_pass_in(fp2_t *output, const fp2_t *input, uint8_t input_length, uint8_t input_index) {
    // This function copies the ith entry of the input into the output 
    // At most 255-length lists
    uint8_t i;
    for(i = 0; i < input_length; i++) {
        int64_t mask = (int64_t)input_index - (int64_t)i;
        fp2_cset(output, input[i], ~((mask >> 63) | (-mask >> 63)));
    }
}


void fp2_linear_pass_out(fp2_t *output, fp2_t input, uint8_t input_length, uint8_t input_index) {
    // This function copies the input into the ith entry of the output
    // At most 255-length lists
    uint8_t i;
    for(i = 0; i < input_length; i++) {
        int64_t mask = (int64_t)input_index - (int64_t)i;
        fp2_cset(&output[i], input, ~((mask >> 63) | (-mask >> 63)));
    }
}


uint8_t fp2_locate_zero(const fp2_t *input, uint8_t input_length) {
    // At most 255-length lists
    uint8_t i;
    int64_t index = 0;
    for(i = 0; i < input_length; i++) {
        index |= i & fp2_is_zero(input[i]);
    }
    return (uint8_t)index;
}


uint8_t fp2_is_square(fp2_t input) {
    fp_t t0, t1;

    fp_sqr(t0, input.re);
    fp_sqr(t1, input.im);
    fp_add(t0, t0, t1);
    return fp_is_square(t0);
}


void fp2_batchinv(fp2_t *output_list, const fp2_t *input_list, uint8_t input_length) {
    int i;
    fp2_t tmp_list[input_length], inv, tmp;

    fp2_copy(&tmp_list[0], input_list[0]);
    for(i = 1; i < input_length; i++) { fp2_mul(&tmp_list[i], tmp_list[i - 1], input_list[i]); }
    fp2_inv(&inv, tmp_list[input_length - 1]);
    for(i = (input_length - 1); i > 0; i--) {
        fp2_mul(&tmp, inv, tmp_list[i - 1]);
        fp2_mul(&inv, inv, input_list[i]);
        fp2_copy(&output_list[i], tmp);
    }
    fp2_copy(&output_list[0], inv);
}

#ifndef SSEC_KONG_ET_AL_S_ALGORITHM
void fp2_sqrt_slow(fp2_t *output, fp2_t input) {
    // Square root over Fq with q = p²: Adj and Rodríguez-Henríquez's algorithm (constant-time implementation). We assume p = 3 mod 4
    // if input does NOT have square-root, then it returns zero

    fp2_t input_1, alpha, ALPHA, alpha_conjugated, input_0, minus_one, x0, temp, choose_from[3];

    fp2_set_to_zero(&alpha_conjugated);
    fp2_set_to_one(&minus_one);
    fp2_neg(&minus_one, minus_one);

    int i, j, k;
    uint64_t flag;
    
    // input_1 <- input ^ ([p - 3] / 4)
    fp2_set_to_one(&input_1);
    fp2_copy(&temp, input);
    for (i = 0, k = 0; i < FIELD_64BITS_WORDS && k < FIELD_BITS; i++, k++) {
        flag = 1;
        for (j = 0; j < 64; j++, k++) {
            if ((flag & SQUARE_ROOT_EXPONENT_34[i]) != 0) {fp2_mul(&input_1, temp, input_1);}
            fp2_sqr(&temp, temp);
            flag <<= 1;
        }
    }

    fp2_sqr(&alpha, input_1);
    fp2_mul(&alpha, alpha, input);

    fp2_conj(&alpha_conjugated, alpha);
    fp2_mul(&input_0, alpha_conjugated, alpha);

    // choose_from[2] stores zero when input does NOT have square-root
    fp2_set_to_zero(&choose_from[2]);

    // choose_from[1] stores the square root concerning alpha == -1
    fp2_mul(&x0, input_1, input);
    fp_neg(choose_from[1].re, x0.im);
    fp_copy(choose_from[1].im, x0.re);

    // choose_from[0] stores the square root concerning alpha != -1
    fp2_sub(&ALPHA, alpha, minus_one);

    // output <- ALPHA ^ ([p - 1] / 2)
    fp2_set_to_one(&choose_from[0]);
    fp2_copy(&temp, ALPHA);
    for (i = 0, k = 0; i < FIELD_64BITS_WORDS && k < FIELD_BITS; i++, k++) {
        flag = 1;
        for (j = 0; j < 64; j++, k++) {
            if ((flag & SQUARE_ROOT_EXPONENT_12[i]) != 0) {fp2_mul(&choose_from[0], temp, choose_from[0]);}
            fp2_sqr(&temp, temp);
            flag <<= 1;
        }
    }
    fp2_mul(&choose_from[0], choose_from[0], x0);

    // select the correct square-root output by linear pass (constant-time)
    fp2_linear_pass_in(output, choose_from, 3, (fp2_is_equal(minus_one, input_0) << 1) ^ fp2_is_equal(minus_one, alpha));
    
    // clear data
    fp2_set_to_zero(&choose_from[0]);
    fp2_set_to_zero(&choose_from[1]);
    fp2_set_to_zero(&choose_from[2]);
    fp2_set_to_zero(&temp);
}
#else
void fp2_sqrt_slow(fp2_t *output, fp2_t input) {
    // Square root over Fq with q = p²: Kong et al.'s algorithm. We assume p² = 9 mod 16 and p = 3 mod 4
    // This function assumes the input has square-root

    int i, j, k;
    uint64_t flag;
    fp_t one;
    fp2_t a_double, b, b_square, c, r, temp, s[2], u;

    fp_set_to_one(one);

    fp2_set_to_one(&b);
    fp2_add(&a_double, input, input);
    fp2_copy(&temp, a_double);

    for (i = 0, k = 0; i < 2 * FIELD_64BITS_WORDS && k < SQUARE_ROOT_EXPONENT_BITS; i++, k++) {
        flag = 1;
        for (j = 0; j < 64; j++, k++) {
            if ((flag & SQUARE_ROOT_EXPONENT_916[i]) != 0) {fp2_mul(&b, temp, b);}
            fp2_sqr(&temp, temp);
            flag <<= 1;
        }
    }
    fp2_sqr(&b_square, b);
    fp2_mul(&c, a_double, b_square);
    fp2_sqr(&r, c);
    fp_sub(c.re, c.re, one); // c - 1

    fp2_mul(&s[0], input, b);
    fp2_mul(&s[0], s[0], c);
    fp2_mul(&u, b, *((fp2_t *)&SQUARE_ROOT_CONSTANT_T));
    fp2_mul(&s[1], u, *((fp2_t *)&SQUARE_ROOT_CONSTANT_D));

    fp2_sqr(&c, s[1]);
    fp2_mul(&c, c, input);
    fp2_add(&c, c, c);
    fp_sub(c.re, c.re, one); // c - 1
    fp2_mul(&s[1], s[1], input);
    fp2_mul(&s[1], s[1], c);

    // select the correct square-root output by linear pass (constant-time)
    fp2_linear_pass_in(output, s, 2, fp_is_equal(one, r.re) & fp_is_zero(r.im));

    // clear data
    fp2_set_to_zero(&s[0]);
    fp2_set_to_zero(&s[1]);
    fp2_set_to_zero(&temp);
}
#endif

void fp2_sqrt_fast(fp2_t *output, fp2_t input) {
    fp_t delta, x0, t0, x1, t1, temp;
    uint64_t flag;

    // x1 <- (a0^2 + a1^2)
    fp_sqr(x0, input.re);
    fp_sqr(x1, input.im);
    fp_add(x1, x0, x1);
    
    // alpha <- x1 ^ ([p + 1] / 4)
    fp_set_to_one(delta);
    fp_copy(temp, x1);
    for (int i = 0; i < FIELD_64BITS_WORDS; i++) {
        flag = 1;
        for (int j = 0; j < 64; j++) {
            if ((flag & SQUARE_ROOT_EXPONENT_14[i]) != 0)
                fp_mul(delta, temp, delta);
            fp_sqr(temp, temp);
            flag <<= 1;
        }
    }

    // x0 <- a0 + delta
    fp_add(x0, input.re, delta);
    // t0 <- 2 * x0
    fp_add(t0, x0, x0);

    // x1 <- t0 ^ ([p - 3] / 4)
    fp_set_to_one(x1);
    fp_copy(temp, t0);
    for (int i = 0; i < FIELD_64BITS_WORDS; i++) {
        flag = 1;
        for (int j = 0; j < 64; j++) {
            if ((flag & SQUARE_ROOT_EXPONENT_34[i]) != 0)
                fp_mul(x1, temp, x1);
            fp_sqr(temp, temp);
            flag <<= 1;
        }
    }
    
    // x0 <- x0 * x1
    fp_mul(x0, x0, x1);
    // x1 <- a1 * x1
    fp_mul(x1, input.im, x1);

    // t1 <- (2 * x0)^2
    fp_add(t1, x0, x0);
    fp_sqr(t1, t1);

    fp_t uno, zero;
    fp_set_to_one(uno);
    fp_set_to_zero(zero);

    uint8_t selector = -fp_is_equal(t1, t0);
    
    fp_neg(temp, x0);
    fp_copy(t0, x1);
    fp_copy(t1, temp);

    constant_time_conditional_mov((uint8_t *)&t0, (uint8_t *)&x0, FIELD_64BITS_WORDS * 8, selector);
    constant_time_conditional_mov((uint8_t *)&t1, (uint8_t *)&x1, FIELD_64BITS_WORDS * 8, selector);

    fp_copy(output->re, t0);
    fp_copy(output->im, t1);
}

void fp2_curt(fp2_t *output, fp2_t input) {
    // This function assumes the input has cube-root
    int i, j, k;
    uint64_t flag;
    fp2_t temp, aux;
    fp2_set_to_one(&aux);
    fp2_copy(&temp, input);

    for (i = 0, k = 0; i < 2 * FIELD_64BITS_WORDS && k < CUBE_ROOT_EXPONENT_BITS; i++, k++) {
        flag = 1;
        for (j = 0; j < 64; j++, k++) {
            if ((flag & CUBE_ROOT_EXPONENT[i]) != 0) {fp2_mul(&aux, temp, aux);}
            fp2_sqr(&temp, temp);
            flag <<= 1;
        }
    }

    fp2_copy(output, aux);
}
