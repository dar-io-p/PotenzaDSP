#pragma once
#include <JuceHeader.h>
#include "Parameters.h"


class ChoiceButton : public juce::Component
{
public:
    ChoiceButton()
    {
        
    }
    void update(int val)
    {
        currentIndex = val;
        repaint();
    }
    
    void update()
    {
        currentIndex += 1;
        if (currentIndex >= numOptions)
            currentIndex -= numOptions;
    }
    
    int getValue()
    {
        return currentIndex;
    }
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds.reduced(1.f), 2.f, 2.f);
        
        auto delta = getHeight() / (numOptions);
        
        g.setColour(juce::Colours::grey);
        
        auto x = 0;
        auto y = currentIndex * delta;
        auto w = getWidth();
        auto h = delta;
        
        auto thumbBounds = juce::Rectangle<float>(x, y, w, h);
        
        g.fillRoundedRectangle(thumbBounds, 5.f);
        
        g.setColour(juce::Colours::black);
        
        thumbBounds.reduce(5.f, h / 4);
        delta = thumbBounds.getHeight() / 5;
        g.fillRoundedRectangle(thumbBounds.removeFromTop(delta), 2.f);
        thumbBounds.removeFromTop(delta);
        g.fillRoundedRectangle(thumbBounds.removeFromTop(delta), 2.f);
        thumbBounds.removeFromTop(delta);
        g.fillRoundedRectangle(thumbBounds.removeFromTop(delta), 2.f); 
        
    }
    
    void setNumOptions(int n) {numOptions = n;}
private:
    int numOptions = 1;
    int currentIndex = 0;
};

struct ChoiceParameterAttachment : private juce::MouseListener
{
public:
    ChoiceParameterAttachment(juce::RangedAudioParameter& param, ChoiceButton& buttonToUse) :
    button(buttonToUse),
    attachment(param, [this] (float f) { setValue (f); })
    {
        auto range = param.getNormalisableRange();
        button.setNumOptions((int)range.end + 1);
        attachment.sendInitialUpdate();
        button.addMouseListener(this, false);
    }
    
    void setValue(float newValue)
    {
        button.update(newValue);
    }
    
    void mouseDown(const juce::MouseEvent &event) override
    {
        button.update();
        if (! ignoreCallbacks)
            attachment.setValueAsCompleteGesture(button.getValue());
    }
    
private:
    ChoiceButton& button;
    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
};


struct AttachedChoiceButton
{
    AttachedChoiceButton(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& name) :
    button(),
    attachment(*apvts.getParameter(iD), button)
    {
    }
    ChoiceButton button;
    ChoiceParameterAttachment attachment;
};


