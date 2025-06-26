#ifndef _SECSIDH_CONFIG_H_
#define _SECSIDH_CONFIG_H_

#include <secsidh/secsidhconfig.h>
#include <stdint.h>


#ifdef SECSIDH_ENABLE_CTIDH2047m1l226

#define SECSIDH_CTIDH2047m1l226_PK_SIZE 264
#define SECSIDH_CTIDH2047m1l226_SK_SIZE 170
#define SECSIDH_CTIDH2047m1l226_SS_SIZE 256

int secsidh_CTIDH2047m1l226_keygen(uint8_t *pk, uint8_t *sk);
int secsidh_CTIDH2047m1l226_derive(uint8_t *ss, const uint8_t *pk, const uint8_t *sk);

#endif

#ifdef SECSIDH_ENABLE_CTIDH2047m4l205

#define SECSIDH_CTIDH2047m4l205_PK_SIZE 264
#define SECSIDH_CTIDH2047m4l205_SK_SIZE 170
#define SECSIDH_CTIDH2047m4l205_SS_SIZE 256

int secsidh_CTIDH2047m4l205_keygen(uint8_t *pk, uint8_t *sk);
int secsidh_CTIDH2047m4l205_derive(uint8_t *ss, const uint8_t *pk, const uint8_t *sk);

#endif

#ifdef SECSIDH_ENABLE_CTIDH2047m6l194

#define SECSIDH_CTIDH2047m6l194_PK_SIZE 264
#define SECSIDH_CTIDH2047m6l194_SK_SIZE 170
#define SECSIDH_CTIDH2047m6l194_SS_SIZE 256

int secsidh_CTIDH2047m6l194_keygen(uint8_t *pk, uint8_t *sk);
int secsidh_CTIDH2047m6l194_derive(uint8_t *ss, const uint8_t *pk, const uint8_t *sk);

#endif

#ifdef SECSIDH_ENABLE_CTIDH2047m7l188

#define SECSIDH_CTIDH2047m7l188_PK_SIZE 264
#define SECSIDH_CTIDH2047m7l188_SK_SIZE 170
#define SECSIDH_CTIDH2047m7l188_SS_SIZE 256

int secsidh_CTIDH2047m7l188_keygen(uint8_t *pk, uint8_t *sk);
int secsidh_CTIDH2047m7l188_derive(uint8_t *ss, const uint8_t *pk, const uint8_t *sk);

#endif


#endif
