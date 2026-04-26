#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class NexusImagerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    NexusImagerAudioProcessorEditor (NexusImagerAudioProcessor&);
    ~NexusImagerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    NexusImagerAudioProcessor& audioProcessor;

    // UI Components para las 4 bandas
    struct BandUI {
        juce::Slider widthSlider;
        juce::Slider widenSlider;
        juce::ToggleButton soloButton;
        juce::ToggleButton muteButton;
        juce::ComboBox modeSelector;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widenAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> soloAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;
    };
    BandUI bands[4];

    // Vectorscope Placeholder (Implementar con OpenGL para rendimiento Pro)
    void drawVectorscope(juce::Graphics& g, juce::Rectangle<int> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NexusImagerAudioProcessorEditor)
};
