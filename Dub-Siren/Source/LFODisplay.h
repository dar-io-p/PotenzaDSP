#pragma once
#include <JuceHeader.h>

class LFODisplay : public juce::Component, public juce::Timer
{
public:
    LFODisplay(std::function<float(void)>&& callback) :
    getterFunction(std::move(callback))
    {
        startTimerHz(30);
    }
    
    void timerCallback() override
    {
        currentLFOVal = getterFunction();
        repaint();
    }
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
       // g.setColour(juce::Colours::white);
       // g.drawRoundedRectangle(bounds.reduced(3), 3.f, 2.f);
        
        bounds = bounds.withSizeKeepingCentre(getWidth() / 3, getHeight());
        g.drawRoundedRectangle(bounds, 3.f, 2.f);
        
        bounds.reduce(3.f, 3.f);
        
        auto yzero = getHeight() / 2;
        int y = yzero - (currentLFOVal * getHeight() / 2);
        
        int thumbHeight = 20;
        
        y = juce::jmap((float)y, 0.f, (float)getHeight(), bounds.getY(), bounds.getBottom() - thumbHeight);
        
        g.setColour(juce::Colours::red);
        g.fillRoundedRectangle(bounds.getX(), (float) y, bounds.getWidth(), thumbHeight, 3.f);
    }
    
    void resized() override
    {
        const auto bounds = getLocalBounds().toFloat();
        grad = juce::ColourGradient {
            juce::Colours::red.withAlpha(1.0f),
            bounds.getBottomLeft(),
            juce::Colours::red,
            bounds.getTopLeft(),
            false
        };
    }
private:
    float currentLFOVal = 0.0f;
    juce::ColourGradient grad;
    std::function<float(void)> getterFunction;
};
