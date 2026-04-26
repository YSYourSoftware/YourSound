function(add_yoursound_binary_player TARGET_NAME)
	set(options)
	set(oneValueArgs VERSION)
	set(multiValueArgs)

	cmake_parse_arguments(YSP_${TARGET_NAME}
		"${options}"
		"${oneValueArgs}"
		"${multiValueArgs}"
		${ARGN}
	)

	add_library(${TARGET_NAME} SHARED)

	set_target_properties(${TARGET_NAME} PROPERTIES
		VERSION ${YSP_${TARGET_NAME}_VERSION}
		SOVERSION ${YSP_${TARGET_NAME}_VERSION}
	)

	target_include_directories(${TARGET_NAME} PRIVATE ${YSBP_YS_ROOT}/include)
	target_compile_definitions(${TARGET_NAME} PRIVATE YS_IN_BIN_PLAYER_BUILD)
	target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
	target_link_libraries(${TARGET_NAME} PRIVATE YourSound ImGui_Player)

	message(STATUS "Added YourSound binary player: ${TARGET_NAME}")
endfunction()