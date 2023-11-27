#pragma once
#include "ptnz_gui/ptnz_gui.h"
#include "Parameters.h"

namespace colours
{
const juce::Colour background = juce::Colour(0xff1c1b21);
const juce::Colour grey1 = juce::Colour(0xff3b3b4f);
const juce::Colour grey2 = juce::Colour(0xff0b0b0d);

const juce::Colour mainBlue = juce::Colour(0xff084696);
const juce::Colour secondaryBlue = juce::Colour(0xff0a5fcc);

const juce::Colour textColour = juce::Colours::whitesmoke;
}

class SimpleSubLookAndFeel : public ptnz_gui::LNF
{
public:
    SimpleSubLookAndFeel()
    : ptnz_gui::LNF()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
        //setColour(juce::Slider::trackColourId, juce::Colour(0xff2e5090));
        
        //track colour is the partially filled colour
        setColour(juce::Slider::trackColourId, colours::mainBlue);
        //
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::whitesmoke);
        //setColour(juce::Slider::backgroundColourId, juce::Colour(0xff084696));
        
        setColour(juce::Label::textColourId, colours::textColour);
        setColour(juce::Label::outlineColourId, colours::mainBlue);
        
        using c = ptnz_gui::colour_ids;
        
        setColour(c::buttonAccent, colours::mainBlue);
        
        setColour(c::accent, colours::mainBlue);
        setColour(c::secondaryAccent, colours::secondaryBlue);
        
        setColour(c::mainBackground, colours::background);
        setColour(c::secondaryBackground, colours::grey2);
        
        setColour(c::outlineColour, colours::grey1);
        setColour(c::white, colours::textColour);
    }
};
