/*
  ==============================================================================

    WheelUpLookAndFeel.h
    Created: 29 Nov 2023 11:01:16pm
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include "ptnz_gui/ptnz_gui.h"

namespace colours
{
const juce::Colour background = juce::Colour(0xff121114);
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
        setColour(c::accent, colours::accent);
    }
};
