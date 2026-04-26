#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class NexusImagerAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    NexusImagerAudioProcessorEditor (NexusImagerAudioProcessor&);
    ~NexusImagerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    NexusImagerAudioProcessor& audioProcessor;

    // UI Components para las 4 bandas
    struct BandUI {
        juce::Slider widthSlider;
        juce::Slider widenSlider;
        juce::ToggleButton soloButton;
        juce::ToggleButton muteButton;
        juce::ComboBox modeSelector;
        juce::Label title;
        juce::Label labelWidth;
        juce::Label labelWiden;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widenAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> soloAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;
    };
    BandUI bands[4];

    // Crossover Sliders
    juce::Slider freqSliders[3];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachments[3];

    // Vectorscope
    void drawVectorscope(juce::Graphics& g, juce::Rectangle<int> area);
    std::vector<juce::Point<float>> scopePoints;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NexusImagerAudioProcessorEditor)
};
