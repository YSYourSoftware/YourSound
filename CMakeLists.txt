cmake_minimum_required(VERSION 3.15)

file(READ "core/include/YourSound/Version.hpp" YS_VERSION_FILE)
string(REGEX MATCH "#define[ ]+YS_VERSION_MAJOR[ ]+([0-9]+)" _ ${YS_VERSION_FILE})
set(YS_VERSION_MAJOR "${CMAKE_MATCH_1}")
string(REGEX MATCH "#define[ ]+YS_VERSION_MINOR[ ]+([0-9]+)" _ ${YS_VERSION_FILE})
set(YS_VERSION_MINOR "${CMAKE_MATCH_1}")
string(REGEX MATCH "#define[ ]+YS_VERSION_PATCH[ ]+([0-9]+)" _ ${YS_VERSION_FILE})
set(YS_VERSION_PATCH "${CMAKE_MATCH_1}")

set(YS_VERSION ${YS_VERSION_MAJOR}.${YS_VERSION_MINOR}.${YS_VERSION_PATCH})

project(YourSound
	VERSION ${YS_VERSION}
	DESCRIPTION "FOSS C++ Sound Engine"
	LANGUAGES CXX
)

if(MSVC)
	add_compile_options($<$<CONFIG:Debug>:/fsanitize=address>)
elseif(GCC OR CLANG)
	add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
	add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)
endif()

set(YS_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

function(add_target_resource TARGET_NAME PRODUCT_NAME FILE_DESCRIPTION)
	set(RC_PRODUCT_NAME "${PRODUCT_NAME}")
	set(RC_FILE_DESCRIPTION "${FILE_DESCRIPTION}")

	set(OUTPUT_RC "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_shared.rc")

	configure_file(
		"${YS_PROJECT_ROOT}/shared.rc.in"
		"${OUTPUT_RC}"
		@ONLY
	)

	target_sources(${TARGET_NAME} PRIVATE "${OUTPUT_RC}")
endfunction()

option(YS_BUILD_PLUGIN "Build DAW Plugins (CLAP, VST3 & AUv3)" OFF)
option(YS_BUILD_INSTALL "Build Application installer and Library Manager" OFF)
option(YS_BUILD_FACTORY "Build Factory Binary Players" OFF)
option(YS_BUILD_QUICKSAMPLE "Build YourSound QuickSample" OFF)

add_subdirectory(lib)

add_subdirectory(core)

if(YS_BUILD_PLUGIN OR YS_BUILD_INSTALL OR YS_BUILD_QUICKSAMPLE)
	add_subdirectory(kbd-mapping-handler)
endif()

if(YS_BUILD_PLUGIN)
	add_subdirectory(plugin)
endif()

if(YS_BUILD_INSTALL)
	add_subdirectory(libmgr)
	add_subdirectory(installer)
	add_subdirectory(libtest)
endif()

if(YS_BUILD_FACTORY)
	add_subdirectory(factory)
endif()

if(YS_BUILD_QUICKSAMPLE)
	add_subdirectory(quicksample)
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "/Od")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
endif()