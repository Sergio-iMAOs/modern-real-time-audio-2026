#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SuperChorusLAF : public juce::LookAndFeel_V4
{
public:
    SuperChorusLAF();

    virtual void drawPluginBackground(
        juce::Graphics& g,
        int width,
        int height
    );
    virtual void drawDisplay(
        juce::Graphics& g,
        juce::Rectangle<int> bounds,
        std::vector<float> timePos,
        std::vector<float> stereoPos,
        float drive,
        float bitDepth,
        bool softClip
    );


    // Juce methods
    void drawRotarySlider(
        juce::Graphics &g,
        int x,
        int y,
        int width,
        int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider &slider
    ) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;
    juce::Label* createSliderTextBox(juce::Slider& slider) override;

    void drawButtonBackground(
        juce::Graphics &g,
        juce::Button &button,
        const juce::Colour &backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;

    void drawComboBox(
        juce::Graphics &g,
        int	width,
        int	height,
        bool isButtonDown,
        int	buttonX,
        int	buttonY,
        int	buttonW,
        int	buttonH,
        juce::ComboBox &cb
    ) override;
    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;
    

private:
    juce::Colour colour0 { 0xffffffff }; //  #ffffff
    juce::Colour colour1 { 0xffecffff }; //  #ecffff
    juce::Colour colour2 { 0xff74c1c1 }; //  #74c1c1
    juce::Colour colour3 { 0xff548b8b }; //  #548b8b
    juce::Colour colour4 { 0xffc17474 }; //  #c17474
    juce::Colour colour5 { 0xffe9b32a }; //  #e9b32a
    juce::Colour colour6 { 0xffb40000 }; //  #b40000
};