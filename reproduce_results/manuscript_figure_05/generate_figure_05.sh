#!/bin/bash

#NOTE:
#   Folder structure:
#   reproduce_results
#     ├── manuscript_figure_03
#     ├── manuscript_figure_04
#     └── manuscript_figure_05
#         ├── (AUTOMATICALLY GENERATED FOLDER) dCTIDH_benchmarks_output
#         ├── (AUTOMATICALLY GENERATED FOLDER) dCTIDH_builds
#         ├── (AUTOMATICALLY GENERATED FOLDER) statistics_output
#         ├── analyze_bench.py
#         ├── benchmark_graph_05.py
#         └── generated_figure_05.sh   <= THIS FILE
# To run, simply
# 1. Add execution permission to this file: In terminal, run
#     chmod +x generate_figure_05.sh
# 2. Execute this file
#     ./generate_figure_05.sh
#
# NOTE: python3-numpy + python3-matplotlib are required

TOTAL_EXPERIMENTS=100                         # <= MODIFY FOR AMOUNT OF EXPERIMENTS TO RUN
RUN_BENCHMARKING_DCTIDH_BUILDS=1            # <= VALUE=1 means to run the benchmarkings from scratch, VALUE=0 means that the benchmarks were generated in the past, so it can be skipped.
OPT_RAND_LIST=(5 4 3 2 1 0)                 # <= Allowed values for OPT_RAND: 5, 4, 3, 2, 1, 0 (in order from faster to slower)
DIR_TIMED=results_$(date +%Y%m%d_%H%M%S)   # <= This variable generates a timestamped folder for each execution (of the form results_YYYYmmdd_HHMMSS), example 2025/07/28 23:07:46 generates 'results_20250728_23746'. Replace this variable if want to repeat multiple tests under the same benchmarkings.
#DIR_TIMED=results_20250728_230746




TURBO=/sys/devices/system/cpu/intel_pstate/no_turbo
DISABLE_TURBO=1
ENABLE_TURBO=0

HYPERT_CMD=/sys/devices/system/cpu/smt/control
HYPERT_STS=/sys/devices/system/cpu/smt/active
ENABLE_HT=on
DISABLE_HT=off

GOV=/sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
PERF_GOV=performance
SAVE_GOV=powersave

disableTurbo() {
    echo ${DISABLE_TURBO} > ${TURBO}
}

enableTurbo() {
    echo ${ENABLE_TURBO} > ${TURBO}
}

statusTurbo() {
    echo -n "Turbo status (0 is enabled): "
    cat ${TURBO}
}

disableHyperThread() {
    echo ${DISABLE_HT} > ${HYPERT_CMD}
}

enableHyperThread() {
    echo ${ENABLE_HT} > ${HYPERT_CMD}
}

statusHyperThread() {
    echo -n "Hyperthread status (1 is enabled): "
    cat ${HYPERT_STS}
}

setPerformance() {
    for i in ${GOV}
    do
        echo ${PERF_GOV} > $i
    done
}

setPowersave() {
    for i in ${GOV}
    do
        echo ${SAVE_GOV} > $i
    done
}

statusGovernor() {
    echo "Governor status"
    for i in ${GOV}
    do
        echo -n "${i} "
        cat ${i}
    done
}




echo "Setup benchmark OPT_RAD"
#setup benchmark
disableTurbo
disableHyperThread
setPerformance

#print status
statusTurbo
statusHyperThread
statusGovernor

echo "Running benchmark OPT_RAD"
#run benchmark
set -x

#increase stack memory for the larger parameters
ulimit -s 16384

prime01=2047m1l226
prime02=2047m4l205
prime03=2047m6l194

BENCHMARKS_OUTPUT_TIMED=dCTIDH_benchmarks_output/${DIR_TIMED}

if [[ "$RUN_BENCHMARKING_DCTIDH_BUILDS" -eq 1 ]]; then
  echo ""
  echo "========================================="
  echo "STAGE 1: Building and Benchmarking dCTIDH (modified version)"
  echo "========================================="
  echo ""

  #for opt_rad in 5 4 3 2 1 0
  for opt_rad in "${OPT_RAND_LIST[@]}"
  do
  {
    echo "Setting up benchmark OPT_RAD_$opt_rad..."

    mkdir -p dCTIDH_builds
    BUILD_OPT_RAD_FOLDER=dCTIDH_builds/build_opt_rad_$opt_rad
    (rm -rf ${BUILD_OPT_RAD_FOLDER} && mkdir -p ${BUILD_OPT_RAD_FOLDER})
    (cd ${BUILD_OPT_RAD_FOLDER} && cmake ../../../../dCTIDH  -DOPT_RAD=$opt_rad)
    (cd ${BUILD_OPT_RAD_FOLDER} && make)


    BENCHMARKS_OUTPUT_OPT_RAD_FOLDER=${BENCHMARKS_OUTPUT_TIMED}/OPT_RAD_$opt_rad
    (mkdir -p ${BENCHMARKS_OUTPUT_TIMED} && mkdir -p ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER})


    # prime02 = 2047m4l205
    echo "Running up benchmark OPT_RAD_$opt_rad - 2047m4l205 - keygen"
    BENCH_KEYGEN_PRIME02=bench_keygen_205_opt_rad_$opt_rad.out
    (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime02}.main -bkey ${TOTAL_EXPERIMENTS} > ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME02})

    echo "Running up benchmark OPT_RAD_$opt_rad - 2047m4l205 - act"
    BENCH_ACT_PRIME02=bench_act_205_opt_rad_$opt_rad.out
    (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime02}.main -bact ${TOTAL_EXPERIMENTS} > ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME02})


    # prime01 = 2047m1l226
    echo "Running up benchmark OPT_RAD_$opt_rad - 2047m1l226 - keygen"
    BENCH_KEYGEN_PRIME01=bench_keygen_226_opt_rad_$opt_rad.out
    (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime01}.main -bkey ${TOTAL_EXPERIMENTS} > ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME01})

    echo "Running up benchmark OPT_RAD_$opt_rad - 2047m1l226 - act"
    BENCH_ACT_PRIME01=bench_act_226_opt_rad_$opt_rad.out
    (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime01}.main -bact ${TOTAL_EXPERIMENTS} > ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME01})



    # prime03 = 2047m6l194
    echo "Running up benchmark OPT_RAD_$opt_rad - 2047m6l194 - keygen"
    BENCH_KEYGEN_PRIME03=bench_keygen_194_opt_rad_$opt_rad.out
    (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime03}.main -bkey ${TOTAL_EXPERIMENTS} > ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME03})

    echo "Running up benchmark OPT_RAD_$opt_rad - 2047m6l194 - act"
    BENCH_ACT_PRIME03=bench_act_194_opt_rad_$opt_rad.out
    (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime03}.main -bact ${TOTAL_EXPERIMENTS} > ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME03})

  }
  done

fi


echo ""
echo "========================================="
echo "STAGE 2: Parsing benchmarking results"
echo "========================================="
echo ""

STATISTICS_OUTPUT_DIR=statistics_output
(rm -rf ${STATISTICS_OUTPUT_DIR} && mkdir ${STATISTICS_OUTPUT_DIR})

for opt_rad in "${OPT_RAND_LIST[@]}"
do
{
  echo "Analyzing statistics for OPT_RAD_$opt_rad..."
  BENCHMARKS_OUTPUT_OPT_RAD_FOLDER=${BENCHMARKS_OUTPUT_TIMED}/OPT_RAD_$opt_rad

  # prime02 = 2047m4l205
  echo "----------------------------------------------------------"
  echo "Parsing benchmarking results for OPT_RAD_$opt_rad - 2047m4l205 - keygen"
  BENCH_KEYGEN_PRIME02=bench_keygen_205_opt_rad_$opt_rad.out
  BENCH_KEYGEN_PRIME02_PARSED=analyzed_bench_keygen_205_opt_rad_$opt_rad.txt
  (python3 analyze_bench.py < ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME02} | tee ${STATISTICS_OUTPUT_DIR}/${BENCH_KEYGEN_PRIME02_PARSED})

  echo "----------------------------------------------------------"
  echo "Parsing benchmarking results for OPT_RAD_$opt_rad - 2047m4l205 - act"
  BENCH_ACT_PRIME02=bench_act_205_opt_rad_$opt_rad.out
  BENCH_ACT_PRIME02_PARSED=analyzed_bench_act_205_opt_rad_$opt_rad.txt
  (python3 analyze_bench.py < ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME02} | tee ${STATISTICS_OUTPUT_DIR}/${BENCH_ACT_PRIME02_PARSED})



  # prime01 = 2047m1l226
  echo "----------------------------------------------------------"
  echo "Parsing benchmarking results for OPT_RAD_$opt_rad - 2047m1l226 - keygen"
  BENCH_KEYGEN_PRIME01=bench_keygen_226_opt_rad_$opt_rad.out
  BENCH_KEYGEN_PRIME01_PARSED=analyzed_bench_keygen_226_opt_rad_$opt_rad.txt
  (python3 analyze_bench.py < ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME01} | tee ${STATISTICS_OUTPUT_DIR}/${BENCH_KEYGEN_PRIME01_PARSED})

  echo "----------------------------------------------------------"
  echo "Parsing benchmarking results for OPT_RAD_$opt_rad - 2047m1l226 - act"
  BENCH_ACT_PRIME01=bench_act_226_opt_rad_$opt_rad.out
  BENCH_ACT_PRIME01_PARSED=analyzed_bench_act_226_opt_rad_$opt_rad.txt
  (python3 analyze_bench.py < ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME01} | tee ${STATISTICS_OUTPUT_DIR}/${BENCH_ACT_PRIME01_PARSED})




  # prime03 = 2047m6l194
  echo "----------------------------------------------------------"
  echo "Parsing benchmarking results for OPT_RAD_$opt_rad - 2047m6l194 - keygen"
  BENCH_KEYGEN_PRIME03=bench_keygen_194_opt_rad_$opt_rad.out
  BENCH_KEYGEN_PRIME03_PARSED=analyzed_bench_keygen_194_opt_rad_$opt_rad.txt
  (python3 analyze_bench.py < ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME03} | tee ${STATISTICS_OUTPUT_DIR}/${BENCH_KEYGEN_PRIME03_PARSED})

  echo "----------------------------------------------------------"
  echo "Parsing benchmarking results for OPT_RAD_$opt_rad - 2047m6l194 - act"
  BENCH_ACT_PRIME03=bench_act_194_opt_rad_$opt_rad.out
  BENCH_ACT_PRIME03_PARSED=analyzed_bench_act_194_opt_rad_$opt_rad.txt
  (python3 analyze_bench.py < ${BENCHMARKS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME03} | tee ${STATISTICS_OUTPUT_DIR}/${BENCH_ACT_PRIME03_PARSED})
}
done

# AT THIS POINT, THE FOLDER ${STATISTICS_OUTPUT_DIR} CONTAINS ALL THE STATISTICS THAT MUST BE ANALYZED FOR GENERATING FIGURE 5 (A) AND FIGURE 5 (B)

python3 benchmark_graph_05.py


# cleanupPKP

{ set +x; } 2>/dev/null

echo "Done!"