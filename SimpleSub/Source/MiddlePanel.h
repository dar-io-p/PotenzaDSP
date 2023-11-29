#pragma once

#include <JuceHeader.h>
#include "Parameters.h"
#include "ptnz_gui/ptnz_gui.h"
#include "DistortionPanel.h"
#include "SimpleSubLookAndFeel.h"

//==============================================================================
/*
*/
class MiddlePanel  : public juce::Component
{
public:
    MiddlePanel(juce::AudioProcessorValueTreeState& apvts) :
    widthSlider(apvts, param::toID(param::param_width), param::toName   (param::param_width), &lnf),
    driveSlider(apvts, param::toID(param::param_drive), param::toName   (param::param_drive), &lnf),
    distPanel(apvts),
    distActiveButton(apvts, param::toID(param::param_distortionIsActive), &lnf)
    {
        addAndMakeVisible(widthSlider.slider);
        addAndMakeVisible(driveSlider.slider);
        addAndMakeVisible(distPanel);
        addAndMakeVisible(distActiveButton.button);
        
        distActiveButton.button.onClick = [this] () {repaint();};
    }

    ~MiddlePanel() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::white));
        
        g.setFont(ptnz_gui::styles::getTitleFont());
        drawSliderTitle(g, widthSlider.slider, "WIDTH");
        
        if (!distActiveButton.button.getToggleState()) g.setColour(lnf.findColour(ptnz_gui::colour_ids::white).withAlpha(0.2f));
        drawSliderTitle(g, driveSlider.slider, "DRIVE");
        
        drawDivider(g, getLocalBounds().toFloat(), getWidth() / 4);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto this_w = bounds.getWidth();
        
        auto widthBounds = bounds.removeFromLeft(this_w / 4);
        placeSlider(widthSlider.slider, widthBounds);
        auto driveBounds = bounds.removeFromLeft(this_w / 4);
        placeSlider(driveSlider.slider, driveBounds);
        
        auto onOffSpacing = 30;
        distPanel.setBounds(bounds.removeFromLeft((this_w / 2) - onOffSpacing));
        
        distActiveButton.button.setBounds(bounds.removeFromTop(onOffSpacing).reduced(5, 5));
    }

private:
    SimpleSubLookAndFeel lnf;
    ptnz_gui::AttachedSlider widthSlider;
    ptnz_gui::AttachedSlider driveSlider;
    DistortionPanel distPanel;
    
    ptnz_gui::AttachedOnOffButton distActiveButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiddlePanel)
};
