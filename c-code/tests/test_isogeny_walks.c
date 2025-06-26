#include "test_declarations.h"
#include "test_utils.h"
#include "isogeny_walks.h"

/*
 * Test cases
 */


static MunitResult mul_by_small_constants(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    fp2_t a, b, c, t;

    RANDOM_FP2_ELEMENT(&a);

    fp2_set_to_zero(&t);
    t.re[0] = 1488;
    fp2_to_mont(&t, t);
    fp2_mul(&c, a, t);
    fp2_mul_by_1488(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c); // Testing 1488 × a

    fp2_set_to_zero(&t);
    t.re[0] = 2976;
    fp2_to_mont(&t, t);
    fp2_mul(&c, a, t);
    fp2_mul_by_2976(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c); // Testing 2976 × a

    fp2_set_to_zero(&t);
    t.re[0] = 162000;
    fp2_to_mont(&t, t);
    fp2_mul(&c, a, t);
    fp2_mul_by_162000(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c); // Testing 162000 × a

    fp2_set_to_zero(&t);
    t.re[0] = 324000;
    fp2_to_mont(&t, t);
    fp2_mul(&c, a, t);
    fp2_mul_by_324000(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c); // Testing 324000 × a

    fp2_set_to_zero(&t);
    t.re[0] = 2532192;
    fp2_to_mont(&t, t);
    fp2_mul(&c, a, t);
    fp2_mul_by_2532192(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c); // Testing 2532192 × a
    
    fp2_set_to_zero(&t);
    t.re[0] = 645205500;
    fp2_to_mont(&t, t);
    fp2_mul(&c, a, t);
    fp2_mul_by_645205500(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c); // Testing 645205500 × a

    fp2_set_to_zero(&t);
    t.re[0] = 8748000000;
    fp2_to_mont(&t, t);
    fp2_mul(&c, a, t);
    fp2_mul_by_8748000000(&b, a);
    assert_memory_equal(sizeof(fp2_t), &b, &c); // Testing 8748000000 × a
    
    return MUNIT_OK;
}

static MunitResult degree_2(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {
    uint8_t path[BIT_LENGTH_PATH / 8] = {0};
    RANDOM_BIT_STRING(path)
    fp2_t j1, j0, j_invariant;
    
    fp2_set_to_zero(&j1);
    j1.re[0] = 287496;
    fp2_to_mont(&j1, j1);

    fp2_set_to_zero(&j0);
    j0.re[0] = 1728;
    fp2_to_mont(&j0, j0);

    // We take as origin a j-invariant defined over GF(p^2) excluding GF(p).
    isogeny_walks_2_slow(&j0, &j1, j0, j1, path, BIT_LENGTH_PATH / 8);
    isogeny_walks_2(&j_invariant, j0, j1, path, BIT_LENGTH_PATH / 8);

    // ++++++++++++++++++++++++++++++++++++
    // Test through magma script generated:
    // If you have installed magma on your machine, run: magma TEST-[FIELD_NAME]-deg2.log
    // Else, copy/paste the content of TEST-[FIELD_NAME]-deg2.log in online magma calculator (http://magma.maths.usyd.edu.au/calc/)
    // Remark. Take into account that the online calculator has a limit buffer.

    char file_name[256];
    strcpy(file_name, "TEST-");
    strcat(file_name, FIELD_NAME);
    strcat(file_name, "-deg2.log");

    FILE *fptr = fopen(file_name, "a");
    if (fptr == NULL) {
        printf("Could not open file");
        return 0;
    }

    file_print_bytes(fptr, "// PATH (Hexadecimal): ", path, BIT_LENGTH_PATH / 8);
    fprintf(fptr, "// j-invariant (from 287496 to j0):\n");
    fprintf(fptr, "// Re := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", j0.re[pos]);
    }
    fprintf(fptr, ";\n// Im := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", j0.im[pos]);
    }
    fprintf(fptr, ";\n");
    fprintf(fptr, "// j-invariant (from 287496 to j1):\n");
    fprintf(fptr, "// Re := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", j1.re[pos]);
    }
    fprintf(fptr, ";\n// Im := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", j1.im[pos]);
    }
    fprintf(fptr, ";\n");
    fprintf(fptr, "clear;\np := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", FIELD_CHARACTERISTIC[pos]);
    }

    fprintf(fptr, ";\n");
    fprintf(fptr, "fp := GF(p);\n");
    fprintf(fptr, "_<x> := PolynomialRing(fp);\n");
    fprintf(fptr, "fp2<i>:= ext<fp | x^2 + 1>;\n");

    fp2_t temp;
    fp2_from_mont(&temp, j_invariant);
    fprintf(fptr, "Re := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", temp.re[pos]);
    }
    fprintf(fptr, ";\nIm := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", temp.im[pos]);
    }
    fprintf(fptr, ";\n");
    fprintf(fptr, "assert(IsZero(Random(EllipticCurveFromjInvariant(fp2![Re, Im])) * (p + 1)) or IsZero(Random(EllipticCurveFromjInvariant(fp2![Re, Im])) * (p - 1)));\n\n");
    fclose(fptr);
    // ++++++++++++++++++++++++++++++++++++

    return MUNIT_OK;
}

static MunitResult trit_string(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {
    int32_t value_equality, expected_value = 0x00000000;
    uint8_t path[TRITLENGTH_PATH / 5] = {0};

    isogeny_walks_sample_trit_string(path);

    uint8_t bound = 0xF3;
    for(int i = 0; i < (TRITLENGTH_PATH / 5); i++) {
        value_equality = issmaller((int32_t)bound, (int32_t)(path[i]));
        assert_memory_equal(sizeof(uint8_t), &value_equality, &expected_value);
    }

    return MUNIT_OK;
}

static MunitResult degree_3(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {
    fp2_t A, t, xP[4], zero;
    fp2_set_to_zero(&zero);
    fp2_set_to_one(&t);
    fp2_add(&A, t, t);
    fp2_add(&A, A, t);
    fp2_add(&A, A, A);

    isogeny_walks_get_points_3(xP, A);
    
    fp2_t xP_squared, A_times_xP, temporal, auxiliar, six;
    fp2_add(&six, t, t);
    fp2_add(&six, six, t);
    fp2_add(&six, six, six);

    // Testing A = 6
    for(int i = 0; i < 4; i++) {
        fp2_sqr(&xP_squared, xP[i]);
        fp2_add(&temporal, xP_squared, xP_squared);
        fp2_add(&temporal, temporal, xP_squared);
        
        fp2_mul(&A_times_xP, A, xP[i]);
        fp2_add(&auxiliar, A_times_xP, A_times_xP);
        fp2_add(&auxiliar, auxiliar, auxiliar);

        fp2_add(&temporal, temporal, auxiliar);
        fp2_add(&temporal, temporal, six);
        fp2_mul(&temporal, xP_squared, temporal);
        fp2_sub(&temporal, temporal, t);

        assert_memory_equal(sizeof(fp2_t), &temporal, &zero);      // temporal must be zero
    }

    fp2_t a1, a3;

    // Each byte determines a coefficient in [0, 3^5 - 1]. Thus, it determines five trits
    uint8_t path[TRITLENGTH_PATH / 5] = {0};
    RANDOM_TRIT_STRING(path);
    uint32_t trit_string[TRITLENGTH_PATH / 5] = {0};
    to_trit_string(trit_string, path, TRITLENGTH_PATH / 5);

    isogeny_walks_from_montgomery_model_3(&a1, &a3, A, xP[0]); // For simplicity we take the first x-coordinate point
    isogeny_walks_3(&a1, &a3, a1, a3, path, TRITLENGTH_PATH / 5);

    fp2_t j_invariant;
    isogeny_walks_switch_from_model_3(&j_invariant, a1, a3);

    // ++++++++++++++++++++++++++++++++++++
    // Test through magma script generated:
    // If you have installed magma on your machine, run: magma TEST-[FIELD_NAME]-deg3.log
    // Else, copy/paste the content of TEST-[FIELD_NAME]-deg3.log in online magma calculator (http://magma.maths.usyd.edu.au/calc/)
    // Remark. Take into account that the online calculator has a limit buffer.

    char file_name[256];
    strcpy(file_name, "TEST-");
    strcat(file_name, FIELD_NAME);
    strcat(file_name, "-deg3.log");

    FILE *fptr = fopen(file_name, "a");
    if (fptr == NULL) {
        printf("Could not open file");
        return 0;
    }

    file_print_bytes(fptr, "// PATH (Hexadecimal): ", path, TRITLENGTH_PATH / 5);
    fprintf(fptr, "// PATH (TRIT string): ");
    for(int i = 0; i < (TRITLENGTH_PATH / 5); i++) {
        fprintf(fptr, "%05X", trit_string[i]);
    }
    fprintf(fptr, "\n// Montgomery Curve Coefficient (A): 6\n");
    fprintf(fptr, "// x-coordinate point (xP): First root deterministically chosen\n");
    fprintf(fptr, "clear;\np := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", FIELD_CHARACTERISTIC[pos]);
    }
    fprintf(fptr, ";\n");
    fprintf(fptr, "fp := GF(p);\n");
    fprintf(fptr, "_<x> := PolynomialRing(fp);\n");
    fprintf(fptr, "fp2<i>:= ext<fp | x^2 + 1>;\n");

    fp2_t temp;
    fp2_from_mont(&temp, j_invariant);
    fprintf(fptr, "Re := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", temp.re[pos]);
    }
    fprintf(fptr, ";\nIm := 0x");
    for(int pos = FIELD_64BITS_WORDS - 1; pos >= 0; pos--) {
        fprintf(fptr, "%016lX", temp.im[pos]);
    }
    fprintf(fptr, ";\n");
    fprintf(fptr, "assert(IsZero(Random(EllipticCurveFromjInvariant(fp2![Re, Im])) * (p + 1)) or IsZero(Random(EllipticCurveFromjInvariant(fp2![Re, Im])) * (p - 1)));\n\n");
    fclose(fptr);
    // ++++++++++++++++++++++++++++++++++++

    return MUNIT_OK;
}

static void print_fp(const fp_t input, char* s){
    fp_t aux;
    fp_from_mont(aux, input);
    printf("%s = 0x", s);
    for(int i = FIELD_64BITS_WORDS -1; i >= 0; i--){
        printf("%016lX", aux[i]);
    }
    printf(";\n");
}

static MunitResult degree_3_fp(MUNIT_UNUSED const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {
    if((FIELD_BITS == 381) || (FIELD_BITS == 575) || (FIELD_BITS == 765)){
        return MUNIT_SKIP;
    }
    fp_t input = {6, 0, 0, 0};
    fp_to_mont(input, input);

    fp_t A;
    fp_copy(A, input);
    for(int value = 0; value < 128; value++) {
        isogeny_walks_3_fp(A, A, value);
    }

    for(int value = 0; value < 128; value++) {
        isogeny_walks_3_fp(A, A, -value);
    }

    assert_memory_equal(sizeof(fp_t), &input, &A);

    return MUNIT_OK;
}


/*
 * Register test cases
 */

MunitTest test_isogeny_walks[] = {
        TEST_CASE(mul_by_small_constants),
        TEST_CASE(degree_2),
        TEST_CASE(trit_string),
        TEST_CASE(degree_3),
        TEST_CASE(degree_3_fp),
        TEST_END
};
