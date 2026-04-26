cmake_minimum_required(VERSION 3.15)

project(NexusImager VERSION 1.0.0)

# Cargar JUCE
# Nota: En GitHub Actions, JUCE debe bajarse como submódulo o vía FetchContent
set(JUCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/JUCE CACHE PATH "Path to JUCE")
add_subdirectory(${JUCE_DIR} _juce)

juce_add_plugin(NexusImager
    PLUGIN_CODE "NEXI"
    PRODUCT_NAME "Nexus Imager"
    COMPANY_NAME "NexusAudio"
    BUNDLE_ID "com.nexusaudio.nexusimager"
    FORMATS VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES "Fx|Spatial"
)

target_sources(NexusImager PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/DSP/MultibandCrossover.h
    Source/DSP/StereoEngine.h
)

target_compile_definitions(NexusImager PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(NexusImager PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
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

juce_generate_juce_header(NexusImager)
