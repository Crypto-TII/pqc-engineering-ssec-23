cmake_minimum_required(VERSION  3.22)
project(ssec VERSION 0.1 LANGUAGES C ASM)

enable_testing()

set(CMAKE_C_STANDARD 11)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(DEBUG_FLAGS -g -DDEBUG -O0 -Wall -fPIC -Wno-unused-function)
set(RELEASE_FLAGS -O3 -DNDEBUG -fPIC -march=native -mavx2)

if(APPLE)
    # REQUIRED for current ASM prime field arithmetic
    set(CONFIG_OS_FLAG "-D__APPLE__")
endif()

set(PRIMES
        254
        255
        381
        383
        398
        511
        575
        592
        765
        783
)

add_compile_definitions(
        "$<$<BOOL:${BOOLTEST}>:BOOLTEST>"
        "$<$<STREQUAL:${BENCHMARKING},CYCLES>:CYCLES>"
        "$<$<STREQUAL:${BENCHMARKING},TIME>:TIME>"
        "$<$<STREQUAL:${ARCHITECTURE},ARMV7l>:ARMV7l>"
        "$<$<STREQUAL:${ARCHITECTURE},AARCH64>:AARCH64>"
        "$<$<STREQUAL:${ARCHITECTURE},x8664>:x8664>"
)

add_subdirectory(src)
add_subdirectory(benchmarks)
add_subdirectory(tests)
