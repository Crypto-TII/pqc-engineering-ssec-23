//
// Quadratic field arithmetic assuming p = 3 mod 4: GF(p²)
//

#ifndef SSEC_QUADRATIC_FIELD_H
#define SSEC_QUADRATIC_FIELD_H

#include "fp.h"

typedef struct {
    fp_t re;// real part: a
    fp_t im;// imaginary part: b
} fp2_t;    // Elements of the form a + ib where i²=-1


void fp2_add(fp2_t *output, fp2_t input_a, fp2_t input_b);
void fp2_sub(fp2_t *output, fp2_t input_a, fp2_t input_b);
void fp2_mul(fp2_t *output, fp2_t input_a, fp2_t input_b);
void fp2_sqr(fp2_t *output, fp2_t input);
void fp2_neg(fp2_t *output, fp2_t input);

void fp2_set_to_one(fp2_t *output);
void fp2_set_to_zero(fp2_t *output);
void fp2_copy(fp2_t *output, fp2_t input);
void fp2_cswap(fp2_t *input_a, fp2_t *input_b, uint64_t input);
void fp2_cset(fp2_t *output, fp2_t input, uint64_t input_mask);

void fp2_inv(fp2_t *output, fp2_t input);
void fp2_half(fp2_t *output, fp2_t input);

void fp2_to_mont(fp2_t *output, fp2_t input);
void fp2_from_mont(fp2_t *output, fp2_t input);

void fp2_conj(fp2_t *output, fp2_t input);

void fp2_to_bytes(uint8_t *output, fp2_t input);
void fp2_from_bytes(fp2_t *output, const uint8_t *input);

int64_t fp2_is_zero(fp2_t input);
uint8_t fp2_is_equal(fp2_t input_a, fp2_t input_b);
void fp2_linear_pass_in(fp2_t *output, const fp2_t *input, uint8_t input_length, uint8_t input_index);
void fp2_linear_pass_out(fp2_t *output, fp2_t input, uint8_t input_length, uint8_t input_index);
uint8_t fp2_locate_zero(const fp2_t *input, uint8_t input_length);

uint8_t fp2_is_square(fp2_t input);

void fp2_batchinv(fp2_t *output_list, const fp2_t *input_list, uint8_t input_length);
void fp2_sqrt_slow(fp2_t *output, fp2_t input);
void fp2_sqrt_fast(fp2_t *output, fp2_t input);
void fp2_curt(fp2_t *output, fp2_t input);
#endif // SSEC_QUADRATIC_FIELD_H
