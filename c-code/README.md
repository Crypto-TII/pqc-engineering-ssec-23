# (Detailed) Build, Test and Benchmarking Framework Documentation

## Table of contents
1. [Introduction](#section_01)  
2. [Build](#section_02)  
   1. [Cmake configuration](#section_02_01)   
   2. [Make library targets](#section_02_02)   
   3. [Make test targets](#section_02_03)    
3. [Tests](#section_03)
4. [Benchmarking](#section_04)
5. [Benchmarking script](#section_05)
6. [Remarks](#section_06)
7. [Appendix: CPU benchmarking details](#section_07)
   1. [Benchmarking CPU cycles and Execution Time](#section_07_01)
8. [References](#section_08)


<a name="section_01"></a>
## 1. Introduction

In this document, we present detailed instructions of the build, test and benchmarking of our source code. This detailed explanation aims to provide insights of our source code.

In the following sections, the list of all the supported primes is: `p254, p255, p381, p383, p398, p511, p575, p592, p765`, and `p783`.

<a name="section_02"></a>
## 2. Build

The following instructions work on Linux using CMAKE. **NOTE: Only Intel x86_64 is currently supported.**

<a name="section_02_01"></a>
### 2.1. Cmake configuration

First configure the build directory with

```bash
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=<BUILD_TYPE> -B <folder-build-name>
```

where `<BUILD_TYPE>` is either **Release** or **Debug**. Optionally, add personal preference to CMAKE such as

```bash
-DCMAKE_MAKE_PROGRAM=path/to/ninja -G Ninja
```

An example of how to build the debug build type is shown below 

```bash
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Debug -B cmake-build-debug
```

<a name="section_02_02"></a>
### 2.2. Make library targets

Jump into the <folder-build-name> folder and build all target libraries for all supported primes:

```bash
pqc-engineering-ssec-23/c-code$ cd <folder-build-name>
pqc-engineering-ssec-23/c-code/<folder-build-name>$ make -j8
```
Example:

```bash
pqc-engineering-ssec-23/c-code$ cd cmake-build-debug
pqc-engineering-ssec-23/c-code/cmake-build-debug$ make -j8
```

To build target libraries individually, simply run

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ make ssec-p<PRIME>
```
where `<PRIME>` is one of the supported primes `p254, p255, p381, p383, p398, p511, p575, p592, p765`, and `p783`. Example:
```bash
pqc-engineering-ssec-23/c-code/cmake-build-debug$ make ssec-p254
```

<a name="section_02_03"></a>
### 2.3. Make test targets

Building all targets from last step will create all test targets as well.
To compile specific test target, simply run

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ make tests-ssec-p<PRIME>
```

where `<PRIME>` is one of the supported primes. Example:

```bash
pqc-engineering-ssec-23/c-code/cmake-build-debug$ make tests-ssec-p254
```

<a name="section_03"></a>
## 3. Tests

Ctest is supported and can be used to call tests using regex, with the command

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ ctest
```

Example:
```bash
pqc-engineering-ssec-23/c-code/cmake-build-debug$ ctest
```

will run all tests. To run all tests in parallel, run

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ make test-parallel
```

Example:

```bash
pqc-engineering-ssec-23/c-code/cmake-build-debug$ make test-parallel
```

Another way to run the tests is provided by

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$  ctest -V -R test
```

<a name="section_04"></a>
## 4. Benchmarking

For benchmarking, the correct commands must be used when doing the first cmake.

```bash
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Debug -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664 -B cmake-build-debug-cycles-x8664
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664 -B cmake-build-release-cycles-x8664
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Debug -DBENCHMARKING=TIME -DARCHITECTURE=x8664 -B cmake-build-debug-time-x8664
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=TIME -DARCHITECTURE=x8664 -B cmake-build-release-time-x8664
```
followed by
```bash
pqc-engineering-ssec-23/c-code$ cd <folder-build-name>
pqc-engineering-ssec-23/c-code/<folder-build-name>$ make -j8
```

**NOTE:** Benchmarking does not work for
```bash
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Release -B cmake-build-release
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Debug -B cmake-build-debug
```
In case you run the benchmarking in either one of these two build modes (without the `-DBENCHMARKING` and the `-DARCHITECTURE` flags), you will get the following error:

\image html ../gifs/03-examples/benchmarks_error.png width=60%

A successful built that can generate correct benchmarks is shown below.

\image html ../gifs/03-examples/benchmarks_ok.png width=60%

After building using the correct flags, to run the benchmarks in verbose mode, run

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ ctest -V -R bench
```

Individual test suites can be run similarly as

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ ctest -V -R test-ssec-p<PRIME>-fp
```
or a single test case by calling directly the test app as

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ cd tests
pqc-engineering-ssec-23/c-code/<folder-build-name>/tests$ ./tests-ssec-p<PRIME> tests/fp/add_and_sub
```

To run tests and benchmarks with more iterations, you can modify the number of times each test is evaluated with the following.

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ cd tests
pqc-engineering-ssec-23/c-code/<folder-build-name>/tests$ ./tests-ssec-p<PRIME> --iterations 10000
pqc-engineering-ssec-23/c-code/<folder-build-name>/tests$ cd ..
pqc-engineering-ssec-23/c-code/<folder-build-name>$ cd benchmarks  
pqc-engineering-ssec-23/c-code/<folder-build-name>/benchmarks$ ./benchmarks/benchmarks-ssec-p<PRIME> # By default 10000
```

Finally, another way to run the benchmarks is given by

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ benchmarks/benchmarks-ssec-p<PRIME> # By default 10000
```

<a name="section_05"></a>
## 5. Benchmarking script

A high-level benchmarking script exists in the `pqc-engineering-ssec-23/high-level-scripts` folder. To run it, first replace the `WORKSPACE` variable inside `benchmark_02_20250408.sh`, and then execute (in their respective folders)

```bash
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Debug -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664 -B cmake-build-debug-cycles-x8664
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664 -B cmake-build-release-cycles-x8664
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=TIME -DARCHITECTURE=x8664 -B cmake-build-debug-time-x8664
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=TIME -DARCHITECTURE=x8664 -B cmake-build-release-time-x8664
```
and
```bash
pqc-engineering-ssec-23/high-level-scripts$ mkdir results
pqc-engineering-ssec-23/high-level-scripts$ sudo bash benchmark_02_20250408.sh
```

This will create the benchmark results inside the `high-level-scripts/results` folder, with a timestamp. 

<a name="section_06"></a>
## 6. Remarks

* Our ASM implementation is based on [^1].
* The test concerning the 2-isogeny and 3-isogeny chains employs **Magma**. In particular, after running `ctest -V -R test` one can verify the correct of the chains by running

```bash
 magma tests/TEST-<PRIME>-deg2.log 
 magma tests/TEST-<PRIME>-deg3.log 
```

where `<PRIME>` is one of the included primes (e.g., see `src/asm/pYYY.s` or `src/parameters/pYYY.h`).

<a name="section_07"></a>
## 7. Appendix: CPU benchmarking details

Our testbed consists of machine with a 12th Gen. Intel(R) Core(TM) i9-12900H CPU and 32 Gb of RAM, running Ubuntu 20.04.6 LTS (64 bits). To avoid the operating system to use multi-thread or additional performance boosts, we disabled the Turbo Boost via the kernel driver
```shell
/sys/devices/system/cpu/intel_pstate/no_turbo
```
and the hyperthread via
```shell
/sys/devices/system/cpu/smt/control
```
where the part of the script concerning disabling both Turbo Boost and hyperthreading is shown below. Note that these boosting disabling commands work in Intel CPUs, and similar ARM instructions or kernel modules are needed in case the benchmarking is desired to be performed in ARM CPUs.

```shell
#!/bin/bash
TURBO=/sys/devices/system/cpu/intel_pstate/no_turbo
DISABLE_TURBO=1

HYPERT_CMD=/sys/devices/system/cpu/smt/control
HYPERT_STS=/sys/devices/system/cpu/smt/active
ENABLE_HT=on
DISABLE_HT=off

disableTurbo() {
    echo ${DISABLE_TURBO} > ${TURBO}
}

statusTurbo() {
    echo -n "Turbo status (0 is enabled): "
    cat ${TURBO}
}

disableHyperThread() {
    echo ${DISABLE_HT} > ${HYPERT_CMD}
}

echo "Setup benchmark"
#setup benchmark
disableTurbo
disableHyperThread
setPerformance
```

<a name="section_07_01"></a>
### 7.1. Benchmarking CPU cycles and Execution Time

We perform two types of benchmarking: CPU cycles and Execution time, measured in nanoseconds. To measure the CPU cycle count between executions of our tests, we used the `rdtsc` assembly instruction for Intel architectures.

```c
#if (defined(__GNUC__) || defined(__ICC) || defined(__SUNPRO_C))
   && defined(__x86_64__) && !defined(HAVE_TICK_COUNTER)

   typedef unsigned long long ticks;
   static __inline__ ticks getticks(void)
   {
      unsigned a, d;
      asm volatile("rdtsc" : "=a" (a), "=d" (d));
      return ((ticks)a) | (((ticks)d) << 32);
   }
   INLINE_ELAPSED(__inline__)
   #define HAVE_TICK_COUNTER
#endif
```

and for the execution time, we use the `timespec_get` instruction from `time.h`:

```c
#include <time.h>

int timespec_get(struct timespec *ts, int base);
```

and we use a custom `get_nanos` function that measures the time in nanoseconds

```c
static unsigned long long get_nanos(void)
{
   struct timespec ts;
   timespec_get(&ts, TIME_UTC);
   return (unsigned long long)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}
```
<a name="section_08"></a>
## 8. References

[^1]: CTIDH: faster constant-time CSIDH. (2021). IACR Transactions on Cryptographic Hardware and Embedded Systems, 2021(4), 351-387. https://doi.org/10.46586/tches.v2021.i4.351-387
