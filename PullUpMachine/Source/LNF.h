#pragma once
#include <JuceHeader.h>

class LNF : public juce::LookAndFeel_V4
{
public:
    LNF()
    {
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) * 0.7f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto spacing = 0.25f;
        
        g.setColour (juce::Colours::whitesmoke);
        juce::Rectangle<float> bounds (rx, ry, rw, rw);
        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius, radius, 0.f, rotaryStartAngle, juce::jmax(angle - spacing, rotaryStartAngle), true);
        g.strokePath(arc, juce::PathStrokeType(3.0f));
        arc.addCentredArc(centreX, centreY, radius, radius, 0.f, juce::jmin(angle + spacing, rotaryEndAngle), rotaryEndAngle, true);
        
        g.strokePath(arc, juce::PathStrokeType(3.0f));
        
        arc.clear();
        arc.addCentredArc(centreX, centreY, radius * 0.8f, radius * 0.8f, 0.f, rotaryStartAngle, juce::jmax(angle - spacing, rotaryStartAngle), true);
        g.setColour (juce::Colours::yellow);
        g.strokePath(arc, juce::PathStrokeType(2.5f));
        
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 4.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.fillPath(p);
    }
    
private:
};
