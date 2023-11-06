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
//==============================================================================
/*
*/
class BottomPanel  : public juce::Component
{
public:
    BottomPanel(juce::AudioProcessorValueTreeState& apvts) :
    outGainSlider(apvts, param::toID(param::param_out_gain), param::toName(param::param_out_gain)),
    widthSlider(apvts, param::toID(param::param_width), param::toName(param::param_width))
    {
        for (auto i = 0; i < 5; i++) {
            param::PID pid = (param::PID) (param::param_harmonic_1 + i);
            jassert(pid < param::TotalNumParams);
            harmonicSliders[i] = std::make_unique<ptnz_gui::AttachedSliderVertical>(apvts, param::toID(pid), param::toName(pid));
            addAndMakeVisible(harmonicSliders[i]->slider);
        }
        
        addAndMakeVisible(outGainSlider.slider);
        addAndMakeVisible(widthSlider.slider);
    }

    ~BottomPanel() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(17.f));
        g.drawFittedText("WIDTH", widthSlider.slider.getX(), 0, widthSlider.slider.getWidth(), 30, juce::Justification::centred, 1);
        g.drawFittedText("OUT", outGainSlider.slider.getX(), 0, outGainSlider.slider.getWidth(), 30, juce::Justification::centred, 1);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto w = bounds.getWidth();
        
        auto harmonicBounds = bounds.removeFromLeft(w / 2).reduced(10, 5);
        auto delta = harmonicBounds.getWidth() / 5;
        
        for (auto& s : harmonicSliders)
        {
            s->slider.setBounds(harmonicBounds.removeFromLeft(delta).reduced(4, 0));
        }
        
        bounds.removeFromTop(20);
        
        widthSlider.slider.setBounds(bounds.removeFromLeft(w / 4).reduced(30, 0));
        outGainSlider.slider.setBounds(bounds.reduced(30, 0));
    }

private:
    ptnz_gui::AttachedSlider outGainSlider;
    ptnz_gui::AttachedSlider widthSlider;
    
    using uniqueSlider = std::unique_ptr<ptnz_gui::AttachedSliderVertical>;
    std::array<uniqueSlider, 5> harmonicSliders;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomPanel)
};
