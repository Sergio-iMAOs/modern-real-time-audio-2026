#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include "DelayLine.h"
#include "Ramp.h"
#include "Chorus.h"

namespace DSP
{

class SuperChorus
{
public:
    enum DistortionType : unsigned int
    {
        SoftClip = 0,
        HardClip
    };

    static constexpr unsigned int MAX_VOICES { 16 };

    SuperChorus(float maxTimeMs, unsigned int numChannels);
    ~SuperChorus();

    // No default ctor
    SuperChorus() = delete;

    // No copy semantics
    SuperChorus(const SuperChorus&) = delete;
    const SuperChorus& operator=(const SuperChorus&) = delete;

    // No move semantics
    SuperChorus(SuperChorus&&) = delete;
    const SuperChorus& operator=(SuperChorus&&) = delete;

    // Update sample rate, reallocates and clear internal buffers
    void prepare(double sampleRate, int samplesPerBlock, float maxTimeMs, unsigned int numChannels);

    // Clear contents of internal buffer
    void clear();

    // Reset modulation phases
    void reset();

    // Process audio
    void process(juce::AudioBuffer<float>& buffer);

    // Set modulation depth in ms
    void setDepth(float newDepthMs);

    // Set delay time modulation rate in Hz
    void setModulationRate(float newModRateHz);

    // Set delay time modulation waveform type
    void setModulationType(Chorus::ModulationType newModType);

    void setStereoWidth(float newStereoWidth);

    void setDistortion(float newDistortion);

    void setOffset(float newOffsetMs);

    // Set how appart is the time offset of each voice
    void setTimeSmearing(float newTimeSmearing);

    void setDistortionType(DistortionType newDistType);

    void setBitDepth(float newBitDepth);

    void setNumVoices(unsigned int newNumVoices);

    std::vector<float> getNormalizedTimePositions();
    std::vector<float> getNormalizedStereoPositions();

    static constexpr unsigned int MaxChannels { 2 };

private:

    Ramp<float> widthRamp;
    Ramp<float> distortionRamp;
    Ramp<float> bitDepthRamp;
    float width { 0.5f };
    float distortion { 1.f };
    float offset { 0.f };
    float timeSmearing { 0.f };
    DistortionType distortionType;

    juce::AudioBuffer<float> fxBuffer;
    juce::AudioBuffer<float> inputBuffer;
    juce::AudioBuffer<float> stereoBuffer;
    juce::AudioBuffer<float> bitLevelBuffer;

    unsigned int numVoices { 2 };
    // 1/numVoices -> Avoid divisions during processing
    float numVoicesInv { 1.f };

    std::array<std::unique_ptr<DSP::Chorus>, MAX_VOICES> voices;

    const float maxTimeMs;
};

}