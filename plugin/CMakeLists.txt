add_subdirectory(lib/JUCE)

juce_add_plugin(YourSoundPlugin
	COMPANY_NAME "YourSoftware"
	PRODUCT_NAME "YourSound Plug-In"
	PLUGIN_MANUFACTURER_CODE YSft
	PLUGIN_CODE Ysnp
	FORMATS AU VST3 Standalone
	IS_SYNTH TRUE
	NEEDS_MIDI_INPUT TRUE
	ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon-large.png"
	ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon-small.png"
	DOCUMENT_EXTENSIONS yslib
)

target_link_libraries(YourSoundPlugin
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

add_subdirectory(lib)