add_library(YSKeyboardMappingHandler SHARED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LuaJIT REQUIRED IMPORTED_TARGET luajit)

message(STATUS "LuaJIT_FOUND = ${LuaJIT_FOUND}")

if(TARGET PkgConfig::LuaJIT)
	message(STATUS "PkgConfig::LuaJIT exists")
else()
	message(FATAL_ERROR "PkgConfig::LuaJIT does not exist")
endif()

set_target_properties(YSKeyboardMappingHandler PROPERTIES OUTPUT_NAME "YSKMH")
target_link_libraries(YSKeyboardMappingHandler PRIVATE
	rtmidi
	nlohmann_json
	PUBLIC
	sol2
	PkgConfig::LuaJIT
)

target_compile_features(YSKeyboardMappingHandler PUBLIC cxx_std_23)
target_compile_definitions(YSKeyboardMappingHandler PRIVATE YSKMH_LIBRARY_BUILD SOL_ALL_SAFETIES_ON)
target_include_directories(YSKeyboardMappingHandler PUBLIC include)
target_sources(YSKeyboardMappingHandler PRIVATE
	src/MIDIMapping.cpp
)

if(WIN32)
	add_target_resource(YSKeyboardMappingHandler "YourSound Keyboard Mapping Handler" "Required to use MIDI keyboards with YourSound applications.")
endif()