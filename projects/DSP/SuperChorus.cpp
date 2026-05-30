#include "SuperChorus.h"

#include <cmath>
#include <algorithm>


namespace DSP
{

SuperChorus::SuperChorus(float maxTimeMs, unsigned int numChannels) :
    distortionRamp(0.05f),
    widthRamp(0.05f),
    maxTimeMs(maxTimeMs)
{
    // Construct chorus voices
    for (auto i = 0; i < MAX_VOICES; ++i)
    {
        voices[i] = std::make_unique<DSP::Chorus>(maxTimeMs, numChannels);
    }
}

SuperChorus::~SuperChorus()
{
}

void SuperChorus::prepare(double newSampleRate, int samplesPerBlock, float maxTimeMs, unsigned int numChannels)
{
    distortionRamp.prepare(newSampleRate, true, distortion);
    widthRamp.prepare(newSampleRate, true, width);
    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();
    inputBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    inputBuffer.clear();
    stereoBuffer.setSize(1, samplesPerBlock);
    stereoBuffer.clear();
    bitLevelBuffer.setSize(1, samplesPerBlock);
    bitLevelBuffer.clear();

    for (auto& chorus : voices)
        chorus->prepare(newSampleRate, maxTimeMs, numChannels);
}

void SuperChorus::clear()
{
    for (auto chorusIdx = 0; chorusIdx < numVoices; ++chorusIdx)
        voices[chorusIdx]->clear();
}

void SuperChorus::reset()
{
    for (auto chorusIdx = 0; chorusIdx < numVoices; ++chorusIdx)
        voices[chorusIdx]->reset();
}

void SuperChorus::process(juce::AudioBuffer<float>& buffer)
{
    const unsigned int numChannels { static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples { static_cast<unsigned int>(buffer.getNumSamples()) };
    const auto numChannelsInv = 1.f / static_cast<float>(numChannels);

    // Prepare stereo and bit depth buffers
    for (auto sampleIdx = 0; sampleIdx < numSamples; ++sampleIdx)
    {
        stereoBuffer.setSample(0, sampleIdx, 1.f);
        auto bitLevel = std::pow(2.f, bitDepthRamp.getNext() - 1.f);
        bitLevelBuffer.setSample(0, sampleIdx, bitLevel);
    }
    widthRamp.applyGain(stereoBuffer.getArrayOfWritePointers(), 1, numSamples);

    // Copy to input buffer
    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        inputBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));

    // Clear output
    buffer.clear();

    for (auto chorusIdx = 0; chorusIdx < numVoices; ++chorusIdx)
    {
        auto& chorus = voices[chorusIdx];

        // Copy input buffer
        for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
            fxBuffer.copyFrom(ch, 0, inputBuffer, ch, 0, static_cast<int>(numSamples));

        // Process chorus
        chorus->process(fxBuffer.getArrayOfWritePointers(), fxBuffer.getArrayOfReadPointers(), numChannels, numSamples);

        // Calculate stereo position [-1.0, 1.0]
        auto stereoPos = chorusIdx * numVoicesInv * 2.f - 1.f;

        // Calculate distortion depending on stereo position
        // More saturation drive at stereo extremes, x1 at centre
        distortionRamp.applyGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);
        const auto stereoDrive = 1.f + std::fabs(stereoPos);
        fxBuffer.applyGain(0, numSamples, stereoDrive);

        // Apply distortion and stereo panning
        for (auto sampleIdx = 0; sampleIdx < numSamples; ++sampleIdx)
        {
            auto sample = 0.f;

            // Sum to mono
            for (auto chIdx = 0; chIdx < numChannels; ++chIdx)
            {
                sample += fxBuffer.getSample(chIdx, sampleIdx);
            }
            sample *= numChannelsInv;

            // Distortion
            switch (distortionType)
            {
                case DistortionType::SoftClip:
                    sample = std::tanh(sample);
                    break;
                case DistortionType::HardClip:
                    sample = std::clamp(sample, -1.f, 1.f);
                    break;
            }

            // Apply bit crushing
            auto bitLevel = bitLevelBuffer.getSample(0, sampleIdx);
            sample = std::round(sample * bitLevel) / bitLevel;

            // Adjust stereo position to width
            auto workingStereoPos = stereoPos * stereoBuffer.getSample(0, sampleIdx);

            // Apply panning adding the result to 'buffer'
            // Equal-power panning: map workingStereoPos [-1, 1] to angle [0, pi/2]
            const float panAngle = (workingStereoPos + 1.f) * (juce::MathConstants<float>::pi * 0.25f);
            const float leftGain  = std::cos(panAngle);
            const float rightGain = std::sin(panAngle);

            // Add to output buffer applying gain compensation
            auto gainCompensation = std::max(numVoicesInv, 0.125f);
            buffer.addSample(0, sampleIdx, sample * leftGain  * gainCompensation);
            if (numChannels > 1)
                buffer.addSample(1, sampleIdx, sample * rightGain * gainCompensation);
        }
    }
}

void SuperChorus::setOffset(float newOffsetMs)
{
    offset = newOffsetMs;
    auto halfVoices = numVoices / 2;
    const auto smearStep = static_cast<float>(MAX_VOICES) / numVoices;

    // Assign each chorus voice a different offset
    // Voices further away from stereo filed centre are smeared more
    for (auto i = 0; i < halfVoices; ++i)
    {
        auto smear = static_cast<float>(halfVoices - 1 - i) * smearStep;
        voices[i]->setOffset(offset + offset * smear * timeSmearing);
        voices[numVoices - 1 - i]->setOffset(offset + offset * (smear + 0.5f) * timeSmearing);
    }
    // Check parity
    if (numVoices & 1)
    {
        voices[halfVoices]->setOffset(offset - offset * 0.25f * timeSmearing);
    }
}

void SuperChorus::setTimeSmearing(float newTimeSmearing)
{
    timeSmearing = std::max(newTimeSmearing, 0.f);
    setOffset(offset);
}

void SuperChorus::setDistortionType(DistortionType newDistType)
{
    distortionType = newDistType;
}

void SuperChorus::setBitDepth(float newBitDepth)
{
    auto bitDepth = std::max(newBitDepth, 1.f);
    bitDepthRamp.setTarget(bitDepth);
}

void SuperChorus::setNumVoices(unsigned int newNumVoices)
{
    numVoices = newNumVoices;
    numVoicesInv = 1.f / static_cast<float>(numVoices - 1);

    // Set different modulation phases
    auto phaseProp = 2.f * M_PI / numVoices;
    for (auto chorusIdx = 0; chorusIdx < numVoices; ++chorusIdx)
    {
        voices[chorusIdx]->setPhaseOffset(phaseProp * chorusIdx);
    }
    setOffset(offset);
    reset();
}

void SuperChorus::setDepth(float newDepthMs)
{
    for (auto& chorus : voices)
        chorus->setDepth(newDepthMs);
}

void SuperChorus::setModulationRate(float newModRateHz)
{
    for (auto& chorus : voices)
        chorus->setModulationRate(newModRateHz);
}

void SuperChorus::setModulationType(Chorus::ModulationType newModType)
{
    for (auto& chorus : voices)
        chorus->setModulationType(newModType);
}

void SuperChorus::setStereoWidth(float newStereoWidth)
{
    width = std::fmax(newStereoWidth, 0.f);
    widthRamp.setTarget(width);
}

void SuperChorus::setDistortion(float newDistortion)
{
    distortion = newDistortion;
    distortionRamp.setTarget(distortion);
}

std::vector<float> SuperChorus::getNormalizedTimePositions()
{
    std::vector<float> times(numVoices);
    for (auto chorusIdx = 0; chorusIdx < numVoices; ++chorusIdx)
    {
        times[chorusIdx] = voices[chorusIdx]->getTimePosition() / maxTimeMs;
    }
    return times;
}

std::vector<float> SuperChorus::getNormalizedStereoPositions()
{
    /*
    This is executed by the message thread, who
    changes numVoices, the audio thread never changes
    numVoices, so this is concurrent safe
    */
    std::vector<float> positions(numVoices);
    for (auto chorusIdx = 0; chorusIdx < numVoices; ++chorusIdx)
    {
        positions[chorusIdx] = chorusIdx * numVoicesInv * width + (0.5f - width * 0.5f);
    }
    return positions;
}

}