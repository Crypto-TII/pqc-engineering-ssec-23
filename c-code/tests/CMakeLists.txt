get_property(prime_targets DIRECTORY ${CMAKE_SOURCE_DIR}/src PROPERTY BUILDSYSTEM_TARGETS)

message(STATUS "Test targets: ${prime_targets}")

set(TEST_SRC
        test_main.c
        munit.c
        test_fp.c
        test_fq.c
        test_isogeny_walks.c
)

#   Add test case names here.
#   These are the ones used in the DECLARE_TEST_CASE macro.

set(TEST_SUITES
        fp
        fq
        isogeny_walks
)



#   Will create a test app for each prime. 
#   Unit tests have unit/ prefix
#   Benchmarks have bench/ prefix

foreach (lib IN LISTS prime_targets)
    set(TEST_APP tests-${lib})

    message(STATUS "Creating test target: ${TEST_APP}")
    add_executable(${TEST_APP} ${TEST_SRC})
    target_link_libraries(${TEST_APP} LINK_PUBLIC ${lib})
    target_include_directories(${TEST_APP} PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/asm ${CMAKE_SOURCE_DIR}/src/parameters)
    target_compile_options(${TEST_APP} PRIVATE "$<$<CONFIG:DEBUG>:$<IF:$<PLATFORM_ID:Windows>,,-DMUNIT_NO_FORK>>") #Disable forking for debug and NON-Windows compilation
    target_compile_options(${TEST_APP} PRIVATE -DMUNIT_ENABLE_ASSERT_ALIASES -DMUNIT_FAIL_NO_TEST_RUN -DMUNIT_NO_NL_LANGINFO) #Disable forking for debug compilation
    add_dependencies(${TEST_APP} ${lib})

    #Add test suites for current target
    foreach (test ${TEST_SUITES})
        add_test(test-${lib}-${test} ${TEST_APP} tests/${test})
    endforeach ()

endforeach ()


# Add custom target for running all tests in parallel

add_custom_target(
        test-parallel
        COMMAND ${CMAKE_CTEST_COMMAND} -j 8 -C $<CONFIGURATION> --force-new-ctest-process
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

