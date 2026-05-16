add_yoursound_binary_player(binplayers VERSION 1.0.0)

set_target_properties(binplayers PROPERTIES OUTPUT_NAME binplayers-x86-64)
target_include_directories(binplayers PRIVATE
	include

	lib/TinySoundFont
)

target_sources(binplayers PUBLIC
	src/BinPlayer.cpp
	src/BinPlayerHexwave.cpp
	src/BinPlayerSampler.cpp
	src/BinPlayerSF2.cpp
)