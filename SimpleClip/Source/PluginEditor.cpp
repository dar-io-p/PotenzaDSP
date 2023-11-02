/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleClipAudioProcessorEditor::SimpleClipAudioProcessorEditor (SimpleClipAudioProcessor& p)
    :
    AudioProcessorEditor (&p),
    audioProcessor (p),
    threshSlider(p.apvts, param::toID(param::PID::Threshold), param::toName(param::PID::Threshold)),
    kneeSlider(p.apvts, param::toID(param::PID::Knee), param::toName(param::PID::Knee)),
    inputGainSlider(p.apvts, param::toID(param::PID::InputGain), param::toName(param::PID::InputGain)),
    outPutGainSlider(p.apvts, param::toID(param::PID::OutputGain), param::toName(param::PID::OutputGain)),
    inMeter([&]() {return audioProcessor.getInRMSPeakLevels();}, true),
    outMeter([&]() {return audioProcessor.getOutRMSPeakLevels();}, false),
    shaperView(audioProcessor.getAudioBufferQueue()),
    oversampleButton(p.apvts, param::toID(param::PID::Oversampling), "HQ")
{
    setSize (consts::WIDTH, consts::HEIGHT);
    setLookAndFeels();
    
    std::function<void()> onValueChange = [this] {
        auto k = param::cook::knee(kneeSlider.slider.getValue());
        auto t = param::cook::threshold(threshSlider.slider.getValue());
        shaperView.updateGraph(k, t);
    };
    addAndMakeVisible(kneeSlider.slider);
    kneeSlider.slider.onValueChange = onValueChange;
    
    addAndMakeVisible(threshSlider.slider);
    threshSlider.slider.onValueChange = onValueChange;
        
    addAndMakeVisible(inMeter);
    addAndMakeVisible(inputGainSlider.slider);
    
    addAndMakeVisible(outMeter);
    addAndMakeVisible(outPutGainSlider.slider);
    
    addAndMakeVisible(oversampleButton.button);
    
    addAndMakeVisible(shaperView);
    onValueChange();
}

SimpleClipAudioProcessorEditor::~SimpleClipAudioProcessorEditor()
{
    unsetLookAndFeel();
}

//==============================================================================
void SimpleClipAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (consts::black);
}

void SimpleClipAudioProcessorEditor::paintOverChildren (juce::Graphics& g)
{
    g.setColour(consts::whitesmoke);
    g.drawRoundedRectangle(shaperView.getBounds().toFloat(), 1.f, 2.f);
//    g.drawRoundedRectangle(inMeter.getBounds().toFloat(), 1.f, 2.f);
//    g.drawRoundedRectangle(outMeter.getBounds().toFloat(), 1.f, 2.f);
}

void SimpleClipAudioProcessorEditor::resized()
{
    const int pad = consts::u;
    
    auto bounds = getLocalBounds();
    auto bottomStrip = bounds.removeFromBottom(pad * 2);
    auto topStrip = bounds.removeFromTop(pad);
    
    auto x = (bounds.getWidth() - bounds.getHeight()) / 2;
    auto leftStrip = bounds.removeFromLeft(x);
    auto rightStrip = bounds.removeFromRight(x);
    
    shaperView.setBounds(bounds);
    inMeter.setBounds(leftStrip.removeFromRight(pad*2).translated(-4, 0));
    outMeter.setBounds(rightStrip.removeFromLeft(pad*2).translated(4, 0));
    
    inputGainSlider.slider.setBounds(bottomStrip.removeFromLeft(x).removeFromRight(pad*2).reduced(0, 2));
    outPutGainSlider.slider.setBounds(bottomStrip.removeFromRight(x).removeFromLeft(pad*2).reduced(0, 2));
    
    threshSlider.slider.setBounds(bottomStrip.removeFromLeft(x).reduced(10, 2));
    kneeSlider.slider.setBounds(bottomStrip.removeFromRight(x).reduced(10, 2));
    
    oversampleButton.button.setBounds(bottomStrip.reduced(10, 5));
    
    
}
