add_yoursound_binary_player(binplayers VERSION 1.0.0)

set_target_properties(binplayers PROPERTIES OUTPUT_NAME binplayers-x86-64)
target_include_directories(binplayers PRIVATE include)

target_sources(binplayers PUBLIC
	src/BinPlayer.cpp
	src/BinPlayer2A03.cpp
	src/BinPlayerHexwave.cpp
	src/BinPlayerSampler.cpp
	src/BinPlayerWavetable.cpp
)

if(WIN32)
	add_target_resource(binplayers "YourSound Factory Library Binary Players" "Binary players for the YSFL.")
endif()