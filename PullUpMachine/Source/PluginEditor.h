/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WheelComponent.h"
#include "BufferView.h"
#include "ptnz_gui/ptnz_gui.h"
#include "WheelUpLookAndFeel.h"

class PullUpMachineAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PullUpMachineAudioProcessorEditor (PullUpMachineAudioProcessor&);
    ~PullUpMachineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    WheelUpLookAndFeel lnf;
    PullUpMachineAudioProcessor& audioProcessor;
    
    WheelComponent wheel;
    
    ptnz_gui::AttachedButton trigger;
    ptnz_gui::AttachedButton direction;
    
    ptnz_gui::AttachedSlider startSpeedSlider;
    ptnz_gui::AttachedSlider durationSlider;
    ptnz_gui::AttachedSlider bufferLengthSlider;
    ptnz_gui::AttachedSlider powerSlider;
    AttachedFader volFader;
    BufferView bufferView;
    juce::ColourGradient bgGrad;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PullUpMachineAudioProcessorEditor)
};
