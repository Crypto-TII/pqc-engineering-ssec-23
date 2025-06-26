cmake_minimum_required(VERSION 3.13)

project(secsidh C ASM)

# Prepare variable to collect enabled parametersets
set(SECSIDH_PARAMETER_SETS_COMMON "")
set(SECSIDH_PARAMETER_SETS_CSIDH "")
set(SECSIDH_PARAMETER_SETS_CTIDH "")

# set(CMAKE_VERBOSE_MAKEFILE ON)

# define the options for this project
option(SECSIDH_GLOBAL_COUNTERS "Use global counters" ON)
option(SECSIDH_USE_OPENSSL_RAND "Use OpenSSL RNG" OFF)


option(SECSIDH_USE_GMP "Use the GMP implementation" ON)
option(SECSIDH_USE_GMP_SEC_FUNCTIONS "Use GMP low-level functions for cryptography" ON)
option(SECSIDH_USE_KARATSUBA "Use the Karatsuba multiplication" ON)
option(SECSIDH_USE_MONTGOMERY "Use Montgomery reduction" ON)
option(SECSIDH_USE_AVX2 "Use AVX2" OFF)

option(SECSIDH_USE_RANDOMNESS "Use the randomness-based implementation" OFF)
set(SECSIDH_CSIDH_VARIANT "df")

option(SECSIDH_CTIDH_TUNE_MULTS "Use version tuned for multiplications " ON)
option(SECSIDH_CTIDH_TUNE_CYCLES "Use version tuned for clock cycles " OFF)

option(ENABLE_CT_TESTING  "Enable compilation for constant time testing." OFF)

# convenience macro define all parameter sets
macro(SECSIDH_DEFINE_PARAMETERS bits default)
    option(SECSIDH_ENABLE_CTIDH${bits} "Enable CTIDH${bits} parameterset." ${default})
    if(SECSIDH_ENABLE_CTIDH${bits})
        set(SECSIDH_PARAMETER_SETS_CTIDH ${SECSIDH_PARAMETER_SETS_CTIDH} ${bits})
    endif()
endmacro()
SECSIDH_DEFINE_PARAMETERS(2047m1l226 ON)
SECSIDH_DEFINE_PARAMETERS(2047m4l205 ON)
SECSIDH_DEFINE_PARAMETERS(2047m6l194 ON)
SECSIDH_DEFINE_PARAMETERS(2047m7l188 OFF)



# handle counter setting
if(SECSIDH_GLOBAL_COUNTERS)
    add_definitions(-DGLOBAL_COUNTERS=1)
endif()

# Set project code options
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in compiler flags
include(.CMake/compiler_options.cmake)

# Set up headers
configure_file(src/secsidhconfig.h.cmake
               ${PROJECT_BINARY_DIR}/include/secsidh/secsidhconfig.h)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/secsidh.h ${CMAKE_BINARY_DIR}/include/secsidh/secsidh.h)
include_directories(${PROJECT_BINARY_DIR}/include)

# Set up amount of radical 3-isogenies
set(ARG_RAD)
if (OPT_RAD MATCHES 1)
    message(STATUS "Using ONE radical 3-isogenies")
    set(ARG_RAD -D_M1_)
elseif (OPT_RAD MATCHES 2)
    message(STATUS "Using TWO radical 3-isogenies")
    set(ARG_RAD -D_M2_)
elseif (OPT_RAD MATCHES 3)
    message(STATUS "Using FOUR radical 3-isogenies")
    set(ARG_RAD -D_M3_)
elseif (OPT_RAD MATCHES 4)
    message(STATUS "Using EIGHT radical 3-isogenies")
    set(ARG_RAD -D_M4_)
elseif (OPT_RAD MATCHES 5)
    message(STATUS "Using SIXTEEN radical 3-isogenies")
    set(ARG_RAD -D_M5_)
else()
    # By default we assume original dCTIDH approach
    message(STATUS "NOT Using radical 3-isogenies (original dCTIDH)")
    set(ARG_RAD -D_M0_)
endif()

# Set subdirectory for sources
add_subdirectory(src)

# Set up library
foreach(bits IN ITEMS ${SECSIDH_PARAMETER_SETS_CTIDH})
    if(SECSIDH_ENABLE_CTIDH${bits})
        set(SOURCES ${SOURCES}
            $<TARGET_OBJECTS:secsidh-ctidh${bits}>
            $<TARGET_OBJECTS:csidh-ctidh${bits}>
            $<TARGET_OBJECTS:ctidh-${bits}>
            $<TARGET_OBJECTS:uint-${bits}>
            $<TARGET_OBJECTS:primes-ctidh${bits}>
            $<TARGET_OBJECTS:fp-${bits}>
            $<TARGET_OBJECTS:poly-ctidh${bits}>
            $<TARGET_OBJECTS:mont-ctidh${bits}>
            $<TARGET_OBJECTS:elligator-ctidh${bits}>
            $<TARGET_OBJECTS:skgen-ctidh${bits}>
            $<TARGET_OBJECTS:validate-ctidh${bits}>
            $<TARGET_OBJECTS:csidh-ctidh${bits}>
            $<TARGET_OBJECTS:steps-ctidh${bits}>
            $<TARGET_OBJECTS:random-ctidh${bits}>
            $<TARGET_OBJECTS:steps_t-ctidh${bits}>
            $<TARGET_OBJECTS:int32_sort>
            $<TARGET_OBJECTS:fips202>
            $<TARGET_OBJECTS:keccakf1600>
        )
        if(SECSIDH_USE_KARATSUBA)
            set(SOURCES ${SOURCES} $<TARGET_OBJECTS:karatsuba_fp_sqr>)
        endif()
    endif()
endforeach()


add_library(secsidh STATIC ${SOURCES})
if(SECSIDH_USE_GMP)
    target_link_libraries(secsidh PUBLIC gmp rng)
else()
   target_link_libraries(secsidh PUBLIC rng)
endif()



# Main executables (XXX: rename to examples)
add_subdirectory(main)

set_target_properties(secsidh
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    VERSION "0.1"
    SOVERSION 0
)

install(TARGETS secsidh
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(FILES
        ${PROJECT_SOURCE_DIR}/src/secsidh.h
        ${PROJECT_BINARY_DIR}/include/secsidh/secsidhconfig.h
        DESTINATION include/secsidh
)
