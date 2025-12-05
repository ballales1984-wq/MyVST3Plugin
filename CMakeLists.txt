cmake_minimum_required(VERSION 3.15)
project(MyVST3Plugin VERSION 1.0.0)

# Add JUCE as a subdirectory
add_subdirectory("../../../../.." "juce_build")

juce_add_plugin(MyVST3Plugin
    VERSION "1.0.0"
    BUNDLE_ID "com.example.myvst3plugin"
    COMPANY_NAME "Example Company"
    COMPANY_WEBSITE "https://example.com"
    COMPANY_EMAIL "info@example.com"
    PLUGIN_MANUFACTURER_CODE "ExCo"
    PLUGIN_CODE "Mvst"
    FORMATS Standalone
    PRODUCT_NAME "MyVST3Plugin"
    DESCRIPTION "A simple dual oscillator VST3 synthesizer"
    IS_SYNTH TRUE
)

# Add source files
target_sources(MyVST3Plugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Set up target properties
target_compile_definitions(MyVST3Plugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE modules
# Generate JUCE header for easier development
juce_generate_juce_header(MyVST3Plugin)

target_link_libraries(MyVST3Plugin PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Ensure MidiMessageCollector is available
target_compile_definitions(MyVST3Plugin PRIVATE JUCE_USE_WINRT_MIDI=0)

# Plugin will be manually copied after build
# Enable plugin installation to VST3 directory
# if(WIN32)
#     add_custom_command(TARGET MyVST3Plugin POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#         $<TARGET_FILE_DIR:MyVST3Plugin>/../VST3/MyVST3Plugin.vst3
#         "C:/Program Files/Common Files/VST3/MyVST3Plugin.vst3"
#         COMMENT "Copying plugin to VST3 directory"
#     )
# endif()
