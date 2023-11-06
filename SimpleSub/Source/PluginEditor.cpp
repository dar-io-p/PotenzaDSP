/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleSubAudioProcessorEditor::SimpleSubAudioProcessorEditor (SimpleSubAudioProcessor& p) :
    AudioProcessorEditor (&p),
    audioProcessor (p),
    envPanel(p.apvts),
    bottomPanel(p.apvts),
    distPanel(p.apvts),
    glidePanel(p.apvts)
{
    setSize (550, 400);

    addAndMakeVisible(envPanel);
    addAndMakeVisible(bottomPanel);
    addAndMakeVisible(distPanel);
    addAndMakeVisible(glidePanel);
}

SimpleSubAudioProcessorEditor::~SimpleSubAudioProcessorEditor()
{
}

//==============================================================================
void SimpleSubAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
    g.setColour(juce::Colours::darkgrey);
    g.drawRoundedRectangle(distPanel.getBounds().toFloat(), 5.f, 3.f);
}

void SimpleSubAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto w = bounds.getWidth();
    auto h = bounds.getHeight();
    
    envPanel.setBounds(bounds.removeFromTop(h * 3 / 8));
    bottomPanel.setBounds(bounds.removeFromBottom(h / 4));
    
    glidePanel.setBounds(bounds.removeFromLeft(w / 4).reduced(3));
    
    distPanel.setBounds(bounds.removeFromTop(h * 3 / 8).reduced(3));
}
