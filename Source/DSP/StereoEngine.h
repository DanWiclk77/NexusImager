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

    void processBand(float* left, float* right, int numSamples, float width, float wideningAmount, int monitorMode, bool isMuted)
    {
        if (isMuted || left == nullptr)
        {
            if (left != nullptr) juce::FloatVectorOperations::clear(left, numSamples);
            if (right != nullptr) juce::FloatVectorOperations::clear(right, numSamples);
            return;
        }

        // Si es mono, operamos solo en Left
        if (right == nullptr)
        {
            // En mono simple, el imager no hace nada más que Volumen/Mute
            return;
        }

        for (int i = 0; i < numSamples; ++i)
        {
            float m = (left[i] + right[i]) * 0.5f;
            float s = (left[i] - right[i]) * 0.5f;

            // 1. Mono to Stereo Widener (Decorrelación)
            if (std::fabsf(s) < 0.0001f && wideningAmount > 0.0f)
            {
                // Decorrelación ligera para ensanchar mono
                s = m * wideningAmount * 0.5f; 
            }

            // 2. Control de Ancho (M/S)
            s *= width;

            // 3. Monitor Mode (0: Stereo, 1: Mid, 2: Side)
            if (monitorMode == 1) // Solo Mid
                s = 0.0f;
            else if (monitorMode == 2) // Solo Side
                m = 0.0f;

            // 4. Re-síntesis
            left[i] = m + s;
            right[i] = m - s;
        }
    }

private:
    double sampleRate = 44100.0;
    std::array<juce::dsp::IIR::Filter<float>, 4> allPassFilters;
};
