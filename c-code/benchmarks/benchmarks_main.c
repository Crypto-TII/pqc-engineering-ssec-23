#include <stdio.h>
#include <cgl_hash.h>
#include "benchmarks_utils.h"
#include <string.h>

#include <utilities.h>

// A utility function to swap two elements
void swap(ticks* a, ticks* b) {
	ticks t = *a;
	*a = *b;
	*b = t;
}

/* This function takes last element as pivot, places
   the pivot element at EXPERIMENTS correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (ticks arr[], int low, int high) {
	ticks pivot = arr[high];    // pivot
	int i = (low - 1);  // Index of smaller element

	int j;
	for(j = low; j <= high- 1; j++)
	{
		// If current element is smaller than the pivot
		if(arr[j] < pivot)
		{
			i++;    // increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

/* The benchmarks function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quicksort(ticks arr[], int low, int high) {
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now at right place */
		int pi = partition(arr, low, high);

		// Separately sort elements before
		// partition and after partition
		quicksort(arr, low, pi - 1);
		quicksort(arr, pi + 1, high);
	}
}

int main(int argc, char ** argv) {

    hal_init_perfcounters(1, 1);
//    const char * name_of_executable = strrchr(argv[0], '/');
//    printf("Test for %s!\n",name_of_executable);

	// --------------------------
	// For measuring clock cycles
	ticks mark_min = 0xFFFFFFFFFFFFFFFF;
    ticks mark_max = 0;
	ticks mark_mean = 0;
	ticks mark_two_isogeny[NUMBER_OF_BENCHMARK_EXPERIMENTS] = {0};
    ticks mark_three_isogeny[NUMBER_OF_BENCHMARK_EXPERIMENTS] = {0};
    ticks mark_0 = 0;
    ticks mark_1 = 0;

#if defined(NDEBUG)
    ticks mark_current = 0;
#endif

	// -----------------------------------------------------
    // 2-isogeny walks
	cgl_hash_2_ctx ctx2;
    fp2_t A, t;

    fp2_set_to_zero(&A);
    fp2_set_to_one(&t);
    fp2_add(&A, t, t);
    fp2_add(&A, A, t);
    fp2_add(&A, A, A);

	cgl_hash_init_2(&ctx2, A);
    
    //------------------------------------------------------
	// Main loop
    
    printf("\nNumbers correspond for \033[1;33mCGLHash2\033[0m.\n");
	for (int i = 0; i < NUMBER_OF_BENCHMARK_EXPERIMENTS; i++) {
		printf("// Running experiments:\t");
		printf("%2d%%", 100 * i / NUMBER_OF_BENCHMARK_EXPERIMENTS);
		fflush(stdout);
		printf("\r\x1b[K");
		// ----------------

        uint8_t path[BIT_LENGTH_PATH / 8] = {0};
        isogeny_walks_sample_bit_string(path);

        mark_0 = get_timestamp();
		cgl_hash_digest_2(&A, &ctx2, path);
        mark_1 = get_timestamp();

		// ---------------------------------
		// storing the measured clock cycles
        mark_two_isogeny[i] = 0;
        mark_two_isogeny[i] = mark_1 - mark_0;
		// Minimum value
		if(mark_min > mark_two_isogeny[i])
            mark_min = mark_two_isogeny[i];
		// Maximum value
		if(mark_max < mark_two_isogeny[i])
            mark_max = mark_two_isogeny[i];
		// Average value
		mark_mean += mark_two_isogeny[i];
	}
    mark_mean = mark_mean / NUMBER_OF_BENCHMARK_EXPERIMENTS;

	printf("Average: \033[1;32m%llu\033[0m\n\n", (unsigned long long)mark_mean );

#if defined(NDEBUG)
    quicksort(mark_two_isogeny, 0, NUMBER_OF_BENCHMARK_EXPERIMENTS - 1);
	// -----------------------------------------------------------
	mark_current = mark_two_isogeny[NUMBER_OF_BENCHMARK_EXPERIMENTS / 4];
	printf("Q1:      \033[1;33m%llu\033[0m\n", (unsigned long long)mark_current );
    // -----------------------------------------------------------
	mark_current = (mark_two_isogeny[NUMBER_OF_BENCHMARK_EXPERIMENTS / 2] + mark_two_isogeny[NUMBER_OF_BENCHMARK_EXPERIMENTS / 2 - 1]) / 2;
	printf("Median:  \033[1;33m%llu\033[0m\n", (unsigned long long)mark_current );
    // -----------------------------------------------------------
    mark_current = mark_two_isogeny[(3 * NUMBER_OF_BENCHMARK_EXPERIMENTS) / 4];
	printf("Q3:      \033[1;33m%llu\033[0m\n", (unsigned long long)mark_current );
	
	// -----------------------------------------------------------
	printf("\n");
	printf("Min:     \033[1;34m%llu\033[0m\n", (unsigned long long)mark_min );
	printf("Max:     \033[1;31m%llu\033[0m\n", (unsigned long long)mark_max );
	printf("\n");
#endif

    //------------------------------------------------------
    //------------------------------------------------------
    // 3-isogeny walk

    mark_min = 0xFFFFFFFFFFFFFFFF;
    mark_max = 0;
    mark_mean = 0;

#if defined(NDEBUG)
    mark_current = 0;
#endif

	cgl_hash_3_ctx ctx3;
    fp2_set_to_zero(&A);
    fp2_set_to_one(&t);
    fp2_add(&A, t, t);
    fp2_add(&A, A, t);
    fp2_add(&A, A, A);

    // For simplicity we take the first x-coordinate point
	cgl_hash_init_3(&ctx3, A, 0);

	//------------------------------------------------------
	// Main loop
    
    printf("Numbers correspond for \033[1;33mCGLHash3\033[0m.\n");
	for (int i = 0; i < NUMBER_OF_BENCHMARK_EXPERIMENTS; i++) {
		printf("// Running experiments:\t");
		printf("%2d%%", 100 * i / NUMBER_OF_BENCHMARK_EXPERIMENTS);
		fflush(stdout);
		printf("\r\x1b[K");
		// ----------------
        uint8_t path[TRITLENGTH_PATH / 5] = {0};
        isogeny_walks_sample_trit_string(path);

        mark_0 = get_timestamp();
		cgl_hash_digest_3(&A, &ctx3, path);
        mark_1 = get_timestamp();

		// ---------------------------------
		// storing the measured clock cycles
        mark_three_isogeny[i] = 0;
        mark_three_isogeny[i] = mark_1 - mark_0;
		// Minimum value
		if(mark_min > mark_three_isogeny[i])
            mark_min = mark_three_isogeny[i];
		// Maximum value
		if(mark_max < mark_three_isogeny[i])
            mark_max = mark_three_isogeny[i];
		// Average value
		mark_mean += mark_three_isogeny[i];
	}
    mark_mean = mark_mean / NUMBER_OF_BENCHMARK_EXPERIMENTS;

	printf("Average: \033[1;32m%llu\033[0m\n\n", (unsigned long long)mark_mean );

#if defined(NDEBUG)
    quicksort(mark_three_isogeny, 0, NUMBER_OF_BENCHMARK_EXPERIMENTS - 1);
	// -----------------------------------------------------------
	mark_current = mark_three_isogeny[NUMBER_OF_BENCHMARK_EXPERIMENTS / 4];
	printf("Q1:      \033[1;33m%llu\033[0m\n", (unsigned long long)mark_current );
    // -----------------------------------------------------------
	mark_current = (mark_three_isogeny[NUMBER_OF_BENCHMARK_EXPERIMENTS / 2] + mark_three_isogeny[NUMBER_OF_BENCHMARK_EXPERIMENTS / 2 - 1]) / 2;
	printf("Median:  \033[1;33m%llu\033[0m\n", (unsigned long long)mark_current );
    // -----------------------------------------------------------
    mark_current = mark_three_isogeny[(3 * NUMBER_OF_BENCHMARK_EXPERIMENTS) / 4];
	printf("Q3:      \033[1;33m%llu\033[0m\n", (unsigned long long)mark_current );
	
	// -----------------------------------------------------------
	printf("\n");
	printf("Min:     \033[1;34m%llu\033[0m\n", (unsigned long long)mark_min );
	printf("Max:     \033[1;31m%llu\033[0m\n", (unsigned long long)mark_max );
	printf("\n");
#endif

    char benchmarks_statistics_file_name[256];

    strcpy(benchmarks_statistics_file_name, "BENCH-STATS-");

#if defined(CYCLES)
    strcat(benchmarks_statistics_file_name, "CLOCK-CYCLES-");
#elif defined(TIME)
    strcat(benchmarks_statistics_file_name, "NANOSECONDS-");
#endif

    strcat(benchmarks_statistics_file_name, FIELD_NAME);
    strcat(benchmarks_statistics_file_name, ".csv");

    printStatisticsFile(benchmarks_statistics_file_name, mark_two_isogeny, mark_three_isogeny);

	return 0;
}
