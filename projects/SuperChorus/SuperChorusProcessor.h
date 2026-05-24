#pragma once

#include <BaseProcessor.h>

#include <SuperChorus.h>
#include <Ramp.h>


namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String Depth { "depth" };
        static const juce::String Rate { "rate" };
        static const juce::String Offset { "offset" };
        static const juce::String Smear { "smear" };
        static const juce::String Width { "width" };
        static const juce::String Distort { "distort" };
        static const juce::String BitDepth { "bitDepth" };
        static const juce::String Mix { "mix" };
        static const juce::String Voices { "voices" };
        static const juce::String ModType { "modType" };
        static const juce::String DistType { "distType" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String Depth { "Depth" };
        static const juce::String Rate { "Rate" };
        static const juce::String Offset { "Delay" };
        static const juce::String Smear { "Time Smearing" };
        static const juce::String Width { "Width" };
        static const juce::String Distort { "Distort" };
        static const juce::String BitDepth { "Bit Depth" };
        static const juce::String Mix { "Mix" };
        static const juce::String Voices { "Voices" };
        static const juce::String ModType { "Mod Type" };
        static const juce::String DistType { "Dist Type" };
    }

    namespace Ranges
    {
        static constexpr float DepthMin { 0.f };
        static constexpr float DepthMax { 10.f };
        static constexpr float DepthInc { 0.01f };
        static constexpr float DepthSkw { 0.5f };

        static constexpr float RateMin { 0.1f };
        static constexpr float RateMax { 5.f };
        static constexpr float RateInc { 0.01f };
        static constexpr float RateSkw { 0.5f };

        static constexpr float OffsetMin { 5.f };
        static constexpr float OffsetMax { 50.f };
        static constexpr float OffsetInc { 0.1f };
        static constexpr float OffsetSkw { 0.5f };

        static constexpr float SmearMin { 0.f };
        static constexpr float SmearMax { 2.f };
        static constexpr float SmearInc { 0.01f };
        static constexpr float SmearSkw { 0.5f };

        static constexpr float WidthMin { 0.f };
        static constexpr float WidthMax { 1.f };
        static constexpr float WidthInc { 0.01f };
        static constexpr float WidthSkw { 0.5f };

        static constexpr float DistortMin { 1.f };
        static constexpr float DistortMax { 10.f };
        static constexpr float DistortInc { 0.01f };
        static constexpr float DistortSkw { 0.5f };

        static constexpr float BitDepthMin { 1.f };
        static constexpr float BitDepthMax { 16.f };
        static constexpr float BitDepthInc { 0.01f };
        static constexpr float BitDepthSkw { 0.5f };

        static constexpr float MixMin { 0.f };
        static constexpr float MixMax { 1.f };
        static constexpr float MixInc { 0.001f };
        static constexpr float MixSkw { 1.f };

        static constexpr float VoicesMin { 2.f };
        static constexpr float VoicesMax { static_cast<float>(DSP::SuperChorus::MAX_VOICES) };
        static constexpr float VoicesInc { 1.f };
        static constexpr float VoicesSkw { 1.f };

        static const juce::StringArray ModLabels { "Sine", "Triangle" };
        static const juce::StringArray DistLabels { "Soft-Clip", "Hard-Clip" };

        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };
    }

    namespace Units
    {
        static const juce::String Ms { "ms" };
        static const juce::String Hz { "Hz" };
    }
}

class SuperChorusProcessor : public mrta::BaseProcessor
{
public:
    static constexpr float MaxDelaySizeMs { 
        Param::Ranges::VoicesMax * Param::Ranges::OffsetMax + Param::Ranges::DepthMax * Param::Ranges::SmearMax
    };
    static const unsigned int MaxChannels { 2 };

    SuperChorusProcessor();
    ~SuperChorusProcessor() override;

    // Called before processing starts
    void prepare(double sampleRate, int samplesPerBlock) override;

    // Audio stream callback
    void process(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // Creates the GUI
    juce::AudioProcessorEditor* createEditor() override;

private:
    DSP::SuperChorus superChorus;

    bool enabled { true };
    juce::AudioBuffer<float> fxBuffer;

    DSP::Ramp<float> wetRamp;
    DSP::Ramp<float> dryRamp;
    float mix { 0.5f };
    float offset { 0.f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChorusProcessor)
};