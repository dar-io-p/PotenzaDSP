#pragma once

#include <JuceHeader.h>
#include "SpeedEnvelope.h"

//==============================================================================
/*
*/
class WheelComponent  : public juce::Component, public juce::Timer
{
public:
    WheelComponent(PullUpMachineAudioProcessor& p) :
    processor(p),
    envelope()
    {
        startTimerHz(40);
        envelope.prepare(40);
        angle = 0.0f;
        trig = false;
        
        vinylImage = juce::ImageCache::getFromMemory(BinaryData::purplevinyl_png, BinaryData::purplevinyl_pngSize);
    }

    void paint (juce::Graphics& g) override
    {
        //calculate the centre of the bounds and centre the circle there
        auto radius = (float) juce::jmin (getWidth() * 0.5f, getHeight() * 0.5f) * 0.8f;
        auto centreX = getLocalBounds().getCentreX();
        auto centreY = getLocalBounds().getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.f;
        
        juce::Rectangle<float> bounds (rx, ry, rw, rw);
        
        float imageWidth = 1000.f;
        auto transform = juce::AffineTransform::scale(rw / imageWidth, rw / imageWidth).translated(rx, ry).rotated(angle, bounds.getCentreX(), bounds.getCentreY());
        g.drawImageTransformed(vinylImage, transform);
    }
    
    void timerCallback () override
    {
        envelope.updateParams(processor.getBPM(), processor.getStartSpeed(), processor.getDuration(), processor.getPower());
        envelope.changeDirection(processor.getDirection());
        angle += trig ? 0.05f * envelope.getNextValue() : 0.05f;
        float pi = juce::MathConstants<float>::pi;
        while (angle < -pi)
            angle += 2 * pi;
        
        while (angle > pi)
            angle -= 2 * pi;
        
        repaint();
    }
    
    void trigger()
    {
        trig = true;
    }
    
    void reset()
    {
        envelope.reset();
        trig = false;
    }
    
private:
    PullUpMachineAudioProcessor& processor;
    float angle;
    bool trig;
    
    juce::Image vinylImage;
    
    SpeedEnvelope envelope;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WheelComponent)
};
