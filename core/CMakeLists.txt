add_library(YourSound SHARED)

set(YSBP_YS_ROOT ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")
include(YourSoundBinaryPlayer.cmake)

add_subdirectory(src)

target_compile_definitions(YourSound PRIVATE YS_IN_CORE_BUILD SOL_ALL_SAFETIES_ON)
target_compile_features(YourSound PUBLIC cxx_std_23)
target_include_directories(YourSound PUBLIC
	include
	../lib/dr_libs
	../lib/SArc/include
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LuaJIT REQUIRED IMPORTED_TARGET luajit)

message(STATUS "LuaJIT_FOUND = ${LuaJIT_FOUND}")

if(TARGET PkgConfig::LuaJIT)
	message(STATUS "PkgConfig::LuaJIT exists")
else()
	message(FATAL_ERROR "PkgConfig::LuaJIT does not exist")
endif()

target_link_libraries(YourSound PUBLIC
	dr_libs_interface
	nlohmann_json
	SArc
	sol2
	PkgConfig::LuaJIT
	PRIVATE
	ImGui_Core
)