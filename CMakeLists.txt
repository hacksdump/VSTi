cmake_minimum_required(VERSION 3.15)

project(SineSynth VERSION 0.0.1)

set(JUCE_DIR ../JUCE)  # adjust path if needed
add_subdirectory(${JUCE_DIR} JUCE)

juce_add_plugin(SineSynth
    COMPANY_NAME "Harshit"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    FORMATS VST3
    PRODUCT_NAME "SineSynth"
    PLUGIN_MANUFACTURER_CODE Hrst
    PLUGIN_CODE Snsy
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(SineSynth PRIVATE
    PluginProcessor.cpp
    PluginProcessor.h
    PluginEditor.cpp
    PluginEditor.h
)

target_link_libraries(SineSynth PRIVATE
    juce::juce_audio_utils
)


juce_generate_juce_header(SineSynth)

target_compile_definitions(SineSynth PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

