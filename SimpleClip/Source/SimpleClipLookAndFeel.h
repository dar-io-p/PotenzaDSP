/*
  ==============================================================================

    SimpleClipLookAndFeel.h
    Created: 4 Oct 2023 11:59:36am
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include "ptnz_gui/ptnz_gui.h"

namespace cols
{
const juce::Colour mainPurple = juce::Colour(0xff7575E6);
const juce::Colour white = juce::Colour(0xffffffff);
}

class SimpleClipLookAndFeel : public ptnz_gui::LNF
{
public:
    SimpleClipLookAndFeel()
    : ptnz_gui::LNF()
    {
        setColour(juce::Slider::trackColourId, cols::mainPurple);
        setColour(juce::Slider::rotarySliderOutlineColourId, cols::white);
        
        setColour(juce::Slider::backgroundColourId, cols::mainPurple);
        
        setColour(juce::Label::outlineColourId, cols::mainPurple);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        g.setColour(juce::Colours::white);
        //g.drawRoundedRectangle(slider.getLocalBounds().reduced(2).toFloat(), 2.f, 2.f);
        auto ypad = height / 10;
        auto xpad = width / 10;
        g.drawLine(xpad, ypad, width-xpad, ypad, 2.0f);
        g.drawLine(xpad, height - ypad, width-xpad, height - ypad, 2.0f);
        g.setFont(juce::Font(ypad * 3));
        g.drawFittedText(slider.getTextFromValue(slider.getValue()), x, y, width, height, juce::Justification::centred, 2);
    }
};

class InputMeterLNF : public ptnz_gui::LNF
{
public:
    InputMeterLNF()
    : ptnz_gui::LNF()
    {
        setColour(juce::Slider::trackColourId, cols::mainPurple);
        setColour(juce::Slider::rotarySliderOutlineColourId, cols::white);
        
        setColour(juce::Slider::backgroundColourId, cols::mainPurple);
        
        setColour(juce::Label::outlineColourId, cols::mainPurple);
    }
    
    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle sliderStyle, juce::Slider &slider) override
    {
        g.drawRoundedRectangle(x, y, width, height, 2.0f, 1.0f);
        g.drawFittedText(slider.getTextFromValue(slider.getValue()), x, y, width, height, juce::Justification::centred, 1);
    }
};

class OutputMeterLNF : public ptnz_gui::LNF
{
public:
    OutputMeterLNF()
    : ptnz_gui::LNF()
    {
        setColour(juce::Slider::trackColourId, cols::mainPurple);
        setColour(juce::Slider::rotarySliderOutlineColourId, cols::white);
        
        setColour(juce::Slider::backgroundColourId, cols::mainPurple);
        
        setColour(juce::Label::outlineColourId, cols::mainPurple);
    }
    
    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle sliderStyle, juce::Slider &slider) override
    {
        
    }
};
