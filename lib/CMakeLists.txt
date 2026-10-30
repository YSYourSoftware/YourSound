set(RTMIDI_BUILD_STATIC_LIBS ON)
set(RTMIDI_BUILD_TESTING OFF)

add_subdirectory(CLI11 EXCLUDE_FROM_ALL)
add_subdirectory(json EXCLUDE_FROM_ALL)
add_subdirectory(rtmidi EXCLUDE_FROM_ALL)
add_subdirectory(SArc EXCLUDE_FROM_ALL)
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(sol2 EXCLUDE_FROM_ALL)

add_library(ImGui_SDL3 STATIC
	ImGui/imgui.cpp
	ImGui/imgui_draw.cpp
	ImGui/imgui_tables.cpp
	ImGui/imgui_widgets.cpp

	ImGui/backends/imgui_impl_sdl3.cpp
	ImGui/backends/imgui_impl_sdlrenderer3.cpp #use dx12|opengl3|metal instead

	ImGui/misc/debuggers/imgui.natstepfilter
	ImGui/misc/debuggers/imgui.natvis
	ImGui/misc/cpp/imgui_stdlib.cpp

	imgui-knobs/imgui-knobs.cpp
)

target_link_libraries(ImGui_SDL3 PUBLIC SDL3-shared)
target_compile_definitions(ImGui_SDL3 PUBLIC IMGUI_USER_CONFIG="CustomImConfig.h")
target_include_directories(ImGui_SDL3 PUBLIC
	ImGui
	ImGui/backends
	ImGui/misc/cpp
	ImGuiConfig
	imgui-knobs
	imgui_club/imgui_memory_editor
)

if(YS_BUILD_PLUGIN)
	add_subdirectory(clap EXCLUDE_FROM_ALL)
	add_subdirectory(clap-helpers)

	set(IMGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ImGui")
	set(SDL_LIBRARY SDL3-shared)
	add_subdirectory(clap-imgui-support)

	target_sources(imgui-clap-support PRIVATE
		ImGui/misc/debuggers/imgui.natstepfilter
		ImGui/misc/debuggers/imgui.natvis
		ImGui/misc/cpp/imgui_stdlib.cpp

		imgui-knobs/imgui-knobs.cpp
	)

	target_compile_definitions(imgui-clap-support PUBLIC IMGUI_USER_CONFIG="CustomImConfig.h")
	target_include_directories(imgui-clap-support PUBLIC
		ImGui
		ImGui/backends
		ImGui/misc/cpp
		ImGuiConfig
		imgui-knobs
		imgui_club/imgui_memory_editor
	)
endif()

add_library(ImGui_Core STATIC
	ImGui/imgui.cpp
	ImGui/imgui_draw.cpp
	ImGui/imgui_tables.cpp
	ImGui/imgui_widgets.cpp

	ImGui/misc/debuggers/imgui.natstepfilter
	ImGui/misc/debuggers/imgui.natvis
	ImGui/misc/cpp/imgui_stdlib.cpp

	imgui-knobs/imgui-knobs.cpp

	ImGuiColorTextEdit/TextEditor.cpp
)

target_compile_definitions(ImGui_Core PUBLIC IMGUI_USER_CONFIG="CustomImConfig.h")
target_include_directories(ImGui_Core PUBLIC
	ImGui
	ImGui/misc/cpp
	ImGuiColorTextEdit
	ImGuiConfig
	imgui-knobs
	imgui_club/imgui_memory_editor
)

target_compile_features(ImGui_Core PUBLIC cxx_std_23)