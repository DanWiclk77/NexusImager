#pragma once
#include <JuceHeader.h>
#include "DSP/MultibandCrossover.h"
#include "DSP/StereoEngine.h"

class NexusImagerAudioProcessor : public juce::AudioProcessor
{
public:
    NexusImagerAudioProcessor();
    ~NexusImagerAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Nexus Imager"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override { return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

private:
    LinearPhaseCrossover crossover;
    StereoEngine engines[4];
    
    // Buffers para las bandas
    juce::AudioBuffer<float> bandBuffers[4];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NexusImagerAudioProcessor)
};
