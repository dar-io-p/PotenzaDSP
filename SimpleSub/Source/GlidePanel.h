/*
  ==============================================================================

    GlidePanel.h
    Created: 28 Sep 2023 12:56:11pm
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ptnz_gui/ptnz_gui.h"
#include "Parameters.h"
#include "SimpleSubLookAndFeel.h"
//==============================================================================
/*
*/
class GlidePanel  : public juce::Component
{
public:
    GlidePanel(juce::AudioProcessorValueTreeState& apvts) :
    glideSlider(apvts, param::toID(param::param_glide), param::toName(param::param_glide), &lnf),
    button(apvts, param::toID(param::param_glide_active), &lnf)
    {
        addAndMakeVisible(glideSlider.slider);
        addAndMakeVisible(button.button);
    }

    void paint (juce::Graphics& g) override
    {
        auto buttonHeight = 25;
        g.setColour(lnf.findColour(juce::Label::textColourId));
        g.setFont(juce::Font(17.f));
        g.drawFittedText("GLIDE", 0, 0, getWidth(), buttonHeight, juce::Justification::centred , 1);
    }

    void resized() override
    {
        auto buttonHeight = 25;
        auto sliderBounds = getLocalBounds().reduced(buttonHeight, buttonHeight);
        glideSlider.slider.setBounds(sliderBounds);
        
        button.button.setBounds(getRight() - buttonHeight, 0, buttonHeight, buttonHeight);
    }

private:
    SimpleSubLookAndFeel lnf;

    ptnz_gui::AttachedSlider glideSlider;
    ptnz_gui::AttachedOnOffButton button;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlidePanel)
};
