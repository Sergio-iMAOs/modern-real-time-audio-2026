#pragma once

#include <GenericParameterEditor.h>
#include <BaseProcessor.h>

namespace {
    constexpr int DEF_WIDTH { 400 };
    constexpr int DEF_HEIGHT { 300 };
} // anonymous namespace

class PluginEditor  : public juce::AudioProcessorEditor
{
public:
    PluginEditor(mrta::BaseProcessor& p);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    mrta::BaseProcessor& processor;
    mrta::GenericParameterEditor ringModParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};