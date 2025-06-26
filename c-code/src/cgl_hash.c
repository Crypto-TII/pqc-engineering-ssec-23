//
// // Charles-Goren-Lauter Hash function
//

#include "cgl_hash.h"


// Using 2-isogenies
void cgl_hash_digest_2(fp2_t *output, const cgl_hash_2_ctx *ctx, const uint8_t *input_bitstring) {
    fp2_set_to_zero(output);

    isogeny_walks_2(output, ctx->j_, ctx->j0, input_bitstring, BIT_LENGTH_PATH);
}


void cgl_hash_init_2(cgl_hash_2_ctx *ctx, fp2_t input_A) {
    isogeny_walks_from_montgomery_model_2(&ctx->j0, input_A);
    isogeny_walks_get_previous_step_2(&ctx->j_, input_A);
}


// Using 3-isogenies
void cgl_hash_digest_3(fp2_t *output, const cgl_hash_3_ctx *ctx, const uint8_t *input_trit_string) {
    fp2_t a1, a3;
    fp2_set_to_zero(output);

    isogeny_walks_3(&a1, &a3, ctx->a1, ctx->a3, input_trit_string, TRITLENGTH_PATH);
    isogeny_walks_switch_from_model_3(output, a1, a3);
}


void cgl_hash_init_3(cgl_hash_3_ctx *ctx, fp2_t input_A, uint8_t choice) {
    fp2_t xP[4];
    isogeny_walks_get_points_3(xP, input_A);
    isogeny_walks_from_montgomery_model_3(&ctx->a1, &ctx->a3, input_A, xP[choice % 4]);
}
