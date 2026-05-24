#include "Chorus.h"

#include <cmath>
#include <algorithm>


namespace DSP
{

Chorus::Chorus(float maxTimeMs, unsigned int numChannels) :
    delayLine(static_cast<unsigned int>(std::ceil(std::fmax(maxTimeMs, 1.f) * static_cast<float>(0.001 * sampleRate))), numChannels),
    offsetRamp(0.05f),
    modDepthRamp(0.05f)
{
}

Chorus::~Chorus()
{
}

void Chorus::prepare(double newSampleRate, float maxTimeMs, unsigned int numChannels)
{
    sampleRate = newSampleRate;

    delayLine.prepare(static_cast<unsigned int>(std::round(maxTimeMs * static_cast<float>(0.001 * sampleRate))), MaxChannels);
    delayLine.setDelaySamples(static_cast<unsigned int>(std::ceil(0.001 * sampleRate))); // Set fixed delay to 1ms

    offsetRamp.prepare(sampleRate, true, offsetMs * static_cast<float>(0.001 * sampleRate));
    modDepthRamp.prepare(sampleRate, true, modDepthMs * static_cast<float>(0.001 * sampleRate));

    phaseState[0] = 0.f;
    phaseState[1] = 0.f;
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
}

void Chorus::clear()
{
    delayLine.clear();
}

void Chorus::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{
    numChannels = std::min(numChannels, MaxChannels);
    constexpr float F_PI = static_cast<float>(M_PI);

    for (unsigned int n = 0; n < numSamples; ++n)
    {
        // Apply phase offset with phase wrapping
        phaseOffset = phaseOffsetRamp.getNext();
        float phase[2] {
            phaseState[0] + phaseOffset - (2.f * F_PI * static_cast<float>(phaseState[0] + phaseOffset > 2.f * F_PI)),
            phaseState[1] + phaseOffset - (2.f * F_PI * static_cast<float>(phaseState[0] + phaseOffset > 2.f * F_PI))
        };

        // Process LFO acording to mod type
        float lfo[MaxChannels] { 0.f, 0.f };
        switch (modType)
        {
        case Tri:
            lfo[0] = std::fabs((phase[0] - static_cast<float>(M_PI)) / static_cast<float>(M_PI));
            lfo[1] = std::fabs((phase[1] - static_cast<float>(M_PI)) / static_cast<float>(M_PI));
            break;

        case Sin:
            lfo[0] = 0.5f + 0.5f * std::sin(phase[0]);
            lfo[1] = 0.5f + 0.5f * std::sin(phase[1]);
            break;
        }

        // Increment and wrap phase states
        phaseState[0] = std::fmod(phaseState[0] + phaseInc, static_cast<float>(2 * M_PI));
        phaseState[1] = std::fmod(phaseState[1] + phaseInc, static_cast<float>(2 * M_PI));

        // Apply mod depth and offset ramps
        modDepthRamp.applyGain(lfo, numChannels);
        offsetRamp.applySum(lfo, numChannels);

        // Delay in/out
        float x[MaxChannels];
        float y[MaxChannels];

        for (unsigned int ch = 0; ch < numChannels; ++ch)
            x[ch] = input[ch][n];

        // Process delay
        delayLine.process(y, x, lfo, numChannels);

        // Write to output buffers
        for (unsigned int ch = 0; ch < numChannels; ++ch)
            output[ch][n] = y[ch];
    }
}

void Chorus::setOffset(float newOffsetMs)
{
    // Since the fixed delay is set to 1ms
    // We can deduct that from the offset ramp
    offsetMs = std::fmax(newOffsetMs - 1.f, 0.f);
    offsetRamp.setTarget(offsetMs * static_cast<float>(0.001 * sampleRate));
}

void Chorus::setDepth(float newDepthMs)
{
    modDepthMs = std::fmax(newDepthMs, 0.f);
    modDepthRamp.setTarget(modDepthMs * static_cast<float>(0.001 * sampleRate));
}

void Chorus::setModulationRate(float newModRateHz)
{
    modRate = std::fmax(newModRateHz, 0.f);
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
}

void Chorus::setModulationType(ModulationType newModType)
{
    modType = newModType;
}

void Chorus::setPhaseOffset(float newPhaseOffset)
{
    phaseOffsetRamp.setTarget(std::clamp(newPhaseOffset, 0.f, 2.f * static_cast<float>(M_PI)));
}
float Chorus::getCurrentPhase()
{
    return phaseState[0] + phaseOffset - (2.f * M_PI * static_cast<float>(phaseState[0] + phaseOffset > 2.f * M_PI));
}
}