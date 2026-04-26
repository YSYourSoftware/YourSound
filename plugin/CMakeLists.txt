set(JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING ON)
set(JUCE_BUILD_VST OFF)

juce_add_plugin(YourSoundPlugin
	COMPANY_NAME "YourSoftware"
	PRODUCT_NAME "YourSound Plug-In"
	PLUGIN_MANUFACTURER_CODE YSft
	PLUGIN_CODE Ysnp
	FORMATS AU VST3 Standalone
	IS_SYNTH TRUE
	NEEDS_MIDI_INPUT TRUE
	ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/../assets/icon-large.png"
	ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/../assets/icon-small.png"
)

juce_generate_juce_header(YourSoundPlugin)

target_compile_definitions(YourSoundPlugin
	PUBLIC
		JUCE_WEB_BROWSER=0
		JUCE_USE_CURL=0
		JUCE_REPORT_APP_USAGE=0
		JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:YourSoundPlugin,JUCE_PRODUCT_NAME>"
		JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:YourSoundPlugin,JUCE_VERSION>"
		JUCE_FORCE_USE_LEGACY_PARAM_IDS=1
		JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(YourSoundPlugin
    PRIVATE
        juce::juce_audio_utils
		ImGui
		imgui_impl_juce
		YourSound
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

add_subdirectory(src)