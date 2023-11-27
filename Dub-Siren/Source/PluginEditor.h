/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "LFODisplay.h"
#include "ptnz_gui/ptnz_gui.h"

//==============================================================================
/**
*/
class DubSirenAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DubSirenAudioProcessorEditor (DubSirenAudioProcessor&);
    ~DubSirenAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DubSirenAudioProcessor& audioProcessor;
    
    AttachedChoiceButton lfo1Choice;
    AttachedChoiceButton lfo2Choice;
    
    LFODisplay lfoDisplay;
    ptnz_gui::AttachedSlider modRateSlider;
    ptnz_gui::AttachedSlider modAmountSlider;
    ptnz_gui::AttachedSlider lfoBlendSlider;
    
    ptnz_gui::AttachedSlider baseFreqSlider;
    ptnz_gui::AttachedSlider outGainSlider;
    
    ptnz_gui::AttachedButton activateButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DubSirenAudioProcessorEditor)
};
