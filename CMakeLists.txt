cmake_minimum_required(VERSION 3.22)

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

option(YS_BUILD_VST3 "Build VST3 Plugin" ON)
option(YS_BUILD_AU "Build AU Plugin" ON)
option(YS_BUILD_WEB "Build Web Integration" OFF)

add_subdirectory(core)
add_subdirectory(plugin)

target_compile_features(YourSound PUBLIC cxx_std_23)