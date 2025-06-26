get_property(prime_targets DIRECTORY ${CMAKE_SOURCE_DIR}/src PROPERTY BUILDSYSTEM_TARGETS)

message(STATUS "Benchmarks targets: ${prime_targets}")

set(BENCH_SRC
        benchmarks_main.c
)

set(BENCH_HDR
        benchmarks_utils.h
)

#   Add benchmark names here, similar as above
set(BENCHMARK_SUITES
        isogeny_walks
)


#   Will create a benchmarks app for each prime.
#   Benchmarks have benchmarks/ prefix



foreach (lib IN LISTS prime_targets)
    set(BENCH_APP benchmarks-${lib})

    message(STATUS "Creating main target: ${BENCH_APP}")
    add_executable(${BENCH_APP} ${BENCH_SRC} ${BENCH_HDR})
    target_link_libraries(${BENCH_APP} LINK_PUBLIC ${lib})
    target_include_directories(${BENCH_APP} PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/asm ${CMAKE_SOURCE_DIR}/src/parameters)
    add_dependencies(${BENCH_APP} ${lib})

#    #Add benchmark suites for current target
    foreach (test ${BENCHMARK_SUITES})
        add_test(benchmarks-${lib}-${test} ${BENCH_APP} benchmarks/${test} --show-stderr)
    endforeach ()

endforeach ()
