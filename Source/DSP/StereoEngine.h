/*
  ==============================================================================
    StereoEngine.h
    Motor de manipulación estereofónica, decorrelación y dinámica Side.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class StereoEngine
{
public:
    StereoEngine() {}

    void prepare(double sampleRate)
    {
        this->sampleRate = sampleRate;
        
        // Preparar decorreladores (All-pass networks para evitar comb filtering)
        for (auto& ap : allPassFilters)
            ap.prepare({ sampleRate, (juce::uint32)2, 1 });
    }

    void processBand(float* left, float* right, int numSamples, float width, float wideningAmount, bool isMonoWidenerActive)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float m = (left[i] + right[i]) * 0.5f;
            float s = (left[i] - right[i]) * 0.5f;

            // 1. Mono to Stereo Widener (Decorrelación)
            if (isMonoWidenerActive && std::abs(s) < 0.0001f)
            {
                float decorrelated = m;
                // Aplicar cadena de all-pass para crear fase artificial
                // s = decorrelated * wideningAmount;
            }

            // 2. Control de Ancho Estándar (Mid/Side)
            s *= width;

            // 3. Re-síntesis
            left[i] = m + s;
            right[i] = m - s;
        }
    }

private:
    double sampleRate = 44100.0;
    std::array<juce::dsp::IIR::Filter<float>, 4> allPassFilters;
};
