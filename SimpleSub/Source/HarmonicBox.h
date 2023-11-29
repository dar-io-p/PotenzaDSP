/*
  ==============================================================================

    HarmonicBox.h
    Created: 29 Nov 2023 5:48:22pm
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ptnz_gui/ptnz_gui.h"
class HarmonicBox  : public juce::Component
{
public:
    HarmonicBox(juce::AudioProcessorValueTreeState& apvts)
    {
        for (auto i = 0; i < 5; i++) {
            param::PID pid = (param::PID) (param::param_harmonic_1 + i);
            jassert(pid < param::TotalNumParams);
            harmonicSliders[i] = std::make_unique<ptnz_gui::AttachedSliderVertical>(apvts, param::toID(pid), param::toName(pid), &lnf);
            addAndMakeVisible(harmonicSliders[i]->slider);
        }
    }

    ~HarmonicBox() override
    {
    }

    void paint (juce::Graphics& g) override
    {
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto delta = bounds.getWidth() / 5;
        
        for (auto& s : harmonicSliders)
        {
            s->slider.setBounds(bounds.removeFromLeft(delta).reduced(3, 0));
        }
    }

private:
    SimpleSubLookAndFeel lnf;
    using uniqueSlider = std::unique_ptr<ptnz_gui::AttachedSliderVertical>;
    std::array<uniqueSlider, 5> harmonicSliders;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonicBox)
};
