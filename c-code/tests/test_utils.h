#ifndef SSEC_TEST_UTILS_H
#define SSEC_TEST_UTILS_H

#include "munit.h"
#include <fp2.h>
#include <utilities.h>
#include <stdio.h>

static inline __attribute__((always_inline)) void escape(void *p) { __asm__ __volatile__("" : "+m,r"(p) : : "memory"); }

#define TEST_END \
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
#define SUITE_END \
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
#define PARAM_END \
    { NULL, NULL }


#define TEST_CASE(test_func) \
    { #test_func, test_func, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

#define TEST_CASE_SETUP(test_func, setup, teardown) \
    { #test_func, test_func, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL }

#define BENCHMARK(test_func, setup, teardown, params) \
    { #test_func, test_func, setup, teardown, MUNIT_TEST_OPTION_SINGLE_ITERATION, params }

#define SUITE(suite_name, test_cases) \
    { suite_name, test_cases, NULL, 0, MUNIT_SUITE_OPTION_NONE }

#define SUITE_IT(suite_name, test_cases, iterations) \
    { suite_name, test_cases, NULL, iterations, MUNIT_SUITE_OPTION_NONE }

#define SUITE_OF_SUITE(suite_name, nested_suite) \
    { suite_name, NULL, nested_suite, 0, MUNIT_SUITE_OPTION_NONE }


#define RANDOM_FP_ELEMENT(out) \
{                                                                                           \
    munit_rand_memory(FIELD_BYTES, (munit_uint8_t *) (out));                                \
    (out)[FIELD_64BITS_WORDS - 1] &= MASK_FIELD_ELEMENT;                                    \
    while (!multiprecision_is_smaller((out), FIELD_CHARACTERISTIC, FIELD_64BITS_WORDS)) {   \
        munit_rand_memory(FIELD_BYTES, (munit_uint8_t *) (out));                            \
        (out)[FIELD_64BITS_WORDS - 1] &= MASK_FIELD_ELEMENT;                                \
    }                                                                                       \
}

#define RANDOM_SEED(out) \
{                                                                             \
    munit_rand_memory(SECURITY_BITS / 8, (munit_uint8_t *) (out));            \
}

static void RANDOM_FP2_ELEMENT(fp2_t *output) {
    RANDOM_FP_ELEMENT(output->re);
    RANDOM_FP_ELEMENT(output->im);
}

#define RANDOM_BIT_STRING(out) \
{                                                                             \
    munit_rand_memory(BIT_LENGTH_PATH / 8, (munit_uint8_t *) (out));            \
}

// Next function should be implemented with is_smaller (in ct)
#define RANDOM_TRIT_STRING(out) \
{                                                                \
    uint8_t bound = 0xF3;                                        \
    for(int i = 0; i < (TRITLENGTH_PATH / 5); i++) {             \
        uint8_t trit_str;                                        \
        munit_rand_memory(1, (munit_uint8_t *) (&trit_str));     \
        while (issmaller(bound, trit_str)) {                     \
            munit_rand_memory(1, (munit_uint8_t *) (&trit_str)); \
        }                                                        \
        out[i] = trit_str;                                       \
    }                                                            \
}

// KAT API
#define	MAX_MARKER_LEN          50

static inline int FindMarker(FILE *infile, const char *marker) {
    char line[MAX_MARKER_LEN] = {0};
    int i, len;
    int curr_line;

    len = (int) strlen(marker);
    if (len > MAX_MARKER_LEN - 1) {
        len = MAX_MARKER_LEN - 1;
    }

    for (i = 0; i < len; i++) {
        curr_line = fgetc(infile);
        line[i] = curr_line;
        if (curr_line == EOF) {
            return 0;
        }
    }
    line[len] = '\0';

    while (1) {
        if (!strncmp(line, marker, len)) {
            return 1;
        }

        for (i = 0; i < len - 1; i++) {
            line[i] = line[i + 1];
        }
        curr_line = fgetc(infile);
        line[len - 1] = curr_line;
        if (curr_line == EOF) {
            return 0;
        }
        line[len] = '\0';
    }
}

static inline int convert_to_hex(int ch) {
    if (ch >= 48 && ch <= 57) {
        return (ch - 48);
    } else if (ch >= 64 && ch <= 70) {
        return (ch - 55);
    }
    return 0;
}

static inline int ReadHex(FILE *infile, unsigned char *A, int Length, char *str) {
    int count, ch1, ch2;

    if (Length == 0) {
        A[0] = 0x00;
        return 1;
    }
    memset(A, 0x00, Length);
    count = 0;
    if (FindMarker(infile, str)) {
        while (count < Length) {
            ch1 = fgetc(infile);
            ch2 = fgetc(infile);
            ch1 = convert_to_hex(ch1);
            ch2 = convert_to_hex(ch2);
            A[count] = ((unsigned char) (ch1 << 4) | (unsigned char) ch2);
            count++;
        }
    } else {
        return 0;
    }
    return 1;
}

static inline void file_print_bytes(FILE *fp, const char *txt, const uint8_t *data_bytes, uint32_t data_bytes_length) {
    fprintf(fp, "%s", txt);

    for (uint32_t i = 0; i < data_bytes_length; i++) {
        fprintf(fp, "%02X", data_bytes[i]);
    }
    fprintf(fp, "\n");
}

#endif // SSEC_TEST_UTILS_H
