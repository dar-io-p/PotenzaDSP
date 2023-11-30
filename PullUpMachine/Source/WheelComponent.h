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
        auto radius = (float) juce::jmin (getWidth() / 2, getHeight() / 2) * 0.8f;
        auto centreX = (float) getX() + (float) getWidth() * 0.5f;
        auto centreY = (float) getY() + (float) getHeight() * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.f;
        
        juce::Rectangle<float> bounds (rx, ry, rw, rw);
        
        float imageWidth = 1000.f;
        auto transform = juce::AffineTransform::scale(rw / imageWidth, rw / imageWidth).translated(rx, ry).rotated(angle, bounds.getCentreX(), bounds.getCentreY());
        g.drawImageTransformed(vinylImage, transform);
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
    }
    
    void timerCallback () override
    {
        envelope.updateParams(processor.getBPM(), processor.getStartSpeed(), processor.getDuration(), processor.getPower());
        //convert direction (0=forwards,1=back) to (-1=back, 1=forward)
        auto direction = 2 * (int)processor.getDirection() - 1;
        angle += 0.05f * envelope.getNextValue(trig) * (trig ? direction : 1);
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
