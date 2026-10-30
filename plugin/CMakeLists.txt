add_library(YourSoundCLAP MODULE
	src/YourSoundCLAP.cpp
)

target_include_directories(YourSoundCLAP PRIVATE include)
target_link_libraries(YourSoundCLAP PRIVATE clap clap-helpers imgui-clap-support YourSound)

set_target_properties(YourSoundCLAP PROPERTIES
	PREFIX ""
	SUFFIX ".clap"
)

if(WIN32)
	add_target_resource(YourSoundCLAP "YourSound" "YourSound CLAP Plugin")
endif()