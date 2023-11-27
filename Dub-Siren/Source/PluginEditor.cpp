/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DubSirenAudioProcessorEditor::DubSirenAudioProcessorEditor (DubSirenAudioProcessor& p) :
AudioProcessorEditor (&p),
audioProcessor (p),
lfo1Choice(p.apvts, param::toID(param::p_LFO1Type), param::toName(param::p_LFO1Type)),
lfo2Choice(p.apvts, param::toID(param::p_LFO2Type), param::toName(param::p_LFO2Type)),
lfoDisplay([this] () {return audioProcessor.getLFOValue();}),
modRateSlider(p.apvts, param::toID(param::p_LFORate), param::toName(param::p_LFORate)),
modAmountSlider(p.apvts, param::toID(param::p_LFODepth), param::toName(param::p_LFODepth)),
lfoBlendSlider(p.apvts, param::toID(param::p_LFOBlend), param::toName(param::p_LFOBlend)),
baseFreqSlider(p.apvts, param::toID(param::p_oscFreq), param::toName(param::p_oscFreq)),
outGainSlider(p.apvts, param::toID(param::p_outGain), param::toName(param::p_outGain)),
activateButton(p.apvts, param::toID(param::p_active), "BOOOP!")
{
    setSize (400, 300);
    addAndMakeVisible(lfo1Choice.button);
    addAndMakeVisible(lfo2Choice.button);
    addAndMakeVisible(lfoDisplay);
    
    addAndMakeVisible(modRateSlider.slider);
    addAndMakeVisible(modAmountSlider.slider);
    addAndMakeVisible(lfoBlendSlider.slider);
    addAndMakeVisible(baseFreqSlider.slider);
    addAndMakeVisible(outGainSlider.slider);
    
    addAndMakeVisible(activateButton.button);
}

DubSirenAudioProcessorEditor::~DubSirenAudioProcessorEditor()
{
}

//==============================================================================
void DubSirenAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void DubSirenAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto panel_height = bounds.getHeight() / 3;
    
    auto top = bounds.removeFromTop(panel_height);
    auto middle = bounds.removeFromTop(panel_height);
    auto bottomBounds = bounds;
    
    auto delta = top.getWidth() / 3;
    
    modRateSlider.slider.setBounds(top.removeFromLeft(delta));
    lfoDisplay.setBounds(top.removeFromLeft(delta));
    modAmountSlider.slider.setBounds(top);
    
    lfo1Choice.button.setBounds(middle.removeFromLeft(delta));
    lfo2Choice.button.setBounds(middle.removeFromRight(delta));
    lfoBlendSlider.slider.setBounds(middle);
    
    baseFreqSlider.slider.setBounds(bottomBounds.removeFromLeft(delta));
    activateButton.button.setBounds(bottomBounds.removeFromLeft(delta));
    outGainSlider.slider.setBounds(bottomBounds);
}
