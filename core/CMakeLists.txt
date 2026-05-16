add_library(YourSound SHARED)

set(YSBP_YS_ROOT ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")
include(YourSoundBinaryPlayer.cmake)

add_subdirectory(src)

target_compile_definitions(YourSound PRIVATE YS_IN_CORE_BUILD)
target_compile_features(YourSound PUBLIC cxx_std_23)
target_include_directories(YourSound PUBLIC
	include
	../lib/SArc/include
	../lib/dr_libs
)

target_link_libraries(YourSound PRIVATE
	dr_libs_interface
	nlohmann_json
	ImGui_Player
PUBLIC
	SArc
)

if(YS_SANITIZE_ADDRESS)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		message(STATUS "Enabling AddressSanitizer")
		target_compile_options(YourSound PRIVATE -fsanitize=address)
		target_link_options(YourSound PRIVATE -fsanitize=address)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		message(STATUS "Enabling AddressSanitizer (MSVC)")
		target_compile_options(YourSound PRIVATE /fsanitize=address)
	else()
		message(WARNING "YS_SANITIZE_ADDRESS is enabled but the compiler does not support it. Ignoring.")
	endif()
endif()