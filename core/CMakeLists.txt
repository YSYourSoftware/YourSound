add_library(YourSound SHARED)

set(YSBP_YS_ROOT ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")
include(YourSoundBinaryPlayer.cmake)

add_subdirectory(src)

target_compile_definitions(YourSound PRIVATE YS_IN_CORE_BUILD)
target_compile_features(YourSound PUBLIC cxx_std_23)
target_include_directories(YourSound PUBLIC include ../lib/SArc/include)
target_link_libraries(YourSound PRIVATE
	nlohmann_json
	ImGui_Player
	PUBLIC
	SArc
)