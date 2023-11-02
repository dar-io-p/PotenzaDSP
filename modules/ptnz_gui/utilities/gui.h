#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "./lnf.h"

namespace ptnz_gui {

using Slider = juce::Slider;
using Attachment = juce::SliderParameterAttachment;

struct AttachedSlider
{
    AttachedSlider(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& name) :
        slider(),
        attachment(*apvts.getParameter(iD), slider)
    {
        slider.setLookAndFeel(&lnf);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    }
    ~AttachedSlider()
    {
        slider.setLookAndFeel(nullptr);
    }
    LNF lnf;
    Slider slider;
    Attachment attachment;
};

struct AttachedSliderVertical
{
    AttachedSliderVertical(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& name) :
        slider(),
        attachment(*apvts.getParameter(iD), slider)
    {
        slider.setLookAndFeel(&lnf);
        //slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setSliderStyle(juce::Slider::LinearBarVertical);
    }
    ~AttachedSliderVertical()
    {
        slider.setLookAndFeel(nullptr);
    }
    LNF lnf;
    Slider slider;
    Attachment attachment;
};


class ToggleButtonWithText : public juce::ToggleButton
{
public:
    enum Shape
    {
        Rectangle,
        Ellipse,
    };
    ToggleButtonWithText(const juce::String& buttonText) : juce::ToggleButton(buttonText) {shape = Rectangle; setLookAndFeel(&lnf);}
    ToggleButtonWithText(const juce::String& buttonText, Shape buttonShape) :
    juce::ToggleButton(buttonText),
    shape(buttonShape)
    {
        setLookAndFeel(&lnf);
    }
    
    ~ToggleButtonWithText()
    {
        setLookAndFeel(nullptr);
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Colour bgColour = juce::Colour(60, 60, 80);
        if (getToggleState())
            bgColour = juce::Colour(30, 30, 60);// Set a different color for the "on" state
        if (isMouseOver())
            bgColour = juce::Colour(40, 40, 50);
        g.setColour(bgColour);
        
        switch (shape) {
            case Rectangle:
                g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
                if(getToggleState())
                {
                    g.setColour(getLookAndFeel().findColour(juce::Slider::trackColourId));
                    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(5), 5.f, 2.f);
                }
                break;
            case Ellipse:
                g.fillEllipse(getLocalBounds().toFloat());
                if(getToggleState())
                {
                    g.setColour(getLookAndFeel().findColour(juce::Slider::trackColourId));
                    g.drawEllipse(getLocalBounds().toFloat().reduced(5), 2.f);
                }
                break;
            default:
                break;
        }
        
        g.setColour(juce::Colours::whitesmoke);
        auto h = getHeight();
        if (h > 50){
            g.setFont(20.f);
        }else if (h > 30) {
            g.setFont(13.f);
        }
        else{
            g.setFont(11.f);
        }
        g.drawFittedText(getButtonText(), getLocalBounds(), juce::Justification::centred, 1);
    }
    Shape getShape() {return shape;}
    void setShape(Shape newShape) {shape = newShape;}
private:
    LNF lnf;
    Shape shape;
};

class OnOffButton : public juce::ToggleButton
{
public:
    OnOffButton() : juce::ToggleButton() {}

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Colour bgColour = juce::Colour(60, 60, 80);
        if (getToggleState())
            bgColour = juce::Colour(30, 30, 60);// Set a different color for the "on" state
        if (isMouseOver())
            bgColour = juce::Colour(40, 40, 50);
        g.setColour(bgColour);
        
        g.fillEllipse(getLocalBounds().toFloat());
        if(getToggleState())
            g.setColour(juce::Colours::yellow);
        else
            g.setColour(juce::Colours::white);
        
        g.drawEllipse(getLocalBounds().toFloat().reduced(4), 2.f);
        
        auto rect = juce::Rectangle<int>(2, getHeight() / 4);
        g.drawRect(rect.withCentre(getLocalBounds().getCentre()));
        g.setColour(juce::Colours::whitesmoke);
    }
};

struct AttachedButton
{
    AttachedButton(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& label) :
        button(label),
        attachment(*apvts.getParameter(iD), button, nullptr)
    {
    }
    ToggleButtonWithText button;
    juce::ButtonParameterAttachment attachment;
};

struct AttachedOnOffButton
{
    AttachedOnOffButton(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD) :
        button(),
        attachment(*apvts.getParameter(iD), button, nullptr)
    {
    }
    OnOffButton button;
    juce::ButtonParameterAttachment attachment;
};

}
