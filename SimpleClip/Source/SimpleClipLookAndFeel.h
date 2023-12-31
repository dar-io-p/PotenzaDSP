/*
  ==============================================================================

    SimpleClipLookAndFeel.h
    Created: 4 Oct 2023 11:59:36am
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include "ptnz_gui/ptnz_gui.h"

namespace fonts
{
    //const juce::Font labelFont  = juce::Font("Futura", 14.f, juce::Font::plain);
static const juce::Typeface::Ptr getTypeface()
{
    static auto typeface = juce::Typeface::createSystemTypefaceFor (BinaryData::Futura_Heavy_font_ttf, BinaryData::Futura_Heavy_font_ttfSize);
    return typeface;
}

//static const juce::Font getLabelFont()     {
//    return juce::Font(getTypeface()).withHeight(14.f);
//}
//static const juce::Font getTitleFont()     {
//    return juce::Font(getTypeface()).withHeight(16.f);
//}
//static const juce::Font getPlainFont()     {
//    return juce::Font(getTypeface()).withHeight(12.f);
//}
//static const juce::Font getBigTitleFont()  {
//    return juce::Font(getTypeface()).withHeight(25.f);
//}
}

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
        
        setColour(ptnz_gui::accent, cols::mainPurple);
        
        juce::LookAndFeel::setDefaultLookAndFeel(this);
        setDefaultSansSerifTypeface(fonts::getTypeface());
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


