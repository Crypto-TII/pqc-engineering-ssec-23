#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>
#include "../common/fips202.h"

#define HASH_BYTES NUMBER_OF_WORDS*8
#define HASH(data,len,out) shake256(out, HASH_BYTES, data, len);

#if defined AVX2
    #include "../common/fp/avx2/fp-avx2.h"
#elif defined GMP
    #include "../common/fp/gmp/fp-gmp.h"
#elif defined KARATSUBA
    #include "../common/fp/karatsuba/fp-karatsuba.h"
#else
    #include "../common/fp/mulx/fp.h"
#endif
#include "../common/namespace.h"
#include "../common/primes.h"
#include "ctidh.h"
// #include "cpucycles.h"

const size_t NSAPI(pk_size) = sizeof(public_key) * 1;
const size_t NSAPI(sk_size) = sizeof(private_key);
const size_t NSAPI(ss_size) = sizeof(fp) * 1;

#define secsidh_keygen NSAPI(keygen)
#define secsidh_derive NSAPI(derive)

#define cprintf(...) printf(__VA_ARGS__)
// #define N primes_num 

#include "ctidh_api.h"
#include "ctidh.h"

#ifdef ENABLE_CT_TESTING
#include <valgrind/memcheck.h>
#endif


// void action(public_key *out, public_key const *in, private_key const *priv)

// Public (Montgomery curve affine coefficient) and private (integer vector) keys generation
void internal_keygen(public_key* pk, private_key* sk)
{
	ctidh_private((private_key*) sk);			// random private integer vector

    action(pk,&base,(private_key*)sk);    
  
    // we need to compute the seed of the full order point
    fp u;
    fulltorsion_points(u, pk->A);

    pk->seed = u[0];
    printf("seed: %ld\n", pk->seed);
}



// Secret sharing derivation (Montgomery curve affine coefficient)
bool internal_derive(fp* ss, public_key* const pk, private_key* const sk)
{
	
#ifdef ENABLE_CT_TESTING    
    VALGRIND_MAKE_MEM_DEFINED(pk, sizeof(public_key));
#endif     
    
    if (!validate((public_key*)pk)) return 0;	// validating the input Montgomery curve affine coefficiente (it must be supersingular!)

    public_key shared;
	action((public_key*)&shared, pk,  sk);	// Secrect sharing Montgomery curve affine coefficient: [sk] * pk
    
    //HASH((uint8_t*)shared.A, sizeof(fp), (uint8_t*) ss);    
    fp_copy(*ss, shared.A);
    return 1;
}

void skgen(int8_t* sk)                      // secret key generation
{
    ctidh_private((private_key*) sk);
}				

void pkgen(public_key* pk, int8_t* const sk)        // public key generation
{
    action(pk,&base,(private_key*)sk);
}			

// ----------------------------------- helpers for the interface between static and public API

#define SECSIDH_SUCCESS 0
#define SECSIDH_FAILURE -1


/* Safely clear a buffer b of size s */
static inline
void secsidh_clear(void *b, size_t s)
{
    /*
     * TODO: tricks might be needed to ensure the compiler does not
     *       strip this call away.
     *
     * Potentially this could be an externally provided function at some
     * point.
     */
    memset(b, 0, s);
}

/*
 * Converts the internal representation of a secret key into an octet
 * buffer.
 *
 * TODO: at some point this should take care of endianess issues.
 */
static inline
void secsidh_sk2oct(uint8_t *buf, const private_key *sk)
{
    memcpy(buf, sk,sizeof(private_key));
}

/*
 * Converts the internal representation of a public key into an octet
 * buffer.
 *
 * TODO: at some point this should take care of endianess issues.
 */
static inline
void secsidh_pk2oct(uint8_t *buf, const public_key *pk)
{
    // uint8_t test[2 * sizeof(fp)];
    //memcpy(test, pk, 2*sizeof(fp));

    // fp_2oct(buf + sizeof(fp), &pk[1]);

    memcpy(&buf[sizeof(fp)], &pk->seed, sizeof(uint64_t));
    fp_2oct(buf, &pk->A);


    //memcpy(buf, pk, sizeof(public_key));

    // assert(memcmp(test, buf, sizeof(test))==0);
}

/*
 * Converts the internal representation of a shared secret into an octet
 * buffer.
 *
 * TODO: at some point this should take care of endianess issues.
 */
static inline
void secsidh_ss2oct(uint8_t *buf, const fp ss[1])
{
    fp_2oct(buf, ss);
}

/*
 * Converts the octet buffer representation of a public key into our
 * internal representation.
 *
 * TODO: at some point this should take care of endianess issues.
 */
static inline
void secsidh_oct2pk(public_key *pk, const uint8_t *buf)
{
    memcpy(&pk->seed, &buf[sizeof(fp)], sizeof(uint64_t));
    oct2_fp(&pk->A, buf);


    //memcpy(pk, buf, sizeof(public_key));
}

/*
 * Converts the octet buffer representation of a secret key into our
 * internal representation.
 *
 * TODO: at some point this should take care of endianess issues.
 */
static inline
void secsidh_oct2sk(private_key *sk, const uint8_t *buf)
{
    memcpy(sk, buf, sizeof(private_key));
}

// ----------------------------------- Interface between static and public API

int secsidh_keygen(uint8_t *pk, uint8_t *sk)
{
    public_key ipk[2] = {0};
    private_key isk;
    internal_keygen(ipk, &isk);


    secsidh_pk2oct(pk, (const public_key*)ipk);

    secsidh_sk2oct(sk, &isk);
    secsidh_clear(&isk, sizeof(isk));    

    return SECSIDH_SUCCESS;
}

int secsidh_derive(uint8_t *ss, const uint8_t *peer_pk, const uint8_t *sk)
{
    int ret;
    public_key ipeer_pk = {0};
    fp iss;
    private_key isk;

    secsidh_oct2pk(&ipeer_pk, peer_pk);
    memset(&iss, 0, sizeof(fp));

    secsidh_oct2sk(&isk, sk);

    ret = internal_derive(&iss, &ipeer_pk, &isk) == 1 ? SECSIDH_SUCCESS : SECSIDH_FAILURE;


    secsidh_clear(&isk, sizeof(isk));
    secsidh_ss2oct(ss, (const fp*)iss);
    secsidh_clear(iss, sizeof(iss));

    return ret;
}
