/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ptnz_gui/ptnz_gui.h"

//==============================================================================
SimpleSubAudioProcessorEditor::SimpleSubAudioProcessorEditor (SimpleSubAudioProcessor& p) :
    AudioProcessorEditor (&p),
    audioProcessor (p),
    envPanel(p.apvts),
    middlePanel(p.apvts),
    bottomPanel(p.apvts)
{
    setSize (430 + bannerSize, 450);

    addAndMakeVisible(envPanel);
    addAndMakeVisible(bottomPanel);
    addAndMakeVisible(middlePanel);
}

SimpleSubAudioProcessorEditor::~SimpleSubAudioProcessorEditor()
{
}

//==============================================================================
void SimpleSubAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (lnf.findColour(ptnz_gui::colour_ids::mainBackground));
    
    //g.drawRoundedRectangle(distPanel.getBounds().toFloat(), 5.f, 3.f);
    //g.fillRoundedRectangle(distPanel.getBounds().toFloat(), 5.f);
    //g.setColour(lnf.findColour(ptnz_gui::colour_ids::outlineColour));
    //g.fillRoundedRectangle(distPanel.getBounds().expanded(6, 6).toFloat(), 8.f);
    g.setColour(lnf.findColour(ptnz_gui::colour_ids::secondaryBackground));
    g.fillRoundedRectangle(envPanel.getBounds().toFloat(), 5.f);
    g.fillRoundedRectangle(middlePanel.getBounds().toFloat(), 5.f);
    g.fillRoundedRectangle(bottomPanel.getBounds().toFloat(), 5.f);
    
    g.setColour(lnf.findColour(ptnz_gui::colour_ids::outlineColour));
    g.drawRoundedRectangle(envPanel.getBounds().toFloat(), 5.f, 3.f);
    g.drawRoundedRectangle(middlePanel.getBounds().toFloat(), 5.f, 3.f);
    g.drawRoundedRectangle(bottomPanel.getBounds().toFloat(), 5.f, 3.f);
    
    g.setFont(fonts::getBigTitleFont());
    auto banner = juce::Rectangle<int>(0,0,getWidth(),bannerSize+8).reduced(8, 0);
    g.drawFittedText("PotenzaDSP", banner, juce::Justification::centredLeft, 1);
    g.drawFittedText("SimpleSub", banner, juce::Justification::centredRight, 1);
    
    //g.fillRoundedRectangle(distPanel.getBounds().expanded(2, 2).toFloat(), 8.f);
}

void SimpleSubAudioProcessorEditor::resized()
{
    auto spacing = 8.f;

    auto bounds = getLocalBounds();
    bounds.removeFromTop(bannerSize);

    bounds.reduce(spacing, spacing / 2);
    //auto w = bounds.getWidth();
    auto h = bounds.getHeight();
    
    auto envBounds = bounds.removeFromTop((h * 7 / 17)).reduced(0.f, spacing / 2);
    auto midBounds = bounds.removeFromTop((h * 5 / 17)).reduced(0.f, spacing / 2);
    auto bottomBounds = bounds.reduced(0.f, spacing / 2);
    

    envPanel.setBounds(envBounds);
    middlePanel.setBounds(midBounds);
    bottomPanel.setBounds(bottomBounds);
    
    //glidePanel.setBounds(bounds.removeFromLeft(w / 4));
    
    //distPanel.setBounds(bounds.removeFromTop(h * 3 / 8));
}
