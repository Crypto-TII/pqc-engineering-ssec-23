#!/bin/bash

#NOTE: create folder "statistics_output" next to this .sh file
#   Folder stucture:
#   dCTIDH
#     ├── benchmark_opt_rad.sh
#     └── statistics_output
#         ├── OPT_RAD_0
#         ├── OPT_RAD_1
#         ├── OPT_RAD_2
#         ├── OPT_RAD_3
#         ├── OPT_RAD_4
#         └── OPT_RAD_5
# To run:
# dCTIDH$
# 1. Clear cmake-build folders with sudo
# $ sudo rm -rf ssec-23/dCTIDH/cmake-build-*
# 2. Run script with sudo
# $ sudo bash benchmark_opt_rad.sh

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

# CHANGE THIS FOLDER TO THE LOCATION OF ssec-23/c-code
WORKSPACE=../c-code


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

TOTAL_EXPERIMENTS=100

DIR_TIMED=results_$(date +%Y%m%d_%H%M%S)
for opt_rad in 5 4 3 2 1 0
#for opt_rad in 0
do
{
  echo "Setting up benchmark OPT_RAD_$opt_rad..."
  BUILD_OPT_RAD_FOLDER=build_opt_rad_$opt_rad
  (rm -rf ${BUILD_OPT_RAD_FOLDER} && mkdir -p ${BUILD_OPT_RAD_FOLDER})
  (cd ${BUILD_OPT_RAD_FOLDER} && cmake .. -DOPT_RAD=$opt_rad)
  (cd ${BUILD_OPT_RAD_FOLDER} && make)

  STATISTICS_OUTPUT_TIMED=statistics_output/${DIR_TIMED}
  STATISTICS_OUTPUT_OPT_RAD_FOLDER=${STATISTICS_OUTPUT_TIMED}/OPT_RAD_$opt_rad
  (mkdir -p ${STATISTICS_OUTPUT_TIMED} && mkdir -p ${STATISTICS_OUTPUT_OPT_RAD_FOLDER})


  # prime02 = 2047m4l205
  echo "Running up benchmark OPT_RAD_$opt_rad - 2047m4l205 - keygen"
  BENCH_KEYGEN_PRIME02=bench_keygen_205_opt_rad_$opt_rad.out
  (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime02}.main -bkey ${TOTAL_EXPERIMENTS} > ${STATISTICS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME02})

  echo "Running up benchmark OPT_RAD_$opt_rad - 2047m4l205 - act"
  BENCH_ACT_PRIME02=bench_act_205_opt_rad_$opt_rad.out
  (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime02}.main -bact ${TOTAL_EXPERIMENTS} > ${STATISTICS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME02})


  # prime01 = 2047m1l226
  echo "Running up benchmark OPT_RAD_$opt_rad - 2047m1l226 - keygen"
  BENCH_KEYGEN_PRIME01=bench_keygen_226_opt_rad_$opt_rad.out
  (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime01}.main -bkey ${TOTAL_EXPERIMENTS} > ${STATISTICS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME01})

  echo "Running up benchmark OPT_RAD_$opt_rad - 2047m1l226 - act"
  BENCH_ACT_PRIME01=bench_act_226_opt_rad_$opt_rad.out
  (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime01}.main -bact ${TOTAL_EXPERIMENTS} > ${STATISTICS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME01})



  # prime03 = 2047m6l194
  echo "Running up benchmark OPT_RAD_$opt_rad - 2047m6l194 - keygen"
  BENCH_KEYGEN_PRIME03=bench_keygen_194_opt_rad_$opt_rad.out
  (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime03}.main -bkey ${TOTAL_EXPERIMENTS} > ${STATISTICS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_KEYGEN_PRIME03})

  echo "Running up benchmark OPT_RAD_$opt_rad - 2047m6l194 - act"
  BENCH_ACT_PRIME03=bench_act_194_opt_rad_$opt_rad.out
  (./${BUILD_OPT_RAD_FOLDER}/main/ctidh-${prime03}.main -bact ${TOTAL_EXPERIMENTS} > ${STATISTICS_OUTPUT_OPT_RAD_FOLDER}/${BENCH_ACT_PRIME03})

}
done





# cleanupPKP

{ set +x; } 2>/dev/null

echo "Done!"