/*
  ==============================================================================
    MultibandCrossover.h
    Crossover de fase lineal para separación de bandas sin distorsión de fase.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LinearPhaseCrossover
{
public:
    LinearPhaseCrossover() {}

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        numChannels = spec.numChannels;
        sampleRate = spec.sampleRate;

        for (int i = 0; i < 3; ++i)
        {
            filters[i].prepare(spec);
            filters[i].setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
            
            highFilters[i].prepare(spec);
            highFilters[i].setType(juce::dsp::LinkwitzRileyFilterType::highpass);
            
            updateFilters(i);
        }
    }

    void setCrossoverFrequency(int index, float frequency)
    {
        if (index >= 0 && index < 3)
        {
            frequencies[index] = frequency;
            updateFilters(index);
        }
    }

    void process(const juce::dsp::AudioBlock<float>& inputBlock, std::array<juce::AudioBuffer<float>, 4>& buffers)
    {
        const int numSamples = (int)inputBlock.getNumSamples();

        // Limpiar buffers y copiar entrada para procesar en paralelo/cascada
        for (int i = 0; i < 4; ++i)
        {
            buffers[i].setSize(numChannels, numSamples, false, false, true);
            buffers[i].clear();
            
            for (int ch = 0; ch < numChannels; ++ch)
                buffers[i].copyFrom(ch, 0, inputBlock.getChannelPointer(ch), numSamples);
        }

        // Diseño de cascada para separación de 4 bandas
        juce::dsp::AudioBlock<float> b0(buffers[0]);
        juce::dsp::AudioBlock<float> b1(buffers[1]);
        juce::dsp::AudioBlock<float> b2(buffers[2]);
        juce::dsp::AudioBlock<float> b3(buffers[3]);

        // Banda 0: Low
        filters[0].process(juce::dsp::ProcessContextReplacing<float>(b0));

        // Banda 1: Low-Mid
        highFilters[0].process(juce::dsp::ProcessContextReplacing<float>(b1));
        filters[1].process(juce::dsp::ProcessContextReplacing<float>(b1));

        // Banda 2: High-Mid
        highFilters[1].process(juce::dsp::ProcessContextReplacing<float>(b2));
        filters[2].process(juce::dsp::ProcessContextReplacing<float>(b2));

        // Banda 3: High
        highFilters[2].process(juce::dsp::ProcessContextReplacing<float>(b3));
    }

private:
    void updateFilters(int index)
    {
        float freq = frequencies[index];
        filters[index].setCutoffFrequency(freq);
        highFilters[index].setCutoffFrequency(freq);
    }

    int numChannels = 2;
    double sampleRate = 44100.0;
    float frequencies[3] = { 200.0f, 1000.0f, 5000.0f };
    juce::dsp::LinkwitzRileyFilter<float> filters[3];
    juce::dsp::LinkwitzRileyFilter<float> highFilters[3];
};
