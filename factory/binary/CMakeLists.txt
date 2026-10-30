add_yoursound_binary_player(binplayers VERSION 1.0.0)
add_yoursound_binary_player(bp_sampler VERSION 1.0.0)

set_target_properties(binplayers PROPERTIES OUTPUT_NAME binplayers-x86-64)
set_target_properties(bp_sampler PROPERTIES OUTPUT_NAME binsampler-x86-64)

target_include_directories(binplayers PRIVATE include)
target_include_directories(bp_sampler PRIVATE include lib/dr_libs)

target_sources(binplayers PRIVATE
	src/BinPlayer.cpp
	src/BinPlayer2A03.cpp
	src/BinPlayerDrumController.cpp
	src/BinPlayerHexwave.cpp
	src/BinPlayerWavetable.cpp
)

target_sources(bp_sampler PRIVATE
	src/BinPlayerSampler.cpp
)

if(WIN32)
	add_target_resource(binplayers "YourSound Factory Library" "YSFL Binary Players")
	add_target_resource(bp_sampler "YourSound Factory Library" "YSFL Sample Binary Player")
endif()