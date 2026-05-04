#include "PluginEditor.h"
#include "PluginProcessor.h"

PluginEditor::PluginEditor(mrta::BaseProcessor &p) :
    juce::AudioProcessorEditor(p),
    processor { p },
    ringModParameterEditor(processor.getParameterManager(), DEF_HEIGHT,
                         { Param::ID::Freq })
{
    addAndMakeVisible(ringModParameterEditor);

    setSize(DEF_WIDTH, DEF_HEIGHT);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint(juce::Graphics&)
{
}

void PluginEditor::resized()
{
    auto localBounds { getLocalBounds() };
    ringModParameterEditor.setBounds(localBounds);
}

