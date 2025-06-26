#undef NDEBUG
#include <time.h>

#include <secsidh/secsidh.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cycle.h"
#include "fp-counters.h"
#include "primes.h"
#include "ctidh.h"
#include "ctidh_api.h"

#if GLOBAL_COUNTERS != 0
#define cprintf(...) printf(__VA_ARGS__)
#else
#define cprintf(...)
#endif

#define CPASTER(x, y) SECSIDH_CTIDH##x##_##y
#define CEVALUATOR(x, y) CPASTER(x, y)
#define CONSTNAMESPACE(name) CEVALUATOR(BITS, name)
#define SK_SIZE CONSTNAMESPACE(SK_SIZE)
#define PK_SIZE CONSTNAMESPACE(PK_SIZE)
#define SS_SIZE CONSTNAMESPACE(SS_SIZE)

#define pk_size FNAMESPACE(pk_size)
#define sk_size FNAMESPACE(sk_size)
#define ss_size FNAMESPACE(ss_size)

#define FPASTER(x, y) secsidh_CTIDH##x##_##y
#define FEVALUATOR(x, y) FPASTER(x, y)
#define FNAMESPACE(name) FEVALUATOR(BITS, name)

#define keygen FNAMESPACE(keygen)
#define derive FNAMESPACE(derive)

#ifdef ENABLE_CT_TESTING
#include <valgrind/memcheck.h>
#endif

static int dumb_fp_isequal(const uint8_t *a, const uint8_t *b, size_t size)
{
#ifdef ENABLE_CT_TESTING
    VALGRIND_MAKE_MEM_DEFINED(a, size);
    VALGRIND_MAKE_MEM_DEFINED(b, size);  
#endif
    return memcmp(a, b, size) == 0;
}

int main(void)
{

    printf("\033[0;33m// Key generation\033[0m\n");

    printf("sizeof private key = %lu \n", sizeof(private_key));
    printf("sizeof public key = %lu \n", sizeof(public_key));

    // ----------
    // Alice
    printf("\n\033[0;35m// Alice\033[0m\n");
    uint8_t a[SK_SIZE] = {0}, A[PK_SIZE] = {0}, ss_a[SS_SIZE] = {0};
    keygen(A, a);

    // ----------
    // Bob
    printf("\n\033[0;34m// Bob\033[0m\n");
    uint8_t b[SK_SIZE], B[PK_SIZE], ss_b[SS_SIZE];
    keygen(B, b);

    //------------------------------------------------------
    // Secret sharing derivation
    printf("\033[0;33m// Secret sharing generation\033[0m\n");

    // ----------------
    // Alice
    printf("\n\033[0;35m// Alice\033[0m\n");
    assert(derive(ss_a, B, a) == 0);

    printf("\n\033[0;34m// Bob\033[0m\n");
    assert(derive(ss_b, A, b) == 0);

    // =============================
    // Verifying same secret sharing
    assert(dumb_fp_isequal(ss_a, ss_b, SS_SIZE));

    //------------------------------------------------------
    printf("\n\033[0;32m// Successfully secret sharing computation!\033[0m\n");
    return 0;
}

