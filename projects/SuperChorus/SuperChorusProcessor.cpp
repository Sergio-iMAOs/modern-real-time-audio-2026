#include "SuperChorusProcessor.h"
#include "SuperChorusEditor.h"
#include <Chorus.h>

#include <algorithm>

static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,  Param::Ranges::EnabledOff, Param::Ranges::EnabledOn, true },
    { Param::ID::Depth,    Param::Name::Depth,    Param::Units::Ms,  2.f,  Param::Ranges::DepthMin,    Param::Ranges::DepthMax,    Param::Ranges::DepthInc,    Param::Ranges::DepthSkw },
    { Param::ID::Rate,     Param::Name::Rate,     Param::Units::Hz,  0.5f, Param::Ranges::RateMin,     Param::Ranges::RateMax,     Param::Ranges::RateInc,     Param::Ranges::RateSkw },
    { Param::ID::Offset,   Param::Name::Offset,   Param::Units::Ms,  5.f,  Param::Ranges::OffsetMin,   Param::Ranges::OffsetMax,   Param::Ranges::OffsetInc,   Param::Ranges::OffsetSkw },
    { Param::ID::Smear,    Param::Name::Smear,    "",                0.f,  Param::Ranges::SmearMin,    Param::Ranges::SmearMax,    Param::Ranges::SmearInc,    Param::Ranges::SmearSkw },
    { Param::ID::Width,    Param::Name::Width,    "",                0.5f, Param::Ranges::WidthMin,    Param::Ranges::WidthMax,    Param::Ranges::WidthInc,    Param::Ranges::WidthSkw },
    { Param::ID::Distort,  Param::Name::Distort,  "",                1.f,  Param::Ranges::DistortMin,  Param::Ranges::DistortMax,  Param::Ranges::DistortInc,  Param::Ranges::DistortSkw },
    { Param::ID::BitDepth, Param::Name::BitDepth, "",                16.f, Param::Ranges::BitDepthMin, Param::Ranges::BitDepthMax, Param::Ranges::BitDepthInc, Param::Ranges::BitDepthSkw },
    { Param::ID::Mix,      Param::Name::Mix,      "",                0.5f, Param::Ranges::MixMin,      Param::Ranges::MixMax,      Param::Ranges::MixInc,      Param::Ranges::MixSkw },
    { Param::ID::Voices,   Param::Name::Voices,   "",                2.f,  Param::Ranges::VoicesMin,   Param::Ranges::VoicesMax,   Param::Ranges::VoicesInc,   Param::Ranges::VoicesSkw },
    { Param::ID::ModType,  Param::Name::ModType,  Param::Ranges::ModLabels, 0 },
    { Param::ID::DistType, Param::Name::DistType, Param::Ranges::DistLabels, 0 }
};

SuperChorusProcessor::SuperChorusProcessor() :
    mrta::BaseProcessor(parameters),
    superChorus(MaxDelaySizeMs, DSP::SuperChorus::MaxChannels),
    wetRamp(0.05f),
    dryRamp(0.05f)
{
    registerParameterCallback(Param::ID::Enabled,
        [this](float newValue, bool force)
        {
            enabled = newValue;
            wetRamp.setTarget(std::clamp(enabled * mix, 0.f, 1.f), force);
            dryRamp.setTarget(std::clamp((1.f - mix) * enabled + (1.f - enabled), 0.f, 1.f), force);
        });

    registerParameterCallback(Param::ID::Depth,
        [this](float newValue, bool /*force*/)
        {
            superChorus.setDepth(newValue);
        });

    registerParameterCallback(Param::ID::Rate,
        [this] (float newValue, bool /*force*/)
        {
            superChorus.setModulationRate(newValue);
        });

    registerParameterCallback(Param::ID::Offset,
        [this] (float newValue, bool /*force*/)
        {
            superChorus.setOffset(newValue);
        });

    registerParameterCallback(Param::ID::Smear,
        [this] (float newValue, bool /*force*/)
        {
            superChorus.setTimeSmearing(newValue);
        });

    registerParameterCallback(Param::ID::Width,
        [this] (float newValue, bool /*force*/)
        {
            superChorus.setStereoWidth(newValue);
        });

    registerParameterCallback(Param::ID::Distort,
        [this] (float newValue, bool /*force*/)
        {
            superChorus.setDistortion(newValue);
        });

    registerParameterCallback(Param::ID::BitDepth,
        [this] (float newValue, bool /*force*/)
        {
            superChorus.setBitDepth(newValue);
        });

    registerParameterCallback(Param::ID::Mix,
        [this] (float value, bool force)
        {
            mix = value;
            wetRamp.setTarget(std::clamp(enabled * mix, 0.f, 1.f), force);
            dryRamp.setTarget(std::clamp((1.f - mix) * enabled + (1.f - enabled), 0.f, 1.f), force);
        });

    registerParameterCallback(Param::ID::Voices,
        [this] (float newValue, bool /*force*/)
        {
            superChorus.setNumVoices(static_cast<unsigned int>(newValue));
        });

    registerParameterCallback(Param::ID::ModType,
        [this](float newValue, bool /*force*/)
        {
            DSP::Chorus::ModulationType modType = static_cast<DSP::Chorus::ModulationType>(std::round(newValue));
            superChorus.setModulationType(std::min(std::max(modType, DSP::Chorus::Sin), DSP::Chorus::Tri));
        });

    registerParameterCallback(Param::ID::DistType,
        [this](float newValue, bool /*force*/)
        {
            DSP::SuperChorus::DistortionType distType = static_cast<DSP::SuperChorus::DistortionType>(std::round(newValue));
            superChorus.setDistortionType(std::min(std::max(distType, DSP::SuperChorus::DistortionType::SoftClip), DSP::SuperChorus::DistortionType::HardClip));
        });
}

SuperChorusProcessor::~SuperChorusProcessor()
{
}

void SuperChorusProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { static_cast<unsigned int>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };

    superChorus.clear();
    superChorus.prepare(sampleRate, samplesPerBlock, MaxDelaySizeMs, numChannels);
    wetRamp.prepare(sampleRate);
    dryRamp.prepare(sampleRate);

    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();
}

void SuperChorusProcessor::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    const unsigned int numChannels { static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples { static_cast<unsigned int>(buffer.getNumSamples()) };

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        fxBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));

    // Process superChorus
    superChorus.process(fxBuffer);
    
    // Dry wet
    wetRamp.applyGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    dryRamp.applyGain(buffer.getArrayOfWritePointers(), numChannels, numSamples);

    // Add to output
    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        buffer.addFrom(ch, 0, fxBuffer, ch, 0, static_cast<int>(numSamples));
}

DSP::SuperChorus &SuperChorusProcessor::getSuperChorus()
{
    return superChorus;
}

juce::AudioProcessorEditor* SuperChorusProcessor::createEditor()
{
    return new SuperChorusEditor(*this);
}

CREATE_PLUGIN(SuperChorusProcessor)