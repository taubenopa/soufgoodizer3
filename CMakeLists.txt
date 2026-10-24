cmake_minimum_required(VERSION 3.22)

# Statische MSVC-Runtime: Nutzer brauchen kein VC++ Redistributable
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Soufgoodizer VERSION 1.0.0 LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Soufgoodizer
    COMPANY_NAME "Souf"
    PLUGIN_MANUFACTURER_CODE Souf
    PLUGIN_CODE Sgdz
    FORMATS VST3
    PRODUCT_NAME "Soufgoodizer"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    VST3_CATEGORIES Fx Dynamics
    COPY_PLUGIN_AFTER_BUILD FALSE)

# Das Kopf-Bild wird fest in die Plugin-DLL eingebettet
juce_add_binary_data(SoufAssets SOURCES head.png)

target_sources(Soufgoodizer PRIVATE
    PluginProcessor.cpp
    PluginEditor.cpp)

target_compile_definitions(Soufgoodizer PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(Soufgoodizer
    PRIVATE
        SoufAssets
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
