#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <ParameterComponents.h>
#include "SuperChorusProcessor.h"
#include "SuperChorusLAF.h"


class SuperChorusEditor final : public juce::AudioProcessorEditor,
                                public juce::Timer
{
public:
    SuperChorusEditor(SuperChorusProcessor&);
    ~SuperChorusEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SuperChorusProcessor& processor;
    DSP::SuperChorus& superChorus;

    juce::Rectangle<int> displayBounds;
    mrta::ParameterSlider delaySlider;
    mrta::ParameterSlider smearSlider;
    mrta::ParameterSlider widthSlider;
    mrta::ParameterSlider modDepthSlider;
    mrta::ParameterSlider modRateSlider;
    mrta::ParameterComboBox modTypeComboBox;
    mrta::ParameterSlider driveSlider;
    mrta::ParameterSlider bitDepthSlider;
    mrta::ParameterComboBox distTypeComboBox;
    mrta::ParameterSlider voicesSlider;
    mrta::ParameterSlider mixSlider;
    mrta::ParameterButton enabledButton;

    juce::Label delayLabel;
    juce::Label smearLabel;
    juce::Label widthLabel;
    juce::Label modDepthLabel;
    juce::Label modRateLabel;
    juce::Label modTypeLabel;
    juce::Label driveLabel;
    juce::Label bitDepthLabel;
    juce::Label distTypeLabel;
    juce::Label voicesLabel;
    juce::Label mixLabel;
    juce::Label enabledLabel;

    SuperChorusLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SuperChorusEditor)
};