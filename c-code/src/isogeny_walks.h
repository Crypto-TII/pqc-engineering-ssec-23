//
// Isogeny walks via radical computations
//

#ifndef SSEC_ISOGENY_WALKS_H
#define SSEC_ISOGENY_WALKS_H

#include "fp2.h"

// 2-isogenies

// Multiplications by specific "small" constants
void fp2_mul_by_1488(fp2_t *output, fp2_t input);
void fp2_mul_by_2976(fp2_t *output, fp2_t input);
void fp2_mul_by_162000(fp2_t *output, fp2_t input);
void fp2_mul_by_324000(fp2_t *output, fp2_t input);
void fp2_mul_by_2532192(fp2_t *output, fp2_t input);
void fp2_mul_by_645205500(fp2_t *output, fp2_t input);
void fp2_mul_by_8748000000(fp2_t *output, fp2_t input);

void isogeny_walks_2(fp2_t *output, fp2_t input_prev, fp2_t input, const uint8_t *input_path, size_t input_length);
void isogeny_walks_from_montgomery_model_2(fp2_t *output_j, fp2_t input_A);
void isogeny_walks_get_previous_step_2(fp2_t *output_j, fp2_t input_A);

void isogeny_walks_sample_bit_string(uint8_t *output);
void isogeny_walks_2_slow(fp2_t *j0, fp2_t *j1, fp2_t input_prev, fp2_t input, const uint8_t *input_path, size_t input_length);

// 3-isogenies

void isogeny_walks_3(fp2_t *output_a1, fp2_t *output_a3, fp2_t input_a1, fp2_t input_a3, const uint8_t *input_path, size_t input_length);
void isogeny_walks_get_points_3(fp2_t *output, fp2_t input_A);
void isogeny_walks_from_montgomery_model_3(fp2_t *output_a1, fp2_t *output_a3, fp2_t input_A, fp2_t input_xP);
void isogeny_walks_switch_from_model_3(fp2_t *output_j, fp2_t input_a1, fp2_t input_a3);

void isogeny_walks_sample_trit_string(uint8_t *output);

void isogeny_walks_get_points_3_fp(fp_t *output, const fp_t *input_A);
void isogeny_walks_to_montgomery_model_3_fp(fp_t *output_A, const fp_t *input_t);
void isogeny_walks_3_fp(fp_t output_A, const fp_t input_A, int input_length);

#endif // SSEC_ISOGENY_WALKS_H
