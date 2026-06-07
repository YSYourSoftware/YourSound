if(YS_SANITIZE_ADDRESS)

endif()

if(YS_BUILD_PLUGIN)
	add_subdirectory(JUCE)
	add_subdirectory(imgui_juce)
endif()

add_subdirectory(dr_libs)

add_subdirectory(json)
add_subdirectory(SArc)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

add_library(ImGui_SDL3 STATIC
	ImGui/imgui.cpp
	ImGui/imgui_draw.cpp
	ImGui/imgui_tables.cpp
	ImGui/imgui_widgets.cpp

	ImGui/backends/imgui_impl_sdl3.cpp
	ImGui/backends/imgui_impl_sdlrenderer3.cpp

	ImGui/misc/cpp/imgui_stdlib.cpp

	imgui-knobs/imgui-knobs.cpp
)

target_link_libraries(ImGui_SDL3 PUBLIC SDL3::SDL3)
target_compile_definitions(ImGui_SDL3 PUBLIC IMGUI_USER_CONFIG="CustomImConfig.h")
target_include_directories(ImGui_SDL3 PUBLIC
	ImGui
	ImGui/backends
	ImGui/misc/cpp
	ImGuiConfig
	imgui-knobs
	imgui_club/imgui_memory_editor
)

add_library(ImGui_Plugin STATIC
	ImGui/imgui.cpp
	ImGui/imgui_draw.cpp
	ImGui/imgui_tables.cpp
	ImGui/imgui_widgets.cpp

	ImGui/backends/imgui_impl_opengl3.cpp
	ImGui/backends/imgui_impl_null.cpp

	ImGui/misc/cpp/imgui_stdlib.cpp

	imgui-knobs/imgui-knobs.cpp
)

target_compile_definitions(ImGui_Plugin PUBLIC IMGUI_USER_CONFIG="CustomImConfig.h")
target_include_directories(ImGui_Plugin PUBLIC
	ImGui
	ImGui/backends
	ImGui/misc/cpp
	ImGuiConfig
	imgui-knobs
	imgui_club/imgui_memory_editor
)

add_library(ImGui_Core STATIC
	ImGui/imgui.cpp
	ImGui/imgui_draw.cpp
	ImGui/imgui_tables.cpp
	ImGui/imgui_widgets.cpp

	ImGui/misc/cpp/imgui_stdlib.cpp

	imgui-knobs/imgui-knobs.cpp
)

target_compile_definitions(ImGui_Core PUBLIC IMGUI_USER_CONFIG="CustomImConfig.h")
target_include_directories(ImGui_Core PUBLIC
	ImGui
	ImGui/misc/cpp
	ImGuiConfig
	imgui-knobs
	imgui_club/imgui_memory_editor
)