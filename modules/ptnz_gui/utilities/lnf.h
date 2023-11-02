#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

namespace ptnz_gui
{

class LNF : public juce::LookAndFeel_V4
{
public:
    LNF()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::trackColourId, juce::Colours::yellow);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
        setColour(juce::Slider::backgroundColourId, juce::Colours::yellow);
        
        setColour(juce::Label::textColourId, juce::Colours::white);
        setColour(juce::Label::outlineColourId, juce::Colours::yellow);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) * 0.7f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto spacing = 0.3f;
        
        //draw full arc
        g.setColour (findColour(juce::Slider::rotarySliderOutlineColourId));
        juce::Rectangle<float> bounds (rx, ry, rw, rw);
        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius, radius, 0.f, rotaryStartAngle, juce::jmax(angle - spacing, rotaryStartAngle), true);
        //g.strokePath(arc, juce::PathStrokeType(3.0f));
        arc.addCentredArc(centreX, centreY, radius, radius, 0.f, juce::jmin(angle + spacing, rotaryEndAngle), rotaryEndAngle, true);
        
        g.strokePath(arc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
        arc.clear();
        arc.addCentredArc(centreX, centreY, radius * 0.8f, radius * 0.8f, 0.f, rotaryStartAngle, juce::jmax(angle - spacing - 0.05f, rotaryStartAngle), true);
        //draw Partially filled arc
        g.setColour (findColour(juce::Slider::trackColourId));
        g.strokePath(arc, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        //POINTER
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 4.0f;
        //p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.addLineSegment(juce::Line<float>(0, -radius, 0, -radius + pointerLength), 1.f);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour (findColour(juce::Slider::thumbColourId));
        //g.fillPath(p);
        g.strokePath(p, juce::PathStrokeType(pointerThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        g.setColour(findColour(juce::Slider::backgroundColourId));
        if(slider.isMouseButtonDown())
        {
            g.fillEllipse(bounds.reduced(radius - radius * sliderPos * 0.2));
        }
    }
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.setColour(findColour(juce::Label::textColourId));
        g.setFont(label.getHeight() * 0.8);
        g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
        
        g.setColour(findColour(juce::Label::outlineColourId));
        g.drawRoundedRectangle(label.getLocalBounds().toFloat().reduced(2.f), 5.f, 2.f);
    }
    
    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override
    {
        juce::Rectangle<int> sliderBounds = slider.getLocalBounds();

        if(slider.getTextBoxPosition() == juce::Slider::TextBoxBelow)
        {
            juce::Rectangle<int> textBounds = sliderBounds.removeFromBottom(slider.getHeight() / 5);
            return { sliderBounds, textBounds };
        }

        return { sliderBounds, {} };
    }
    
    void  drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override
    {
        auto bounds = slider.getLocalBounds().reduced(2, 2);
        g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
        g.drawRoundedRectangle(bounds.toFloat(), 4.f, 2.f);
        
        g.setColour(findColour(juce::Slider::trackColourId));
        g.fillRoundedRectangle(slider.isHorizontal() ? juce::Rectangle<float> (static_cast<float> (x), (float) y + 0.5f, sliderPos - (float) x, (float) height - 1.0f)
                                          : juce::Rectangle<float> ((float) x + 0.5f, sliderPos, (float) width - 1.0f, (float) y + ((float) height - sliderPos)), 4.f);
    }
};

}
