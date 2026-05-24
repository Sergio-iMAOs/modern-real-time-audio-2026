#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SuperChorusLAF : public juce::LookAndFeel_V4
{
public:
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
        float bitDepth
    );


private:
    juce::Colour colour0 { 0xffecffff }; //  #ecffff
    juce::Colour colour1 { 0xff74c1c1 }; //  #74c1c1
    juce::Colour colour2 { 0xff265353 }; //  #265353
    juce::Colour colour3 { 0xffc17474 }; //  #c17474
    juce::Colour colour4 { 0xffe9b32a }; //  #e9b32a
    juce::Colour colour5 { 0xffb40000 }; //  #b40000
};