#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

namespace ptnz_gui
{

enum colour_ids
{
    buttonBackground,
    buttonHoverBackground,
    buttonDownBackground,
    buttonAccent,
    
    mainBackground,
    secondaryBackground,
    
    outlineColour,
    
    accent,
    secondaryAccent,
    
    white,
};

//namespace styles
//{
//    //const juce::Font labelFont  = juce::Font("Futura", 14.f, juce::Font::plain);
//inline juce::Font getLabelFont()     { return juce::Font("Futura", 14.f, juce::Font::plain); }
//inline juce::Font getTitleFont()     { return juce::Font("Futura", 16.f, juce::Font::plain); }
//inline juce::Font getPlainFont()     { return juce::Font("Futura", 13.f, juce::Font::plain); }
//inline juce::Font getBigTitleFont()  { return juce::Font("Futura", 25.f, juce::Font::bold); }
//
//}

class LNF : public juce::LookAndFeel_V4
{
public:
    LNF()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
        //setColour(juce::Slider::trackColourId, juce::Colour(0xff2e5090));
        //setColour(juce::Slider::trackColourId, juce::Colour(0xff084696));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::whitesmoke);
        // setColour(juce::Slider::backgroundColourId, juce::Colour(0xff084696));
        
        setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
        setColour(juce::Label::outlineColourId, juce::Colours::yellow);
        
        setColour(colour_ids::accent, juce::Colours::yellow);
        setColour(colour_ids::secondaryAccent, juce::Colours::lightyellow);
        
        setColour(colour_ids::buttonBackground, juce::Colour(60, 60, 80));
        setColour(colour_ids::buttonDownBackground, juce::Colour(30, 30, 60));
        setColour(colour_ids::buttonHoverBackground, juce::Colour(40, 40, 50));
        setColour(colour_ids::buttonAccent, juce::Colours::yellow);
        
        setColour(colour_ids::outlineColour, juce::Colours::black);
        setColour(colour_ids::white, juce::Colours::white);
        
        setColour(colour_ids::mainBackground, juce::Colours::black);
        setColour(colour_ids::secondaryBackground, juce::Colours::darkgrey);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) * 0.8f;
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
        arc.addCentredArc(centreX, centreY, radius * 0.7f, radius * 0.7f, 0.f, rotaryStartAngle, juce::jmax(angle - spacing - 0.05f, rotaryStartAngle), true);
        
        //draw Partially filled arc
        g.setColour (findColour(colour_ids::accent));
        g.strokePath(arc, juce::PathStrokeType(4.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        //POINTER
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 3.0f;
        //p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.addLineSegment(juce::Line<float>(0, -radius, 0, -radius + pointerLength), 1.f);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour (findColour(juce::Slider::thumbColourId));
        //g.fillPath(p);
        g.strokePath(p, juce::PathStrokeType(pointerThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        g.setColour(findColour(colour_ids::accent));
        if(slider.isMouseButtonDown())
        {
            g.fillEllipse(bounds.reduced(radius - radius * sliderPos * 0.23));
        }
    }
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.setColour(findColour(juce::Label::textColourId));
        g.setFont(13.f);
        g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
        
        //g.setColour(findColour(colour_ids::accent));
        //g.drawRoundedRectangle(label.getLocalBounds().toFloat(), 2.f, 1.f);
    }
    
    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override
    {
        juce::Rectangle<int> sliderBounds = slider.getLocalBounds();
        


        if(slider.getTextBoxPosition() == juce::Slider::TextBoxBelow)
        {
            auto textBoxHeight = 17;
            
            auto w = sliderBounds.getWidth();
            auto h = sliderBounds.getHeight();
            auto dim = 0;
            if (h > w)
                dim = sliderBounds.getWidth() + textBoxHeight;
            else
                dim = sliderBounds.getHeight();
            
            auto newBounds = juce::Rectangle<int>(0, 0, dim, dim).withCentre(sliderBounds.getCentre());
            
            juce::Rectangle<int> textBounds = newBounds.removeFromBottom(textBoxHeight);
            return { newBounds, textBounds };
        }

        return { sliderBounds, {} };
    }
    
    void  drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override
    {
        auto bounds = slider.getLocalBounds().reduced(2, 2);
        g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
        g.drawRoundedRectangle(bounds.toFloat(), 4.f, 2.f);
        
        g.setColour(findColour(colour_ids::accent));
        g.fillRoundedRectangle(slider.isHorizontal() ? juce::Rectangle<float> (static_cast<float> (x), (float) y + 0.5f, sliderPos - (float) x, (float) height - 1.0f)
                                          : juce::Rectangle<float> ((float) x + 0.5f, sliderPos, (float) width - 1.0f, (float) y + ((float) height - sliderPos)), 4.f);
    }
};

}
