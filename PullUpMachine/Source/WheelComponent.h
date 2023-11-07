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
        startTimerHz(24);
        envelope.prepare(24);
        angle = 0.0f;
        trig = false;
    }

    void paint (juce::Graphics& g) override
    {
        auto radius = (float) juce::jmin (getWidth() / 2, getHeight() / 2) * 0.9f;
        auto centreX = (float) getX() + (float) getWidth() * 0.5f;
        auto centreY = (float) getY() + (float) getHeight() * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.f;
        
        juce::Rectangle<float> bounds (rx, ry, rw, rw);
        
        //Main Circle
        g.setColour(juce::Colour(70, 70, 80));
        g.fillEllipse(bounds);
        
        //Label Circle
        g.setColour(juce::Colours::whitesmoke);
        g.fillEllipse(bounds.reduced(radius * 0.7));
        
        //Centre hole
        g.setColour(juce::Colours::black);
        g.fillEllipse(bounds.reduced(radius * 0.97));

        juce::Path p2;
        juce::PathStrokeType stroke(2.0f);
        float dash[2] = {5.f, 15.f};
        p2.addCentredArc(centreX, centreY, radius*0.85, radius*0.85, 0.0, 0.0f, juce::MathConstants<float>::twoPi, true);
        stroke.createDashedStroke(p2, p2, dash, 2);
        stroke.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
        p2.applyTransform(juce::AffineTransform::rotation(angle, centreX, centreY));
        g.setColour(juce::Colour(50, 50, 50).withAlpha(.8f));
        g.strokePath(p2, stroke);
        
        g.setColour(juce::Colour(40, 40, 40));
        g.drawEllipse(bounds.reduced(5), 4.f);
        
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 6.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(juce::Colours::whitesmoke);
        g.fillPath(p);
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
    }
    
    void timerCallback () override
    {
        envelope.updateParams(processor.getBPM(), processor.getStartSpeed(), processor.getDuration(), processor.getPower());
        
        angle += 0.05f * envelope.getNextValue(trig);
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
    
    SpeedEnvelope envelope;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WheelComponent)
};
