#include "PluginProcessor.h"
#include "PluginEditor.h"

//=============================================================================
PullUpMachineAudioProcessorEditor::PullUpMachineAudioProcessorEditor (PullUpMachineAudioProcessor& p) :
    AudioProcessorEditor (&p),
    audioProcessor (p),
    wheel(p),
    trigger(p.apvts, param::toID(param::PID::Trigger),"PULL UP", &lnf),
    direction(p.apvts, param::toID(param::PID::Direction), "", &lnf),
    startSpeedSlider(p.apvts, param::toID(param::PID::StartSpeed), param::toName(param::PID::StartSpeed), &lnf),
    durationSlider(p.apvts, param::toID(param::PID::Duration), param::toName(param::PID::Duration), &lnf),
    bufferLengthSlider(p.apvts, param::toID(param::PID::BufferLength), param::toName(param::PID::BufferLength), &lnf),
    powerSlider(p.apvts, param::toID(param::PID::Power), param::toName(param::PID::Power), &lnf),
    bufferView(p.getCircularBuffer())
{
    setSize (340, 490);
    
    addAndMakeVisible(wheel);
    
    addAndMakeVisible(trigger.button);
    trigger.button.onClick = [&](){
        if (trigger.button.getToggleState())
            wheel.trigger();
        else
            wheel.reset();
    };
    
    addAndMakeVisible(direction.button);
    direction.button.onClick = [&](){
        if (direction.button.getToggleState())
            direction.button.setButtonText("<<");
        else
            direction.button.setButtonText(">>");
    };
    
    addAndMakeVisible(startSpeedSlider.slider);
    addAndMakeVisible(durationSlider.slider);
    addAndMakeVisible(bufferLengthSlider.slider);
    addAndMakeVisible(powerSlider.slider);
    
    addAndMakeVisible(bufferView);
    
    direction.button.onClick();
}

PullUpMachineAudioProcessorEditor::~PullUpMachineAudioProcessorEditor()
{
}

//==============================================================================
void PullUpMachineAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(lnf.findColour(ptnz_gui::colour_ids::mainBackground));
    
    auto x = 0;
    auto w = startSpeedSlider.slider.getWidth();
    auto h = 40;
    auto y = startSpeedSlider.slider.getY() - 30;
    g.setColour(lnf.findColour(ptnz_gui::colour_ids::white));
    g.setFont(ptnz_gui::styles::getPlainFont());
    
    g.drawFittedText("Start Speed", x, y, w, h, juce::Justification::centred, 2);
    x += w;
    g.drawFittedText("Duration (Bars)", x, y, w, h, juce::Justification::centred, 2);
    
    x = bufferLengthSlider.slider.getX();
    g.drawFittedText("Buffer Length \n(Bars)", x, y, w, h, juce::Justification::centred, 2);
    x += w;
    g.drawFittedText("Power", x, y, w, h, juce::Justification::centred, 2);
    
    //g.drawHorizontalLine(bufferView.getY()-4, 0, getWidth());
    g.setColour(lnf.findColour(ptnz_gui::colour_ids::white).withAlpha(0.5f));
    g.drawLine(bufferView.getX() + 15, bufferView.getY(), bufferView.getRight() - 15, bufferView.getY(), 2.0f);
}

void PullUpMachineAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    wheel.setBounds(bounds.removeFromTop(300));
    
    int pot_width = getWidth() / 5;
    
    auto controlBounds = bounds.removeFromTop(pot_width + 20);
    
    startSpeedSlider.slider.setBounds(controlBounds.removeFromLeft(pot_width));
    durationSlider.slider.setBounds(controlBounds.removeFromLeft(pot_width));
    
    powerSlider.slider.setBounds(controlBounds.removeFromRight(pot_width));
    bufferLengthSlider.slider.setBounds(controlBounds.removeFromRight(pot_width));
    
    trigger.button.setBounds(controlBounds.removeFromTop((pot_width + 20) / 2).reduced(5));
    direction.button.setBounds(controlBounds.reduced(5));
    
    bounds.removeFromTop(7);
    bufferView.setBounds(bounds);
}
