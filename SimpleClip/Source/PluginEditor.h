/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveShaper.h"
#include "StereoLevelMeter.h"
#include "ShaperView.h"
#include "consts.h"
#include "ptnz_gui/ptnz_gui.h"
#include "SimpleClipLookAndFeel.h"

struct AttachedSlider
{
    AttachedSlider(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& name) :
        slider(),
        attachment(*apvts.getParameter(iD), slider)
    {
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    }
    ~AttachedSlider()
    {
    }
    juce::Slider slider;
    juce::SliderParameterAttachment attachment;
};

class SimpleClipAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleClipAudioProcessorEditor (SimpleClipAudioProcessor&);
    ~SimpleClipAudioProcessorEditor() override;
    
    void paint (juce::Graphics&) override;
    void paintOverChildren (juce::Graphics& g) override;
    void resized() override;
    
    void setLookAndFeels()
    {
        inputGainSlider.slider.setLookAndFeel(&lnf);
        outPutGainSlider.slider.setLookAndFeel(&lnf);
//        
        oversampleButton.button.setLookAndFeel(&lnf);
        
        threshSlider.slider.setLookAndFeel(&lnf);
        kneeSlider.slider.setLookAndFeel(&lnf);
    }
    
    void unsetLookAndFeel()
    {
        inputGainSlider.slider.setLookAndFeel(nullptr);
        outPutGainSlider.slider.setLookAndFeel(nullptr);
//        
       oversampleButton.button.setLookAndFeel(nullptr);
//        

        threshSlider.slider.setLookAndFeel(nullptr);
        kneeSlider.slider.setLookAndFeel(nullptr);
    }
private:
    SimpleClipLookAndFeel lnf;
    SimpleClipAudioProcessor& audioProcessor;
    
//    ptnz_gui::AttachedSlider aKneeSlider,       aThreshSlider;
//    ptnz_gui::AttachedSlider inputGainSlider,   outPutGainSlider;
//    
//    juce::Label inPeakLabel, outPeakLabel;
    
    AttachedSlider threshSlider, kneeSlider;
    AttachedSlider inputGainSlider, outPutGainSlider;
    
    MultiLevelMeter inMeter, outMeter;
    
    ShaperView shaperView;
    ptnz_gui::AttachedButton oversampleButton;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleClipAudioProcessorEditor)
};
