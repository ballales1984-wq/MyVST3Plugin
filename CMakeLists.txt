cmake_minimum_required(VERSION 3.22)

project(MyVST3Plugin VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(MyVST3Plugin
    COMPANY_NAME "MyVST3"
    PLUGIN_MANUFACTURER_CODE Myv3
    PLUGIN_CODE Mv3p
    FORMATS VST3 Standalone
    PRODUCT_NAME "MyVST3Plugin"
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(MyVST3Plugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SynthVoice.h
    Source/SynthLookAndFeel.cpp
)

juce_generate_juce_header(MyVST3Plugin)

target_compile_definitions(MyVST3Plugin PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MyVST3Plugin PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

if(MSVC)
    target_compile_options(MyVST3Plugin PRIVATE /W4)
else()
    target_compile_options(MyVST3Plugin PRIVATE -Wall -Wextra -Wpedantic)
endif()
