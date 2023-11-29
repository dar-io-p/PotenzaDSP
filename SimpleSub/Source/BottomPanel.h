/*
  ==============================================================================

    BottomPanel.h
    Created: 26 Sep 2023 11:09:28am
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ptnz_gui/ptnz_gui.h"
#include "Parameters.h"
#include "SimpleSubLookAndFeel.h"
#include "HarmonicBox.h"
//==============================================================================
/*
*/
class BottomPanel  : public juce::Component
{
public:
    BottomPanel(juce::AudioProcessorValueTreeState& apvts) :
    outGainSlider(apvts, param::toID(param::param_out_gain), param::toName(param::param_out_gain), &lnf),
    glideSlider(apvts, param::toID(param::param_glide), param::toName(param::param_width), &lnf),
    glideActiveButton(apvts, param::toID(param::param_glide_active), &lnf),
    hBox(apvts)
    {

        addAndMakeVisible(outGainSlider.slider);
        addAndMakeVisible(glideSlider.slider);
        
        addAndMakeVisible(glideActiveButton.button);
        
        addAndMakeVisible(hBox);
        
        glideActiveButton.button.onClick = [this] () {repaint();};
    }

    ~BottomPanel() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::white));
        g.setFont(ptnz_gui::styles::getTitleFont());
        
        g.drawFittedText("Harmonics", hBox.getX(), hBox.getY() - 23 , hBox.getWidth(), 20, juce::Justification::centred, 1);
        
        drawSliderTitle(g, outGainSlider.slider, "OUT");
        
        if(!glideActiveButton.button.getToggleState()) g.setColour(lnf.findColour(ptnz_gui::colour_ids::white).withAlpha(0.2f));
        drawSliderTitle(g, glideSlider.slider, "GLIDE");
        
        
        const auto bounds = getLocalBounds().toFloat();
        drawDivider(g, bounds, (int)(0.5f * getWidth()));
        drawDivider(g, bounds, (int)(0.75f * getWidth()));
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto w = bounds.getWidth();
        
        auto harmonicBounds = bounds.removeFromLeft(w / 2).reduced(10, 10);
        harmonicBounds.removeFromTop(15);
        
        hBox.setBounds(harmonicBounds);
        
        auto buttonRect = juce::Rectangle<int>(bounds.getX() + 8, 8 , 20, 20);

        auto glideBounds = bounds.removeFromLeft(w / 4);
        placeSlider(glideSlider.slider, glideBounds);
        
        placeSlider(outGainSlider.slider, bounds);
        
        glideActiveButton.button.setBounds(buttonRect);
    }

private:
    SimpleSubLookAndFeel lnf;

    ptnz_gui::AttachedSlider outGainSlider;
    ptnz_gui::AttachedSlider glideSlider;
    ptnz_gui::AttachedOnOffButton glideActiveButton;
    
    HarmonicBox hBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomPanel)
};
