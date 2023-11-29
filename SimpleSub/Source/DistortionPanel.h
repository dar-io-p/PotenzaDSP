#pragma once

#include <JuceHeader.h>
#include "ptnz_gui/ptnz_gui.h"
#include "WaveShaperDisplay.h"
#include "SimpleSubLookAndFeel.h"


class DistortionPanel  : public juce::Component
{
public:
    DistortionPanel(juce::AudioProcessorValueTreeState& apvts) :
    cubicButton (apvts, param::toID(param::param_distorionType), "cubic", 0, &lnf),
    tanhButton  (apvts, param::toID(param::param_distorionType), "tanh", 1, &lnf),
    sinButton   (apvts, param::toID(param::param_distorionType), "sine", 2, &lnf),
    wsd(apvts)
    {
        addAndMakeVisible(cubicButton.button);
        addAndMakeVisible(tanhButton.button);
        addAndMakeVisible(sinButton.button);

        addAndMakeVisible(wsd);
    }

    void paint (juce::Graphics& g) override
    {

    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto this_w = bounds.getWidth();
                
        auto viewBounds = bounds.removeFromLeft(this_w * 4 / 9);
        wsd.setBounds(viewBounds);
        
        bounds = bounds.removeFromRight(this_w * 4 / 9);
        
        auto d = bounds.getWidth();
        auto buttonBounds = juce::Rectangle<int>(0, 0, d, d);
        buttonBounds = buttonBounds.withCentre(bounds.getCentre());
        
        auto buttonSpacing = 5;
        auto buttonSize = (buttonBounds.getHeight() - (2 * buttonSpacing)) / 3;
        
        
        cubicButton.button.setBounds(buttonBounds.removeFromTop(buttonSize));
        buttonBounds.removeFromTop(buttonSpacing);
        tanhButton.button.setBounds(buttonBounds.removeFromTop(buttonSize));
        buttonBounds.removeFromTop(buttonSpacing);
        sinButton.button.setBounds(buttonBounds.removeFromTop(buttonSize));
    }

private:
    SimpleSubLookAndFeel lnf;
    
    AttachedChoiceButton cubicButton;
    AttachedChoiceButton tanhButton;
    AttachedChoiceButton sinButton;
    
    //ptnz_gui::AttachedOnOffButton isActiveButton;
    
    WaveShaperDisplay wsd;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionPanel)
};
