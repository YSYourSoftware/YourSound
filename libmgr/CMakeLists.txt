add_executable(YSLibMgr WIN32 MACOSX_BUNDLE)

target_compile_features(YSLibMgr PRIVATE cxx_std_23)

add_subdirectory(src)

target_include_directories(YSLibMgr PRIVATE
	include
	../lib/SDL/include
	../lib/SArc/include
)

target_link_libraries(YSLibMgr PRIVATE
	ImGui_SDL3
	YourSound
	SArc
	nlohmann_json
)

target_compile_definitions(YSLibMgr PRIVATE $<$<CONFIG:Debug>:YSL_DEBUG>)

if(WIN32)
	target_sources(YSLibMgr PRIVATE app.rc)
	target_link_options(YSLibMgr PRIVATE
		"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'"
	)
endif()

if(APPLE)
	set(MACOSX_BUNDLE_ICON_FILE icon.icns)
	set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	target_sources(YSLibMgr PRIVATE icon.icns)
endif()

add_dependencies(YSLibMgr YourSound)
add_custom_command(TARGET YSLibMgr POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	$<TARGET_RUNTIME_DLLS:YSLibMgr>
	$<TARGET_FILE_DIR:YSLibMgr>
	COMMAND_EXPAND_LISTS
)