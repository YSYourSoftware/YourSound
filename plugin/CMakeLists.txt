add_library(YourSoundCLAP MODULE
	src/PluginMain.cpp
)

target_link_libraries(YourSoundCLAP PRIVATE clap clap-helpers YourSound)

set_target_properties(YourSoundCLAP PROPERTIES
	PREFIX ""
	SUFFIX ".clap"
)