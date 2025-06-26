set(HDR
    rng.h
    utilities.h
    parameters.h
    fp.h
    fp2.h
    isogeny_walks.h
    cgl_hash.h
    )


set(SRC
        rng.c
        utilities.c
        fp.c
        fp2.c
        isogeny_walks_2.c
        isogeny_walks_3.c
        isogeny_walks_3_fp.c
        cgl_hash.c
    )

# Create a library target for each prime defined in parent PRIMES variable

foreach (P IN LISTS PRIMES)
    message(STATUS "Creating library target ${CMAKE_PROJECT_NAME}-p${P}")
    set(SRCP)
    if (OPT_AVX MATCHES YES)
        MESSAGE(STATUS "Using AVX2 implementation")
        set(SRCP asm/p${P}-avx.c)
    else()
        set(SRCP asm/p${P}.s)
    endif()
    add_library(${PROJECT_NAME}-p${P} STATIC ${SRC} ${SRCP})
    target_compile_options(${PROJECT_NAME}-p${P} PUBLIC -D_P${P}_ ${COMP_ASM})
    target_compile_options(${PROJECT_NAME}-p${P} PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
    target_compile_options(${PROJECT_NAME}-p${P} PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
endforeach ()