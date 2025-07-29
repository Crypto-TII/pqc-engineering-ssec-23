#!/bin/bash

#NOTE: create folder "results" next to benchmark_02.sh
# Folder stucture:
# - benchmark_02_20250408.sh - results (these two folders at the same level)
# To run:
# 1. Clear cmake-build folders with sudo
# $ sudo rm -rf pqc-engineering-ssec-23/c-code/cmake-build-*
# 2. Run script with sudo
# $ sudo bash benchmark_02_20250408.sh

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

# CHANGE THIS FOLDER TO THE LOCATION OF pqc-engineering-ssec-23/c-code
WORKSPACE=(location to the project)/pqc-engineering-ssec-23/c-code

# BE SURE THAT THESE FOLDERS EXIST (run cmake)
BUILD_DIR_CYCLES=${WORKSPACE}/cmake-build-release-cycles-x8664
CSV_DIR_CYCLES=${WORKSPACE}/cmake-build-release-cycles-x8664
APP_CYCLES=${BUILD_DIR_CYCLES}/benchmarks/benchmarks-ssec

BUILD_DIR_TIMES=${WORKSPACE}/cmake-build-release-time-x8664
CSV_DIR_TIMES=${WORKSPACE}/cmake-build-release-time-x8664
APP_TIMES=${BUILD_DIR_TIMES}/benchmarks/benchmarks-ssec

#BENCH_PARAMS="PERK/benchmark/api --show-stderr --param iterations 1000"
DIR=results-$(date +%Y%m%d_%H%M%S)

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

cleanupPKP() {
    rm -rf ${BUILD_DIR_CYCLES}
    rm -rf ${BUILD_DIR_TIMES}
}


echo "Setup benchmark"
#setup benchmark
disableTurbo
disableHyperThread
setPerformance

#print status
statusTurbo
statusHyperThread
statusGovernor

echo "Running benchmark"
#run benchmark
set -x

#increase stack memory for the larger parameters
ulimit -s 16384

for prime in 254 255 381 383 398 511 575 592 765 783
do
    {
        (mkdir -p results/${DIR})
        
        (cd ${BUILD_DIR_CYCLES} && make benchmarks-ssec-p$prime)
        (cd ${BUILD_DIR_CYCLES} && ${APP_CYCLES}-p$prime)
        (cp ${CSV_DIR_CYCLES}/BENCH-STATS-CLOCK-CYCLES-p$prime.csv results/${DIR}/)

        (cd ${BUILD_DIR_TIMES} && make benchmarks-ssec-p$prime)
        (cd ${BUILD_DIR_TIMES} && ${APP_TIMES}-p$prime)
        (cp ${CSV_DIR_TIMES}/BENCH-STATS-NANOSECONDS-p$prime.csv results/${DIR}/)
    }
done

# cleanupPKP

{ set +x; } 2>/dev/null

echo "Done!"