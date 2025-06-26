//
// Charles-Goren-Lauter Hash function
//

#ifndef SSEC_CGL_HASH_H
#define SSEC_CGL_HASH_H

#include "isogeny_walks.h"

typedef struct cgl_hash_2_ctx {
    fp2_t j0; // This corresponds with j₀
    fp2_t j_; // This corresponds with j₋₁
} cgl_hash_2_ctx;

void cgl_hash_init_2(cgl_hash_2_ctx *ctx, fp2_t input_A);
void cgl_hash_digest_2(fp2_t *output, const cgl_hash_2_ctx *ctx, const uint8_t *input_bitstring);

typedef struct cgl_hash_3_ctx {
    fp2_t a1;
    fp2_t a3;
} cgl_hash_3_ctx;

void cgl_hash_init_3(cgl_hash_3_ctx *ctx, fp2_t input_A, uint8_t choice);
void cgl_hash_digest_3(fp2_t *output, const cgl_hash_3_ctx *ctx, const uint8_t *input_trit_string);

#endif // SSEC_CGL_HASH_H
