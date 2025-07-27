#!/bin/bash

cd ../../c-code
echo "cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664 -B cmake-build-release-cycles-x8664-graph-04"
cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664 -B cmake-build-release-cycles-x8664-graph-04
echo "cd cmake-build-release-cycles-x8664-graph-04"
cd cmake-build-release-cycles-x8664-graph-04
echo "make -j"
make -j

echo "benchmarks/benchmarks-ssec-p381 | tee benchmarks_ssec-p381-output.txt"
benchmarks/benchmarks-ssec-p381 | tee benchmarks_ssec-p381-output.txt

echo "benchmarks/benchmarks-ssec-p398 | tee benchmarks_ssec-p398-output.txt"
benchmarks/benchmarks-ssec-p398 | tee benchmarks_ssec-p398-output.txt

echo "benchmarks/benchmarks-ssec-p575 | tee benchmarks_ssec-p575-output.txt"
benchmarks/benchmarks-ssec-p575 | tee benchmarks_ssec-p575-output.txt

echo "benchmarks/benchmarks-ssec-p592 | tee benchmarks_ssec-p592-output.txt"
benchmarks/benchmarks-ssec-p592 | tee benchmarks_ssec-p592-output.txt

echo "benchmarks/benchmarks-ssec-p765 | tee benchmarks_ssec-p765-output.txt"
benchmarks/benchmarks-ssec-p765 | tee benchmarks_ssec-p765-output.txt

echo "benchmarks/benchmarks-ssec-p783 | tee benchmarks_ssec-p783-output.txt"
benchmarks/benchmarks-ssec-p783 | tee benchmarks_ssec-p783-output.txt

cd ../../reproduce_results/manuscript_figure_04
python3 benchmark_graph_04.py
