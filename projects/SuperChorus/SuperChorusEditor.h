#pragma once

#include <BaseProcessor.h>

#include <GenericParameterEditor.h>


class SuperChorusEditor final : public juce::AudioProcessorEditor
{
public:
    SuperChorusEditor(mrta::BaseProcessor&);
    ~SuperChorusEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    mrta::BaseProcessor& processor;

    mrta::GenericParameterEditor paramEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChorusEditor)
};