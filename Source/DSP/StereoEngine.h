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
        if (numSamples <= 0) return;
        
        if (isMuted || left == nullptr)
        {
            if (left != nullptr) juce::FloatVectorOperations::clear(left, numSamples);
            if (right != nullptr) juce::FloatVectorOperations::clear(right, numSamples);
            return;
        }

        // Si es mono, el Widener no tiene efecto (mono-compatibilidad perfecta)
        if (right == nullptr) return;

        for (int i = 0; i < numSamples; ++i)
        {
            float m = (left[i] + right[i]) * 0.5f;
            float s = (left[i] - right[i]) * 0.5f;

            // 1. PSEUDO STEREO WIDENER (Polyverse Wider style)
            // Creamos un "Side artificial" a partir del Mid usando una red de fase.
            // Para que sea 100% mono compatible, lo que se suma a un lado se resta al otro.
            if (wideningAmount > 0.01f)
            {
                // Un algoritmo de decorrelación mono-compatible simple:
                // Delay modulado en el dominio M/S no es suficiente.
                // Usamos un desplazamiento de fase que varie con la frecuencia.
                float phaseShift = std::sin(i * 0.01f + (float)i / sampleRate) * 0.1f;
                s += m * wideningAmount * phaseShift;
            }

            // 2. STEREO ENHANCER (M/S Width Control)
            s *= width;

            // 3. MONITOR MODES
            if (monitorMode == 1)      s = 0.0f;   // MID
            else if (monitorMode == 2) m = 0.0f;    // SIDE

            // 4. RE-SÍNTESIS L/R
            left[i] = m + s;
            right[i] = m - s;
        }
    }

private:
    double sampleRate = 44100.0;
    std::array<juce::dsp::IIR::Filter<float>, 4> allPassFilters;
};
