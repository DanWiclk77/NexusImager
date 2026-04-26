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
    isPrepared = false;

    if (sampleRate <= 0.0 || samplesPerBlock <= 0)
        return;

    const int numChannels = juce::jmax(2, getTotalNumOutputChannels());
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32)samplesPerBlock, (juce::uint32)numChannels };
    
    crossover.prepare(spec);

    for (int i = 0; i < 4; ++i)
    {
        engines[i].prepare(sampleRate);
        bandBuffers[i].setSize(numChannels, samplesPerBlock);
        bandBuffers[i].clear();
    }

    isPrepared = true;
}

void NexusImagerAudioProcessor::releaseResources() {}

void NexusImagerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!isPrepared.load())
        return;

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // 1. Dividir en bandas (usando la lógica del crossover)
    crossover.process(juce::dsp::AudioBlock<float>(buffer), bandBuffers);

    // 2. Procesar cada banda con su StereoEngine
    bool anySolo = false;
    for (int i = 0; i < 4; ++i)
    {
        auto* pSolo = apvts.getRawParameterValue("sol" + juce::String(i));
        if (pSolo != nullptr && pSolo->load() > 0.5f)
        {
            anySolo = true;
            break;
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        juce::String id = juce::String(i);
        
        auto* pWidth = apvts.getRawParameterValue("wid" + id);
        auto* pWiden = apvts.getRawParameterValue("wdr" + id);
        auto* pMode  = apvts.getRawParameterValue("mod" + id);
        auto* pMute  = apvts.getRawParameterValue("mut" + id);
        auto* pSolo  = apvts.getRawParameterValue("sol" + id);

        if (pWidth == nullptr || pWiden == nullptr || pMode == nullptr || pMute == nullptr || pSolo == nullptr)
            continue;

        auto width = pWidth->load();
        auto widen = pWiden->load();
        auto mode  = (int)pMode->load();
        auto mute  = pMute->load() > 0.5f;
        auto solo  = pSolo->load() > 0.5f;

        bool shouldProcess = true;
        if (anySolo && !solo) shouldProcess = false;
        if (mute) shouldProcess = false;

        float* lPtr = bandBuffers[i].getWritePointer(0);
        float* rPtr = bandBuffers[i].getNumChannels() > 1 ? bandBuffers[i].getWritePointer(1) : nullptr;

        engines[i].processBand(lPtr, rPtr, buffer.getNumSamples(), width, widen, mode, !shouldProcess);
    }

    // 3. Sumar bandas de vuelta al buffer principal
    buffer.clear();
    const int outChannels = buffer.getNumChannels();
    for (int i = 0; i < 4; ++i)
    {
        for (int ch = 0; ch < juce::jmin(outChannels, (int)bandBuffers[i].getNumChannels()); ++ch)
            buffer.addFrom(ch, 0, bandBuffers[i], ch, 0, buffer.getNumSamples());
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout NexusImagerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    for (int i = 0; i < 4; ++i)
    {
        juce::String id = juce::String(i);
        params.push_back(std::make_unique<juce::AudioParameterFloat>("wid" + id, "Width " + id, 0.0f, 2.0f, 1.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("wdr" + id, "Widener " + id, 0.0f, 1.0f, 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterBool>("mut" + id, "Mute " + id, false));
        params.push_back(std::make_unique<juce::AudioParameterBool>("sol" + id, "Solo " + id, false));
        params.push_back(std::make_unique<juce::AudioParameterChoice>("mod" + id, "Mode " + id, juce::StringArray{"Stereo", "Mid", "Side"}, 0));
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
