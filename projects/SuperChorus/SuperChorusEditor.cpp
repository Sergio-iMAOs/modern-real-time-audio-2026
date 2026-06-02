#include "SuperChorusEditor.h"
#include <SuperChorus.h>

// Dimensions of the whole GUI
constexpr int WIDTH  { 600 };
constexpr int HEIGHT { 480 };
constexpr float DISPLAY_RATE_HZ { 30.f};

SuperChorusEditor::SuperChorusEditor(SuperChorusProcessor& p) :
    juce::AudioProcessorEditor(p),
    processor   { p },
    superChorus { p.getSuperChorus() },

    // Initialize components
    delaySlider     ( Param::ID::Offset     , processor.getParameterManager().getAPVTS() ),
    smearSlider     ( Param::ID::Smear      , processor.getParameterManager().getAPVTS() ),
    widthSlider     ( Param::ID::Width      , processor.getParameterManager().getAPVTS() ),
    modDepthSlider  ( Param::ID::Depth      , processor.getParameterManager().getAPVTS() ),
    modRateSlider   ( Param::ID::Rate       , processor.getParameterManager().getAPVTS() ),
    modTypeComboBox ( Param::ID::ModType    , processor.getParameterManager().getAPVTS() ),
    driveSlider     ( Param::ID::Distort    , processor.getParameterManager().getAPVTS() ),
    bitDepthSlider  ( Param::ID::BitDepth   , processor.getParameterManager().getAPVTS() ),
    distTypeComboBox( Param::ID::DistType   , processor.getParameterManager().getAPVTS() ),
    voicesSlider    ( Param::ID::Voices     , processor.getParameterManager().getAPVTS() ),
    mixSlider       ( Param::ID::Mix        , processor.getParameterManager().getAPVTS() ),
    enabledButton   ( Param::ID::Enabled    , processor.getParameterManager().getAPVTS() ),

    // Set labels text
    delayLabel      { "", Param::Name::Offset },
    smearLabel      { "", Param::Name::Smear },
    widthLabel      { "", Param::Name::Width },
    modDepthLabel   { "", Param::Name::Depth },
    modRateLabel    { "", Param::Name::Rate },
    modTypeLabel    { "", Param::Name::ModType },
    driveLabel      { "", Param::Name::Distort },
    bitDepthLabel   { "", Param::Name::BitDepth },
    distTypeLabel   { "", Param::Name::DistType },
    voicesLabel     { "", Param::Name::Voices },
    mixLabel        { "", Param::Name::Mix }
{
    modTypeLabel    .setJustificationType(juce::Justification::bottomLeft);
    distTypeLabel   .setJustificationType(juce::Justification::bottomLeft);

    // Add components to view
    addAndMakeVisible(delaySlider);
    addAndMakeVisible(smearSlider);
    addAndMakeVisible(widthSlider);
    addAndMakeVisible(modDepthSlider);
    addAndMakeVisible(modRateSlider);
    addAndMakeVisible(modTypeComboBox);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(bitDepthSlider);
    addAndMakeVisible(distTypeComboBox);
    addAndMakeVisible(voicesSlider);
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(enabledButton);
    addAndMakeVisible(delayLabel);
    addAndMakeVisible(smearLabel);
    addAndMakeVisible(widthLabel);
    addAndMakeVisible(modDepthLabel);
    addAndMakeVisible(modRateLabel);
    addAndMakeVisible(modTypeLabel);
    addAndMakeVisible(driveLabel);
    addAndMakeVisible(bitDepthLabel);
    addAndMakeVisible(distTypeLabel);
    addAndMakeVisible(voicesLabel);
    addAndMakeVisible(mixLabel);

    // Set look and feel
    setLookAndFeel(&laf);
    
    // Start timer
    startTimerHz(DISPLAY_RATE_HZ);

    // Set dimensions
    setSize(WIDTH, HEIGHT);
}

SuperChorusEditor::~SuperChorusEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void SuperChorusEditor::paint(juce::Graphics& g)
{
    laf.drawPluginBackground(
        g,
        getLocalBounds().getWidth(),
        getLocalBounds().getHeight()
    );

    // Get values from sliders, not directly from DSP
    const auto normalizedDrive = static_cast<float>(
        (driveSlider.getValue()   - driveSlider.getMinimum()) / \
        (driveSlider.getMaximum() - driveSlider.getMinimum())
    );
    const auto normalizedBitDepth = static_cast<float>(
        (bitDepthSlider.getValue()   - bitDepthSlider.getMinimum()) / \
        (bitDepthSlider.getMaximum() - bitDepthSlider.getMinimum())
    );

    laf.drawDisplay(
        g,
        displayBounds,
        superChorus.getNormalizedTimePositions(),
        superChorus.getNormalizedStereoPositions(),
        normalizedDrive,
        normalizedBitDepth,
        distTypeComboBox.getSelectedItemIndex() == DSP::SuperChorus::DistortionType::SoftClip,
        enabledButton.getToggleState()
    );

    laf.drawTitle(g, titleBounds);
}

void SuperChorusEditor::resized()
{
    auto bounds = getLocalBounds();

    // Prepare grid
    const auto componentWidth   = bounds.getWidth() / 4;
    const auto componentHeight  = bounds.getHeight() / 6; 
    const auto labelHeight      = componentHeight / 4;
    const auto marginX          = 6;
    const auto marginY          = 8;
    const auto sliderTextWidth  = mixSlider.getTextBoxWidth();

    auto rightBounds    = bounds.removeFromRight(componentWidth);
    auto& leftBounds    = bounds;

    // Left

    // Title
    titleBounds    = leftBounds.removeFromTop(componentHeight);

    // Display
    displayBounds       = leftBounds.removeFromTop(componentHeight * 3);

    // Top row
    auto topBounds      = leftBounds.removeFromTop(componentHeight);
    auto delayBounds    = topBounds.removeFromLeft(componentWidth);
    auto smearBounds    = topBounds.removeFromLeft(componentWidth);
    auto& widthBounds   = topBounds;

    delaySlider.setBounds(delayBounds);
    smearSlider.setBounds(smearBounds);
    widthSlider.setBounds(widthBounds);
    delayLabel .setBounds(delayBounds.removeFromRight(sliderTextWidth));
    smearLabel .setBounds(smearBounds.removeFromRight(sliderTextWidth));
    widthLabel .setBounds(widthBounds.removeFromRight(sliderTextWidth));

    // Bottom row
    auto& bottomBounds  = leftBounds;
    auto modDepthBounds = bottomBounds.removeFromLeft(componentWidth);
    auto modRateBounds  = bottomBounds.removeFromLeft(componentWidth);
    auto modTypeBounds  = bottomBounds.removeFromLeft(componentWidth);

    modDepthSlider  .setBounds(modDepthBounds);
    modRateSlider   .setBounds(modRateBounds);
    modDepthLabel   .setBounds(modDepthBounds   .removeFromRight(sliderTextWidth));
    modRateLabel    .setBounds(modRateBounds    .removeFromRight(sliderTextWidth));
    modTypeLabel    .setBounds(modTypeBounds    .removeFromTop(labelHeight));
    modTypeComboBox .setBounds(modTypeBounds    .reduced(marginX, marginY));

    // Right
    enabledButton.setBounds(rightBounds.removeFromTop(componentHeight).reduced(marginX, marginY));
    auto driveBounds      = rightBounds.removeFromTop(componentHeight);
    auto bitDepthBounds   = rightBounds.removeFromTop(componentHeight);
    auto distTypeBounds   = rightBounds.removeFromTop(componentHeight);
    auto voicesBounds     = rightBounds.removeFromTop(componentHeight);
    auto& mixBounds       = rightBounds;

    driveSlider     .setBounds(driveBounds);
    bitDepthSlider  .setBounds(bitDepthBounds);
    distTypeLabel   .setBounds(distTypeBounds   .removeFromTop(labelHeight));
    distTypeComboBox.setBounds(distTypeBounds   .reduced(marginX, marginY));
    voicesSlider    .setBounds(voicesBounds);
    mixSlider       .setBounds(mixBounds);
    driveLabel      .setBounds(driveBounds      .removeFromRight(sliderTextWidth));
    bitDepthLabel   .setBounds(bitDepthBounds   .removeFromRight(sliderTextWidth));
    voicesLabel     .setBounds(voicesBounds     .removeFromRight(sliderTextWidth));
    mixLabel        .setBounds(mixBounds        .removeFromRight(sliderTextWidth));
}

void SuperChorusEditor::timerCallback()
{
    repaint();
}