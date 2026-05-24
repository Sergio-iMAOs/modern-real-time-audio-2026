#include "SuperChorusEditor.h"


// Width of the whole GUI
static constexpr int WIDTH { 250 };

// Height of each paramter knob on the paramEditor
static const int PARAM_HEIGHT { 60 };

SuperChorusEditor::SuperChorusEditor(mrta::BaseProcessor& p) :
    juce::AudioProcessorEditor(p),
    processor { p },
    paramEditor(processor.getParameterManager(), PARAM_HEIGHT)
{
    addAndMakeVisible(paramEditor);

    // Calculate window height based on number of parameters
    const auto height { processor.getParameterManager().getParameters().size() * PARAM_HEIGHT };
    setSize(WIDTH, height);
}

SuperChorusEditor::~SuperChorusEditor()
{
}

void SuperChorusEditor::paint(juce::Graphics&)
{
}

void SuperChorusEditor::resized()
{
    paramEditor.setBounds(getLocalBounds());
}