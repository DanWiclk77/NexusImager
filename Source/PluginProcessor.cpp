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

    scopeBuffer.setSize(2, 1024);
    scopeBuffer.clear();
    scopeWritePos = 0;

    isPrepared = true;
}

void NexusImagerAudioProcessor::getVectorscopePoints(std::vector<juce::Point<float>>& points)
{
    points.clear();
    int readPos = scopeWritePos.load();
    for (int i = 0; i < 512; ++i)
    {
        int p = (readPos - i + 1024) % 1024;
        points.push_back({ scopeBuffer.getSample(0, p), scopeBuffer.getSample(1, p) });
    }
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

    // 0. Actualizar Crossovers
    crossover.setCrossoverFrequency(0, apvts.getRawParameterValue("f1")->load());
    crossover.setCrossoverFrequency(1, apvts.getRawParameterValue("f2")->load());
    crossover.setCrossoverFrequency(2, apvts.getRawParameterValue("f3")->load());

    // 1. Dividir en bandas (usando la lógica del crossover)
    crossover.process(juce::dsp::AudioBlock<float>(buffer), bandBuffers);

    // ... (Procesamiento de motores) ...

    // 3. Sumar bandas de vuelta al buffer principal
    buffer.clear();
    const int outChannels = buffer.getNumChannels();
    for (int i = 0; i < 4; ++i)
    {
        for (int ch = 0; ch < juce::jmin(outChannels, (int)bandBuffers[i].getNumChannels()); ++ch)
            buffer.addFrom(ch, 0, bandBuffers[i], ch, 0, buffer.getNumSamples());
    }

    // 4. Capturar datos para Vectorscope (del buffer final sumado)
    auto samples = buffer.getNumSamples();
    auto* l = buffer.getReadPointer(0);
    auto* r = buffer.getNumChannels() > 1 ? buffer.getReadPointer(1) : l;

    for (int i = 0; i < samples; ++i)
    {
        int pos = scopeWritePos.load();
        scopeBuffer.setSample(0, pos, l[i]);
        scopeBuffer.setSample(1, pos, r[i]);
        scopeWritePos.store((pos + 1) % scopeBuffer.getNumSamples());
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout NexusImagerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    for (int i = 0; i < 4; ++i)
    {
        juce::String id = juce::String(i);
        params.push_back(std::make_unique<juce::AudioParameterFloat>("wid" + id, "Stereo Enhancer " + id, 0.0f, 2.0f, 1.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("wdr" + id, "Widener " + id, 0.0f, 2.0f, 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterBool>("mut" + id, "Mute " + id, false));
        params.push_back(std::make_unique<juce::AudioParameterBool>("sol" + id, "Solo " + id, false));
        params.push_back(std::make_unique<juce::AudioParameterChoice>("mod" + id, "Mode " + id, juce::StringArray{"Stereo", "Mid", "Side"}, 0));
    }

    // Crossovers
    params.push_back(std::make_unique<juce::AudioParameterFloat>("f1", "Low-Mid Crossover", 20.0f, 500.0f, 200.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("f2", "Mid-High Crossover", 500.0f, 5000.0f, 1000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("f3", "High Crossover", 5000.0f, 20000.0f, 5000.0f));

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
