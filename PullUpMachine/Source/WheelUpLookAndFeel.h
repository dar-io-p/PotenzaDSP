/*
  ==============================================================================

    WheelUpLookAndFeel.h
    Created: 29 Nov 2023 11:01:16pm
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include "ptnz_gui/ptnz_gui.h"

namespace styles
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
static const juce::Font getPlainFont()     {
    return juce::Font(getTypeface()).withHeight(12.f);
}
static const juce::Font getBigTitleFont()  {
    return juce::Font(getTypeface()).withHeight(25.f);
}

}

namespace colours
{
const juce::Colour background = juce::Colour(0xff121114);
const juce::Colour secondaryBG = juce::Colour(0xff414042);
const juce::Colour thirdBG = juce::Colour(0xff1a191c);
const juce::Colour accent = juce::Colour(0xff582bfc);
}

class WheelUpLookAndFeel : public ptnz_gui::LNF
{
public:
    WheelUpLookAndFeel() : ptnz_gui::LNF()
    {
        setColour(juce::Slider::trackColourId, colours::accent);
        
        using c = ptnz_gui::colour_ids;
        setColour(c::mainBackground, colours::background);
        setColour(c::secondaryBackground, colours::secondaryBG);
        setColour(c::accent, colours::accent);
        
        juce::LookAndFeel::setDefaultLookAndFeel(this);
        setDefaultSansSerifTypeface(styles::getTypeface());
    }
    
    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle sliderStyle, juce::Slider &slider) override
    {
        auto thumbHeight = 10;
        auto bounds = slider.getLocalBounds();
        auto pad = thumbHeight / 2;
        
        auto centreX = bounds.getCentreX();
        
        auto numTicks = 10;
        float delta = (float)(height-thumbHeight) / (float)(numTicks-1);
        
        g.setColour(findColour(ptnz_gui::colour_ids::secondaryBackground));
        for (int i = 0; i < numTicks; i++) {
            g.drawLine(x, (int)(pad + i*delta), width, (int)(pad + i*delta), 2.f);
        }
        
        g.setColour(juce::Colours::black);
        g.drawLine(centreX, y , centreX, height, 3.f);
        
        auto thumb_y = juce::jmap(sliderPos, 0.f, (float)height, (float)pad, (float)height-pad);
        auto thumbPad = 3;
        auto thumbBounds = juce::Rectangle<float>(0.0f, 0, width - thumbPad*2, thumbHeight).withCentre({(float)centreX, thumb_y});
        
        g.setColour(findColour(ptnz_gui::colour_ids::accent));
        g.fillRoundedRectangle(thumbBounds, 1.5f);
        g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
        g.drawLine(thumbPad, thumb_y, width-thumbPad, thumb_y, 1.5f);
    }
    
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font &) override
    {
        return styles::getTypeface();
    }
};

struct AttachedFader
{
    AttachedFader(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& name, juce::LookAndFeel_V4* custom_lnf=nullptr) :
        slider(),
        attachment(*apvts.getParameter(iD), slider)
    {
        if(custom_lnf)
            slider.setLookAndFeel(custom_lnf);
        else
            slider.setLookAndFeel(&lnf);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setSliderStyle(juce::Slider::LinearVertical);
    }
    ~AttachedFader()
    {
        slider.setLookAndFeel(nullptr);
    }
    WheelUpLookAndFeel lnf;
    juce::Slider slider;
    juce::SliderParameterAttachment attachment;
};
