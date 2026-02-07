option(YS_BUILD_SHARED "Build YourSound as a shared library" OFF)

if(YS_BUILD_SHARED)
	add_library(YourSound SHARED)
else()
	add_library(YourSound STATIC)
endif()

add_subdirectory(src)
add_subdirectory(lib)

target_include_directories(YourSound 
	PUBLIC include
	PRIVATE src
)