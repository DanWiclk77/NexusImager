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
    StereoEngine() 
    {
        delayBuffer.setSize(1, 4096);
        delayBuffer.clear();
    }

    void prepare(double sampleRate)
    {
        this->sampleRate = sampleRate;
        delayBuffer.clear();
        writePos = 0;
    }

    void processBand(float* left, float* right, int numSamples, float width, float wideningAmount, int monitorMode, bool isMuted)
    {
        if (numSamples <= 0 || left == nullptr) return;
        
        if (isMuted)
        {
            juce::FloatVectorOperations::clear(left, numSamples);
            if (right != nullptr) juce::FloatVectorOperations::clear(right, numSamples);
            return;
        }

        // Si es mono, el Widener no tiene efecto (mono-compatibilidad perfecta)
        if (right == nullptr) return;

        // Widener Delay params (approx 15ms)
        const int delaySamples = (int)(0.015 * sampleRate); 

        for (int i = 0; i < numSamples; ++i)
        {
            float m = (left[i] + right[i]) * 0.5f;
            float s = (left[i] - right[i]) * 0.5f;

            // --- MONO-COMPATIBLE WIDENER (Sidewidener/Wider Methodology) ---
            // Guardamos Mid en el buffer
            delayBuffer.setSample(0, writePos, m);
            
            if (wideningAmount > 0.01f)
            {
                // Extraemos una versión retrasada del Mid
                int readPos = (writePos - delaySamples + delayBuffer.getNumSamples()) % delayBuffer.getNumSamples();
                float delayedMid = delayBuffer.getSample(0, readPos);
                
                // Inyectamos el delay en el SIDE. 
                // Al ser inyectado solo en el Side (S = L-R), al sumar L+R en mono se cancela:
                // L_out = M + (S + delayedMid)
                // R_out = M - (S + delayedMid)
                // L+R = 2M. Perfecta compatibilidad mono.
                s += delayedMid * wideningAmount * 0.8f; // Factor 0.8 para balance
            }

            writePos = (writePos + 1) % delayBuffer.getNumSamples();

            // --- STEREO ENHANCER (M/S Width Control) ---
            s *= width;

            // --- MONITOR MODES ---
            if (monitorMode == 1)      s = 0.0f;   // MID
            else if (monitorMode == 2) m = 0.0f;    // SIDE

            // --- RE-SÍNTESIS L/R ---
            left[i] = m + s;
            right[i] = m - s;
        }
    }

private:
    double sampleRate = 44100.0;
    juce::AudioBuffer<float> delayBuffer;
    int writePos = 0;
};
