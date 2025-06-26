
macro(SECSIDH_CSIDH_EXE bits)
	if(SECSIDH_ENABLE_CSIDH${bits})
		add_executable(csidh-${bits}.main csidh.c)
		if(CMAKE_C_COMPILER_ID MATCHES "Clang")
			target_compile_options(csidh-${bits}.main PUBLIC -Wno-language-extension-token)
		endif()
		set_target_properties(csidh-${bits}.main PROPERTIES C_EXTENSIONS ON)
		target_include_directories(
			csidh-${bits}.main
			PUBLIC
			${CMAKE_SOURCE_DIR}/src/CSIDH/.
            ${CMAKE_SOURCE_DIR}/src/common
            ${CMAKE_SOURCE_DIR}/src/common/fp
		)
		target_link_libraries(csidh-${bits}.main PUBLIC secsidh)
        target_compile_definitions(csidh-${bits}.main PRIVATE BITS=${bits} NAMESPACE_NAME=${bits})
        target_compile_definitions(csidh-${bits}.main PRIVATE "P${bits}" "CSIDH")
        if(SECSIDH_USE_GMP)
            target_compile_definitions(csidh-${bits}.main PRIVATE "GMP")
        endif()

		add_test(NAME csidh-${bits} COMMAND csidh-${bits}.main)
	endif()
endmacro()

macro(SECSIDH_CTIDH_EXE bits)
	if(SECSIDH_ENABLE_CTIDH${bits})
		add_executable(ctidh-${bits}.main ctidh.c)
		if(CMAKE_C_COMPILER_ID MATCHES "Clang")
			target_compile_options(ctidh-${bits}.main PUBLIC -Wno-language-extension-token)
		endif()
		set_target_properties(ctidh-${bits}.main PROPERTIES C_EXTENSIONS ON)
		target_include_directories(
			ctidh-${bits}.main
			PUBLIC
			${CMAKE_SOURCE_DIR}/src/CTIDH/.
            # ${CMAKE_SOURCE_DIR}/src/CSIDH/csidh-${bits}
            ${CMAKE_SOURCE_DIR}/src/common
            ${CMAKE_SOURCE_DIR}/src/common/fp
		)
		target_link_libraries(ctidh-${bits}.main PUBLIC secsidh)
        target_compile_definitions(ctidh-${bits}.main PRIVATE BITS=${bits} NAMESPACE_NAME=${bits})
        target_compile_definitions(ctidh-${bits}.main PRIVATE "P${bits}" "CTIDH")
        if(SECSIDH_USE_KARATSUBA)
            target_compile_definitions(ctidh-${bits}.main PRIVATE "KARATSUBA")
        endif()          
        if(SECSIDH_USE_GMP)
            target_compile_definitions(ctidh-${bits}.main PRIVATE "GMP")
        endif()
		target_compile_options(ctidh-${bits}.main PUBLIC ${ARG_RAD})
      

		add_test(NAME ctidh-${bits} COMMAND ctidh-${bits}.main)
	endif()
endmacro()



macro(SECSIDH_CHECKCT_EXE bits)
	if(SECSIDH_ENABLE_CTIDH${bits})
		add_executable(checkct-${bits}.main checkct.c)
		if(CMAKE_C_COMPILER_ID MATCHES "Clang")
			target_compile_options(checkct-${bits}.main PUBLIC -Wno-language-extension-token)
		endif()
        if (ENABLE_CT_TESTING)
            target_compile_definitions(checkct-${bits}.main PUBLIC ENABLE_CT_TESTING)
        endif()
		set_target_properties(checkct-${bits}.main PROPERTIES C_EXTENSIONS ON)
		target_include_directories(
			checkct-${bits}.main
			PUBLIC
			${CMAKE_SOURCE_DIR}/src/CTIDH/.
            # ${CMAKE_SOURCE_DIR}/src/CSIDH/csidh-${bits}
            ${CMAKE_SOURCE_DIR}/src/common
            ${CMAKE_SOURCE_DIR}/src/common/fp
		)
		target_link_libraries(checkct-${bits}.main PUBLIC secsidh)
        target_compile_definitions(checkct-${bits}.main PRIVATE BITS=${bits} NAMESPACE_NAME=${bits})
        target_compile_definitions(checkct-${bits}.main PRIVATE "P${bits}" "CTIDH")
        if(SECSIDH_USE_KARATSUBA)
            target_compile_definitions(checkct-${bits}.main PRIVATE "KARATSUBA")
        endif()          
        if(SECSIDH_USE_GMP)
            target_compile_definitions(checkct-${bits}.main PRIVATE "GMP")
        endif()


		add_test(NAME checkct-${bits} COMMAND checkct-${bits}.main)
	endif()
endmacro()

macro(SECSIDH_BENCH_EXE bits)
	if(SECSIDH_ENABLE_CSIDH${bits})
		add_executable(bench-csidh-${bits}.main bench.c)
		target_compile_options(bench-csidh-${bits}.main PUBLIC -Wno-language-extension-token)
		set_target_properties(bench-csidh-${bits}.main PROPERTIES C_EXTENSIONS ON)
		target_include_directories(
			bench-csidh-${bits}.main
			PUBLIC
			${CMAKE_SOURCE_DIR}/src/CSIDH
            ${CMAKE_SOURCE_DIR}/src/common/fp
			${CMAKE_SOURCE_DIR}/src/common
		)
		target_link_libraries(bench-csidh-${bits}.main PUBLIC secsidh)
        target_compile_definitions(bench-csidh-${bits}.main PRIVATE BITS=${bits} NAMESPACE_NAME=${bits})
        target_compile_definitions(bench-csidh-${bits}.main PRIVATE "P${bits}" "CSIDH")
        if(SECSIDH_USE_KARATSUBA)
            target_compile_definitions(bench-csidh-${bits}.main PRIVATE "KARATSUBA")
        endif()      
        if(SECSIDH_USE_GMP)
            target_compile_definitions(bench-csidh-${bits}.main PRIVATE "GMP")
        endif()

		target_link_libraries(bench-csidh-${bits}.main PUBLIC secsidh-csidh${bits} m)
	endif()
endmacro()


foreach(bits IN ITEMS ${SECSIDH_PARAMETER_SETS_CTIDH})
	SECSIDH_CTIDH_EXE(${bits})

    if (ENABLE_CT_TESTING)
        SECSIDH_CHECKCT_EXE(${bits})
    endif()
endforeach()


