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
modRateSlider(p.apvts, param::toID(param::p_LFORate), param::toName(param::p_LFORate), &lnf),
modAmountSlider(p.apvts, param::toID(param::p_LFODepth), param::toName(param::p_LFODepth), &lnf),
lfoBlendSlider(p.apvts, param::toID(param::p_LFOBlend), param::toName(param::p_LFOBlend), &lnf),
baseFreqSlider(p.apvts, param::toID(param::p_oscFreq), param::toName(param::p_oscFreq), &lnf),
outGainSlider(p.apvts, param::toID(param::p_outGain), param::toName(param::p_outGain), &lnf),
activateButton(p.apvts, param::toID(param::p_active), "DUB!")
{
    setSize (350, 300);
    addAndMakeVisible(lfo1Choice.button);
    addAndMakeVisible(lfo2Choice.button);
    addAndMakeVisible(lfoDisplay);
    
    addAndMakeVisible(modRateSlider.slider);
    addAndMakeVisible(modAmountSlider.slider);
    addAndMakeVisible(lfoBlendSlider.slider);
    addAndMakeVisible(baseFreqSlider.slider);
    addAndMakeVisible(outGainSlider.slider);
    
    addAndMakeVisible(activateButton.button);
    
    bgImage = juce::ImageCache::getFromMemory(BinaryData::BG_png, BinaryData::BG_pngSize);
}

DubSirenAudioProcessorEditor::~DubSirenAudioProcessorEditor()
{
}

//==============================================================================
void DubSirenAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.drawImage(bgImage, getBounds().toFloat());
    
    g.setFont(fonts::getLabelFont());
    g.setColour(getLookAndFeel().findColour(ptnz_gui::white).withAlpha(0.5f));
    drawSliderLabel(g, modRateSlider.slider, "RATE");
    drawSliderLabel(g, modAmountSlider.slider, "AMOUNT");
    drawSliderLabel(g, lfoBlendSlider.slider, "LFO BLEND");
    drawSliderLabel(g, baseFreqSlider.slider, "FREQ");
    drawSliderLabel(g, outGainSlider.slider, "GAIN");
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
    lfoBlendSlider.slider.setBounds(top.removeFromLeft(delta));
    modAmountSlider.slider.setBounds(top);
    
    auto choiceWidth = 30;
    auto spacing = 50;
    middle.removeFromLeft(spacing);
    lfo1Choice.button.setBounds(middle.removeFromLeft(choiceWidth).reduced(0, 15));
    middle.removeFromRight(spacing);
    lfo2Choice.button.setBounds(middle.removeFromRight(choiceWidth).reduced(0, 15));
    lfoDisplay.setBounds(middle.reduced(25, 10));
    
    baseFreqSlider.slider.setBounds(bottomBounds.removeFromLeft(delta));
    auto activateBounds = bottomBounds.removeFromLeft(delta);
    activateBounds = activateBounds.withSizeKeepingCentre(70, 70);
    activateButton.button.setBounds(activateBounds);
    outGainSlider.slider.setBounds(bottomBounds);
}
