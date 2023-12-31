/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ptnz_gui/ptnz_gui.h"
#include "EnvelopePanel.h"
#include "BottomPanel.h"
#include "DistortionPanel.h"
#include "MiddlePanel.h"

#include "SimpleSubLookAndFeel.h"

class SimpleSubAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleSubAudioProcessorEditor (SimpleSubAudioProcessor&);
    ~SimpleSubAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleSubLookAndFeel lnf;
    SimpleSubAudioProcessor& audioProcessor;
    EnvelopePanel envPanel;
    MiddlePanel middlePanel;
    BottomPanel bottomPanel;
    
    const int bannerSize = 30;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSubAudioProcessorEditor)
};
