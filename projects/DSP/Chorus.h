#pragma once

#include "DelayLine.h"
#include "Ramp.h"

#include <atomic>

namespace DSP
{

class Chorus
{
public:
    enum ModulationType : unsigned int
    {
        Sin = 0,
        Tri
    };

    Chorus(float maxTimeMs, unsigned int numChannels);
    ~Chorus();

    // No default ctor
    Chorus() = delete;

    // No copy semantics
    Chorus(const Chorus&) = delete;
    const Chorus& operator=(const Chorus&) = delete;

    // No move semantics
    Chorus(Chorus&&) = delete;
    const Chorus& operator=(Chorus&&) = delete;

    // Update sample rate, reallocates and clear internal buffers
    void prepare(double sampleRate, float maxTimeMs, unsigned int numChannels);

    // Clear contents of internal buffer
    void clear();

    // Resets phase
    void reset();

    // Process audio
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Set delay offset in ms
    void setOffset(float newOffsetMs);

    // Set modulation depth in ms
    void setDepth(float newDepthMs);

    // Set delay time modulation rate in Hz
    void setModulationRate(float newModRateHz);

    // Set delay time modulation waveform type
    void setModulationType(ModulationType newModType);

    void setPhaseOffset(float newPhaseOffset);

    float getTimePosition();

    static constexpr unsigned int MaxChannels { 2 };

private:
    double sampleRate { 48000.0 };
    float samplePeriod { static_cast<float>(1.0 / sampleRate) };

    DelayLine delayLine;

    Ramp<float> offsetRamp;
    Ramp<float> modDepthRamp;
    Ramp<float> phaseOffsetRamp;
    Ramp<float> phaseRamp;  // For ramping phase reset

    float phaseState { 0.f };
    float phaseInc { 0.f };
    float phaseOffset { 0.f };

    float offsetMs { 0.f };
    float modDepthMs { 0.f };
    float modRate { 0.f };

    ModulationType modType { Sin };

    std::atomic<float> timePos;
};

}