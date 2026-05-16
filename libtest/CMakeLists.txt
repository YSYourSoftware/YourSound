add_executable(YSLibTest)

target_compile_features(YSLibTest PRIVATE cxx_std_23)

target_sources(YSLibTest PRIVATE
	src/main.cpp
	src/MessageBox.cpp
)

target_include_directories(YSLibTest PRIVATE
	include
	../lib/SDL/include
)

target_link_libraries(YSLibTest PRIVATE
	ImGui_SDL3
	YourSound
)

add_dependencies(YSLibTest YourSound)
add_custom_command(TARGET YSLibTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	$<TARGET_RUNTIME_DLLS:YSLibTest>
	$<TARGET_FILE_DIR:YSLibTest>
	COMMAND_EXPAND_LISTS
)

add_custom_command(TARGET YSLibTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/assets
	$<TARGET_FILE_DIR:YSLibTest>
)