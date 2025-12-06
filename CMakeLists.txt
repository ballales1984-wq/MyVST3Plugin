cmake_minimum_required(VERSION 3.15)
project(MyVST3Plugin VERSION 1.0.0)

# Add JUCE as a subdirectory
add_subdirectory("../../../../../../../JUCE-master/JUCE-master" "juce_build")

juce_add_plugin(MyVST3Plugin
    VERSION "1.0.0"
    BUNDLE_ID "com.ballales1984.myvst3plugin"
    COMPANY_NAME "ballales1984-wq"
    COMPANY_WEBSITE "https://github.com/ballales1984-wq/MyVST3Plugin"
    COMPANY_EMAIL "contact@yourdomain.com"
    PLUGIN_MANUFACTURER_CODE "BL84"
    PLUGIN_CODE "Mvst"
    FORMATS Standalone VST3
    PRODUCT_NAME "MyVST3Plugin"
    DESCRIPTION "Professional Dual Oscillator VST3 Synthesizer for Reaper"
    IS_SYNTH TRUE
    VST3_CATEGORIES "Instrument|Synthesizer"
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
