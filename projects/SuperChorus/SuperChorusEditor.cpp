#include "SuperChorusEditor.h"


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
    mixLabel        { "", Param::Name::Mix },
    enabledLabel    { "", Param::Name::Enabled }
{
    // Set button text
    enabledButton.setButtonText(Param::Name::Enabled);

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
    addAndMakeVisible(enabledLabel);

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

    laf.drawDisplay(
        g,
        displayBounds,
        superChorus.getNormalizedTimePositions(),
        superChorus.getNormalizedStereoPositions(),
        static_cast<float>(driveSlider.getValue() / driveSlider.getMaximum()),
        static_cast<float>(bitDepthSlider.getValue() / bitDepthSlider.getMaximum())
    );
}

void SuperChorusEditor::resized()
{
    auto bounds = getLocalBounds();

    // Prepare grid
    const auto componentWidth   = bounds.getWidth() / 4;
    const auto componentHeight  = bounds.getHeight() / 6; 
    const auto labelHeight      = componentHeight / 4;
    const auto marginX        = 6;
    const auto marginY        = 12;

    auto rightBounds = bounds.removeFromRight(componentWidth);
    auto& leftBounds = bounds;

    // Left

    // Display
    displayBounds = leftBounds.removeFromTop(componentHeight * 4);

    // Top row
    auto topBounds      = leftBounds.removeFromTop(componentHeight);
    auto delayBounds    = topBounds.removeFromLeft(componentWidth);
    auto smearBounds    = topBounds.removeFromLeft(componentWidth);
    auto& widthBounds   = topBounds;

    delayLabel .setBounds(delayBounds.removeFromTop(labelHeight));
    smearLabel .setBounds(smearBounds.removeFromTop(labelHeight));
    widthLabel .setBounds(widthBounds.removeFromTop(labelHeight));
    delaySlider.setBounds(delayBounds);
    smearSlider.setBounds(smearBounds);
    widthSlider.setBounds(widthBounds);

    // Bottom row
    auto& bottomBounds  = leftBounds;
    auto modDepthBounds = bottomBounds.removeFromLeft(componentWidth);
    auto modRateBounds  = bottomBounds.removeFromLeft(componentWidth);
    auto modTypeBounds  = bottomBounds.removeFromLeft(componentWidth);

    modDepthLabel   .setBounds(modDepthBounds   .removeFromTop(labelHeight));
    modRateLabel    .setBounds(modRateBounds    .removeFromTop(labelHeight));
    modTypeLabel    .setBounds(modTypeBounds    .removeFromTop(labelHeight));
    modDepthSlider  .setBounds(modDepthBounds);
    modRateSlider   .setBounds(modRateBounds);
    modTypeComboBox .setBounds(modTypeBounds    .reduced(marginX, marginY));

    // Right
    enabledButton.setBounds(rightBounds.removeFromTop(componentHeight).reduced(marginX, marginY));
    auto driveBounds      = rightBounds.removeFromTop(componentHeight);
    auto bitDepthBounds   = rightBounds.removeFromTop(componentHeight);
    auto distTypeBounds   = rightBounds.removeFromTop(componentHeight);
    auto voicesBounds     = rightBounds.removeFromTop(componentHeight);
    auto& mixBounds       = rightBounds;

    driveLabel      .setBounds(driveBounds      .removeFromTop(labelHeight));
    bitDepthLabel   .setBounds(bitDepthBounds   .removeFromTop(labelHeight));
    distTypeLabel   .setBounds(distTypeBounds   .removeFromTop(labelHeight));
    voicesLabel     .setBounds(voicesBounds     .removeFromTop(labelHeight));
    mixLabel        .setBounds(mixBounds        .removeFromTop(labelHeight));
    driveSlider     .setBounds(driveBounds);
    bitDepthSlider  .setBounds(bitDepthBounds);
    distTypeComboBox.setBounds(distTypeBounds   .reduced(marginX, marginY));
    voicesSlider    .setBounds(voicesBounds);
    mixSlider       .setBounds(mixBounds);
}

void SuperChorusEditor::timerCallback()
{
    repaint();
}
