if(NOT PROJECT_IS_TOP_LEVEL)
    set(EXTERNAL_PROJECT ON)
else()
    set(EXTERNAL_PROJECT OFF)
endif()
if(NOT EXTERNAL_PROJECT)
	#Setup CMake to run tests
   	option(BUILD_UNITTESTS "Create unittests target in makefile with allow to run unittests put into tests directory")
   	if(BUILD_TESTS)
		enable_testing()
		
		find_package(Python3 REQUIRED COMPONENTS Interpreter)
		
		set(VISUAL_TEST_RUNNER "${CMAKE_SOURCE_DIR}/run_functionnal_test.py")
		
		function(add_visual_test TEST_NAME INPUT_SCRIPT EXPECTED_SVG)
		    add_test(
		        NAME "${TEST_NAME}"
		        COMMAND "${Python3_EXECUTABLE}" "${VISUAL_TEST_RUNNER}"
			"$<TARGET_FILE:CairoInterpreter>"
		                "${INPUT_SCRIPT}"
		                "${EXPECTED_SVG}"
		                --diff-dir "${CMAKE_BINARY_DIR}/test_diffs"
		    )
		endfunction()
		file(GLOB FUNCTEST_SRCS ${CMAKE_SOURCE_DIR}/functests/test_*.cairo)
		foreach(functest ${FUNCTEST_SRCS})
			get_filename_component(testName ${functest} NAME_WE)

		
			add_visual_test(
				${testName}
				"${functest}"
				"${CMAKE_SOURCE_DIR}/functests/${testName}.svg"
			)
		endforeach()
	endif()
endif()
