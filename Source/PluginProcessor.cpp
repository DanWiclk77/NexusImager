#include "PluginProcessor.h"
#include "PluginEditor.h"

NexusImagerAudioProcessor::NexusImagerAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

NexusImagerAudioProcessor::~NexusImagerAudioProcessor() {}

void NexusImagerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32)samplesPerBlock, 2 };
    crossover.prepare(spec);

    for (int i = 0; i < 4; ++i)
    {
        engines[i].prepare(sampleRate);
        bandBuffers[i].setSize(2, samplesPerBlock);
    }
}

void NexusImagerAudioProcessor::releaseResources() {}

void NexusImagerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // 1. Dividir en bandas
    // crossover.process(...) -> rellena bandBuffers

    // 2. Procesar cada banda con su StereoEngine
    for (int i = 0; i < 4; ++i)
    {
        auto width = apvts.getRawParameterValue("width" + juce::String(i))->load();
        auto widen = apvts.getRawParameterValue("widener" + juce::String(i))->load();
        auto active = apvts.getRawParameterValue("active" + juce::String(i))->load() > 0.5f;

        if (active)
        {
            engines[i].processBand(bandBuffers[i].getWritePointer(0), 
                                  bandBuffers[i].getWritePointer(1), 
                                  buffer.getNumSamples(), 
                                  width, widen, true);
        }
    }

    // 3. Sumar bandas de vuelta al buffer principal
    buffer.clear();
    for (int i = 0; i < 4; ++i)
    {
        for (int ch = 0; ch < 2; ++ch)
            buffer.addFrom(ch, 0, bandBuffers[i], ch, 0, buffer.getNumSamples());
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout NexusImagerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    for (int i = 0; i < 4; ++i)
    {
        params.push_back(std::make_unique<juce::AudioParameterFloat>("width" + juce::String(i), "Width " + juce::String(i), 0.0f, 2.0f, 1.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("widener" + juce::String(i), "Widener " + juce::String(i), 0.0f, 1.0f, 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterBool>("active" + juce::String(i), "Active " + juce::String(i), true));
    }
    return { params.begin(), params.end() };
}

void NexusImagerAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {}
void NexusImagerAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {}

juce::AudioProcessorEditor* NexusImagerAudioProcessor::createEditor()
{
    return new NexusImagerAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NexusImagerAudioProcessor();
}
