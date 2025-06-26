#include "test_declarations.h"
#include "test_utils.h"
#include <utilities.h>


// NOLINTNEXTLINE
static MunitSuite test_suites[] = {
        SUITE_IT("fp/", test_fp, NUMBER_OF_TEST_EXPERIMENTS),
        SUITE_IT("fq/", test_fq, NUMBER_OF_TEST_EXPERIMENTS),
        SUITE_IT("isogeny_walks/", test_isogeny_walks, NUMBER_OF_TEST_EXPERIMENTS),
        SUITE_END
};

static MunitSuite chained_suites[] = {
        SUITE_OF_SUITE("tests/", test_suites),
        //SUITE_OF_SUITE("benchmarks/", benchmark_suites),
        SUITE_END
};

static const MunitSuite test_suite = {"", NULL, chained_suites, 0, MUNIT_SUITE_OPTION_NONE};

/*
 * Main function
 */
int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)]) {

        // create/clean file: for testing random 2-isogeny walks
        char file_name2[256];

        strcpy(file_name2, "TEST-");
        strcat(file_name2, FIELD_NAME);
        strcat(file_name2, "-deg2.log");

        FILE *fptr2 = fopen(file_name2, "w");
        if (fptr2 == NULL) {
                printf("Could not open file");
                return 0;
        }
        fclose(fptr2);
        // create/clean file: for testing random 3-isogeny walks
        char file_name3[256];

        strcpy(file_name3, "TEST-");
        strcat(file_name3, FIELD_NAME);
        strcat(file_name3, "-deg3.log");

        FILE *fptr3 = fopen(file_name3, "w");
        if (fptr3 == NULL) {
                printf("Could not open file");
                return 0;
        }
        fclose(fptr3);

        return munit_suite_main(&test_suite, NULL, argc, argv);
}
