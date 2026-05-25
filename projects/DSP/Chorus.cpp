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
    samplePeriod = static_cast<float>(1.0 / sampleRate);

    delayLine.prepare(static_cast<unsigned int>(std::round(maxTimeMs * static_cast<float>(0.001 * sampleRate))), MaxChannels);
    delayLine.setDelaySamples(static_cast<unsigned int>(std::ceil(0.001 * sampleRate))); // Set fixed delay to 1ms

    offsetRamp.prepare(sampleRate, true, offsetMs * static_cast<float>(0.001 * sampleRate));
    modDepthRamp.prepare(sampleRate, true, modDepthMs * static_cast<float>(0.001 * sampleRate));

    phaseOffsetRamp.prepare(sampleRate, true, offsetMs * static_cast<float>(0.001 * sampleRate));
    phaseRamp.prepare(sampleRate, true, offsetMs * static_cast<float>(0.001 * sampleRate));

    phaseState = 0.f;
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
}

void Chorus::clear()
{
    delayLine.clear();
}

void Chorus::reset()
{
    // Phase reset ramping
    phaseRamp.setTarget(phaseState, true);
    phaseRamp.setTarget(0.f);
    phaseState = 0.f;
}

void Chorus::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{
    numChannels = std::min(numChannels, MaxChannels);
    constexpr float F_PI = static_cast<float>(M_PI);
    float lfo[2] { 0.f, 0.f };

    for (unsigned int n = 0; n < numSamples; ++n)
    {
        // Apply phase offset with phase wrapping
        phaseOffset = phaseOffsetRamp.getNext();
        float phase = phaseState + phaseRamp.getNext() + phaseOffset \
                    - (2.f * F_PI * static_cast<float>(phaseState + phaseOffset > 2.f * F_PI));

        // Process LFO acording to mod type
        switch (modType)
        {
        case Tri:
            lfo[0] = std::fabs((phase - static_cast<float>(M_PI)) / static_cast<float>(M_PI));
            break;

        case Sin:
            lfo[0] = 0.5f + 0.5f * std::sin(phase);
            break;
        }

        // Increment and wrap phase states
        phaseState = std::fmod(phaseState + phaseInc, static_cast<float>(2 * M_PI));

        // Apply mod depth and offset ramps
        modDepthRamp.applyGain(lfo, 1);
        offsetRamp.applySum(lfo, 1);

        // Copy lfo sample
        lfo[1] = lfo[0];

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

    // Store time position to load from UI
    const float modDelayMs = lfo[0] * 1000.f * samplePeriod;
    timePos.store(modDelayMs);
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

float Chorus::getTimePosition()
{
    return timePos.load();
}

}