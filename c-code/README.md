# Let us walk on the 3-isogeny graph: efficient, fast, and simple

## Primes supported

We support the following primes: `p254, p255, p381, p383, p398, p511, p575, p592, p765`, and `p783`.

## Compilation

Compilation for Linux and MacOS using CMAKE

### cmake configuration

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

### Make library targets

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

### Make test targets

Building all targets from last step will create all test targets as well.
To compile specific test target, simply run

```bash
pqc-engineering-ssec-23/c-code/<folder-build-name>$ make tests-ssec-p<PRIME>
```

where `<PRIME>` is one of the supported primes. Example:

```bash
pqc-engineering-ssec-23/c-code/cmake-build-debug$ make tests-ssec-p254
```

## Tests

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

## Benchmarking

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

NOTE: Benchmarking does not work for
```bash
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Release -B cmake-build-release
pqc-engineering-ssec-23/c-code$ cmake -DCMAKE_BUILD_TYPE=Debug -B cmake-build-debug
```

To run the benchmarks in verbose mode, run

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

## Benchmarking script

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

### Remarks

* Our ASM implementation is based on [^1].
* The test concerning the 2-isogeny and 3-isogeny chains employs **Magma**. In particular, after running `ctest -V -R test` one can verify the correct of the chains by running

```bash
 magma tests/TEST-<PRIME>-deg2.log 
 magma tests/TEST-<PRIME>-deg3.log 
```

where `<PRIME>` is one of the included primes (e.g., see `src/asm/pYYY.s` or `src/parameters/pYYY.h`).

### References

[^1]: CTIDH: faster constant-time CSIDH. (2021). IACR Transactions on Cryptographic Hardware and Embedded Systems, 2021(4), 351-387. https://doi.org/10.46586/tches.v2021.i4.351-387
