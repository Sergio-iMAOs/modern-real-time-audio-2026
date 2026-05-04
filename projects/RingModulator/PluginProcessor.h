#pragma once

#include <BaseProcessor.h>

namespace Param
{
    namespace ID
    {
        static const juce::String Freq { "freq" };
    }

    namespace Name
    {
        static const juce::String Freq { "Frequency" };
    }

    namespace Ranges
    {
        static const float FreqMin { 20.f };
        static const float FreqMax { 20000.f };
        static const float FreqInc { 1.f };
        static const float FreqSkw { 0.3f };
    }

    namespace Unit
    {
        static const juce::String Freq { "Hz" };
    }
}

namespace
{
    constexpr size_t TABLE_SIZE { 512 };
} // anonymous namespace

class PluginProcessor final : public mrta::BaseProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    // Called before processing starts
    void prepare(double sampleRate, int samplesPerBlock) override;

    // Audio stream callback
    void process(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void stepFrame();
    void setFrequency(float newFrequency);

    // Creates the GUI
    juce::AudioProcessorEditor* createEditor() override;

private:
    
    // Padded lookup table
    std::array<float, TABLE_SIZE + 1> lookupTable;

    float phase { 0.f };
    float phaseIncr { 0.f };
    float freq;
    float samplePeriod;

    // Read current sample of lookup table with linear interpolation
    float getWTSample();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
