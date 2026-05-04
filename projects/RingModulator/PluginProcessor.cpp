#include "PluginProcessor.h"
#include "PluginEditor.h"

static std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Freq, Param::Name::Freq, Param::Unit::Freq, 100.f, Param::Ranges::FreqMin, Param::Ranges::FreqMax, Param::Ranges::FreqInc, Param::Ranges::FreqSkw },
};

PluginProcessor::PluginProcessor() :
    mrta::BaseProcessor(parameters)
{
    registerParameterCallback(Param::ID::Freq,
    [this] (float val, bool /*force*/)
    {
        this->setFrequency(val);
    });

    // Populate lookup table
    /*
    The last padding value is equal to the first value
    This allows reading with interpolation without needing to
    warp the read pointer
    */
    for (int i = 0; i < TABLE_SIZE + 1; i++) {
        lookupTable[i] = std::cos(2.0f * M_PI * i / TABLE_SIZE);
    }
}

PluginProcessor::~PluginProcessor()
{
}

void PluginProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    // Avoid divisions during audio processing
    samplePeriod = 1.f / sampleRate;

    // Update phase
    setFrequency(freq);
}

void PluginProcessor::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    // Per frame processing
    for (auto frameIdx = 0; frameIdx < buffer.getNumSamples(); ++frameIdx) {

        // Get modulation sample from oscillator
        auto modSample = getWTSample();

        for (auto channelIdx = 0; channelIdx < buffer.getNumChannels(); ++channelIdx) {
            auto* channelPtr = buffer.getWritePointer(channelIdx);

            // Apply ring modulation
            channelPtr[frameIdx] *= modSample;
        }
        stepFrame();
    }
}

void PluginProcessor::stepFrame()
{
    // Increment phase
    phase += phaseIncr;

    // Wrap phase (branchless)
    phase -= TABLE_SIZE * static_cast<float>(phase >= TABLE_SIZE);
}

void PluginProcessor::setFrequency(float newFrequency)
{
    phaseIncr = newFrequency * samplePeriod * static_cast<float>(TABLE_SIZE);

    // Store frequency for updating phase when calling prepare
    freq = newFrequency;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

float PluginProcessor::getWTSample()
{
    // Truncate fractional part (decimals)
    int position = phase;
    
    // Get fractional part
    auto frac = phase - position;

    // Read lookup table with linear interpolation
    auto sample = lookupTable[position] * (1.f - frac) + \
                lookupTable[position + 1] * frac;

    return sample;
}

CREATE_PLUGIN(PluginProcessor)
