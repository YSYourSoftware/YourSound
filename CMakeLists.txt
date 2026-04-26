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

option(YS_BUILD_PLUGIN "Build Plugin" OFF)
option(YS_BUILD_INSTALL "Build Application installer and Library Manager" OFF)
option(YS_BUILD_FACTORY "Build Factory Binary Players" OFF)

add_subdirectory(lib)

add_subdirectory(core)

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

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "/Od")
	set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
endif()