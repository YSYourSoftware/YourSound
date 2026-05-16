add_executable(YSQuickSample WIN32 MACOSX_BUNDLE)

target_compile_features(YSQuickSample PRIVATE cxx_std_23)

add_subdirectory(src)

target_include_directories(YSQuickSample PRIVATE include)

target_link_libraries(YSQuickSample PRIVATE
	ImGui_SDL3
	YourSound
)

if(WIN32)
	target_sources(YSQuickSample PRIVATE app.rc)
	target_link_options(YSQuickSample PRIVATE
		"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'"
	)
endif()

if(APPLE)
	set(MACOSX_BUNDLE_ICON_FILE icon.icns)
	set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	target_sources(YSQuickSample PRIVATE icon.icns)
endif()

add_dependencies(YSQuickSample YourSound)
add_custom_command(TARGET YSQuickSample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	$<TARGET_RUNTIME_DLLS:YSQuickSample>
	$<TARGET_FILE_DIR:YSQuickSample>
	COMMAND_EXPAND_LISTS
)

add_custom_command(TARGET YSQuickSample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/assets
	$<TARGET_FILE_DIR:YSQuickSample>
)