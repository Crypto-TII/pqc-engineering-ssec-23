//
// Prime field arithmetic GF(p)
//

#ifndef SSEC_PRIME_FIELD_H
#define SSEC_PRIME_FIELD_H

#include <stdint.h>
#include "rng.h"
#include "parameters.h"

typedef uint64_t fp_t[FIELD_64BITS_WORDS];


// Assembly coded functions
extern void fp_add(uint64_t* output, const uint64_t* input_a, const uint64_t* input_b);
extern void fp_sub(uint64_t* output, const uint64_t* input_a, const uint64_t* input_b);
extern void fp_mul(uint64_t* output, const uint64_t* input_a, const uint64_t* input_b);
extern void fp_sqr(uint64_t* output, const uint64_t* input_a);


// C coded functions
void fp_neg(fp_t output, const fp_t input);

void fp_set_to_one(fp_t output);
void fp_set_to_zero(fp_t output);
void fp_copy(fp_t output, const fp_t input);
void fp_cset(fp_t output, const fp_t input, uint64_t input_mask);
void fp_cswap(fp_t input_a, fp_t input_b, uint64_t input);

void fp_sample(fp_t output);

void fp_inv(fp_t output, const fp_t input);
void fp_half(fp_t output, const fp_t input);

void fp_to_mont(fp_t output, const fp_t input);
void fp_from_mont(fp_t output, const fp_t input);

int64_t fp_is_zero(const fp_t input);
uint8_t fp_is_equal(const fp_t input_a, const fp_t input_b);
uint8_t fp_is_smaller(const fp_t input1, const fp_t input2);
uint8_t fp_is_square(const fp_t input);

void fp_sqrt(fp_t output, const fp_t input);
void fp_curt(fp_t output, const fp_t input);

#endif // SSEC_PRIME_FIELD_H
