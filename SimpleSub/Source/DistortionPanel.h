/*
  ==============================================================================

    DistortionPanel.h
    Created: 27 Sep 2023 9:23:58pm
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ptnz_gui/ptnz_gui.h"
#include "WaveShaperDisplay.h"
#include "SimpleSubLookAndFeel.h"

//==============================================================================
/*
*/
class DistortionPanel  : public juce::Component
{
public:
    DistortionPanel(juce::AudioProcessorValueTreeState& apvts) :
    cubicButton("CUBIC", &lnf),
    tanhButton("TANH", &lnf),
    sinButton("SINE", &lnf),
    isActiveButton(apvts, param::toID(param::param_distortionIsActive), &lnf),
    wsd(),
    driveSlider(apvts, param::toID(param::param_drive), param::toName(param::param_drive), &lnf)
    {
        type = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(param::toID(param::param_distorionType)));
        jassert(type != nullptr);
        
        cubicButton.onClick = [&] () {
            tanhButton.setToggleState(false, juce::dontSendNotification);
            sinButton.setToggleState(false, juce::dontSendNotification);
            cubicButton.setEnabled(false);
            tanhButton.setEnabled(true);
            sinButton.setEnabled(true);
            wsd.update(static_cast<int>(param::CubicAlgo));
            type->setValueNotifyingHost((float)param::CubicAlgo / (param::NumAlgo - 1));
        };
        
        tanhButton.onClick = [&] () {
            cubicButton.setToggleState(false, juce::dontSendNotification);
            sinButton.setToggleState(false, juce::dontSendNotification);
            tanhButton.setEnabled(false);
            cubicButton.setEnabled(true);
            sinButton.setEnabled(true);
            wsd.update(static_cast<int>(param::TanhAlgo));
            type->setValueNotifyingHost((float)param::TanhAlgo / (param::NumAlgo - 1));
        };
        
        sinButton.onClick = [&] () {
            cubicButton.setToggleState(false, juce::dontSendNotification);
            tanhButton.setToggleState(false, juce::dontSendNotification);
            sinButton.setEnabled(false);
            tanhButton.setEnabled(true);
            cubicButton.setEnabled(true);
            wsd.update(static_cast<int>(param::SinAlgo));
            type->setValueNotifyingHost((float)param::SinAlgo / (param::NumAlgo - 1));
        };
        
        addAndMakeVisible(isActiveButton.button);
        addAndMakeVisible(cubicButton);
        addAndMakeVisible(tanhButton);
        addAndMakeVisible(sinButton);

        addAndMakeVisible(driveSlider.slider);

        addAndMakeVisible(wsd);
        wsd.update(type->getIndex());
            
        switch (type->getIndex()) {
            case param::CubicAlgo: 
                cubicButton.setToggleState(true, juce::dontSendNotification);
                cubicButton.onClick();
                break;
            case param::TanhAlgo:
                tanhButton.setToggleState(true, juce::dontSendNotification);
                tanhButton.onClick();
                break;
            case param::SinAlgo:
                sinButton.setToggleState(true, juce::dontSendNotification);
                sinButton.onClick();
                break;
            default:
                break;
        };
    
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(lnf.findColour(juce::Label::textColourId));
        g.setFont(juce::Font(17.f));
        g.drawFittedText("DRIVE", driveSlider.slider.getX(), 0, driveSlider.slider.getWidth(), 30, juce::Justification::centred ,1);
        //g.drawRoundedRectangle(wsd.getBounds().toFloat(), 2.f, 2.f);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto this_w = bounds.getWidth();
        
        auto xpad = this_w / 18;
        auto driveSliderBounds = bounds.removeFromLeft(this_w / 3).reduced(xpad, 20);
        driveSlider.slider.setBounds(driveSliderBounds);
        
        auto viewBounds = bounds.removeFromLeft(this_w / 3).reduced(xpad, 0);
        wsd.setBounds(viewBounds);
        
        auto size = 25;
        auto pad = 3;
        isActiveButton.button.setBounds(this_w - size - pad, pad, size, size);
        
        bounds.removeFromRight(xpad);
        bounds.reduce(xpad, (bounds.getHeight() - bounds.getWidth()));
        auto buttonSpacing = 9;
        auto buttonSize = (bounds.getHeight() - (2 * buttonSpacing)) / 3;
        
        cubicButton.setBounds(bounds.removeFromTop(buttonSize));
        bounds.removeFromTop(buttonSpacing);
        tanhButton.setBounds(bounds.removeFromTop(buttonSize));
        bounds.removeFromTop(buttonSpacing);
        sinButton.setBounds(bounds.removeFromTop(buttonSize));
    }

private:
    SimpleSubLookAndFeel lnf;

    juce::AudioParameterChoice* type;
    
    ptnz_gui::ToggleButtonWithText cubicButton;
    ptnz_gui::ToggleButtonWithText tanhButton;
    ptnz_gui::ToggleButtonWithText sinButton;
    
    ptnz_gui::AttachedOnOffButton isActiveButton;
    
    WaveShaperDisplay wsd;
    
    ptnz_gui::AttachedSlider driveSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionPanel)
};
