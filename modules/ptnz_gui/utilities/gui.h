#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "./lnf.h"

namespace ptnz_gui {

using Slider = juce::Slider;
using Attachment = juce::SliderParameterAttachment;
using apvtsAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

struct AttachedSlider
{
    AttachedSlider(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& name, juce::LookAndFeel_V4* custom_lnf=nullptr) :
        slider(),
        attachment(*apvts.getParameter(iD), slider)
    {
        if(custom_lnf)
            slider.setLookAndFeel(custom_lnf);
        else
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
    AttachedSliderVertical(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& name, juce::LookAndFeel_V4* custom_lnf=nullptr) :
        slider(),
        attachment(*apvts.getParameter(iD), slider)
    {
        if(custom_lnf)
            slider.setLookAndFeel(custom_lnf);
        else
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
    ToggleButtonWithText(const juce::String& buttonText, juce::LookAndFeel_V4* custom_lnf=nullptr) :
    juce::ToggleButton(buttonText)
    {
        setLookAndFeel(&lnf);
    }
    
    ~ToggleButtonWithText()
    {
        setLookAndFeel(nullptr);
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Colour bgColour = getLookAndFeel().findColour(colour_ids::buttonBackground);
        if (getToggleState())
            bgColour = getLookAndFeel().findColour(colour_ids::buttonDownBackground);// Set a different color for the "on" state
        if (isMouseOver())
            bgColour = getLookAndFeel().findColour(colour_ids::buttonHoverBackground);
        g.setColour(bgColour);
        
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
        if(getToggleState())
        {
            g.setColour(getLookAndFeel().findColour(colour_ids::accent));
            g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(5), 5.f, 2.f);
        }
        
        g.setColour(getLookAndFeel().findColour(juce::TextButton::textColourOnId));
        g.setFont(13.f);
        g.drawFittedText(getButtonText(), getLocalBounds(), juce::Justification::centred, 1);
    }
private:
    LNF lnf;
};

class OnOffButton : public juce::ToggleButton
{
public:
    OnOffButton() : juce::ToggleButton() {}

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Colour bgColour = getLookAndFeel().findColour(colour_ids::buttonBackground);
        if (getToggleState())
            bgColour = getLookAndFeel().findColour(colour_ids::buttonDownBackground);// Set a different color for the "on" state
        if (isMouseOver())
            bgColour = getLookAndFeel().findColour(colour_ids::buttonHoverBackground);
        g.setColour(bgColour);
        
        g.fillEllipse(getLocalBounds().toFloat());
        if(getToggleState())
            g.setColour(getLookAndFeel().findColour(colour_ids::buttonAccent));
        else
            g.setColour(getLookAndFeel().findColour(juce::Label::textColourId));
        
        auto bounds = getLocalBounds().toFloat();
        g.drawEllipse(bounds.withSizeKeepingCentre(9, 9), 2.f);
        
        //auto rect = juce::Rectangle<int>(2, getHeight() / 5);
        //g.drawRect(rect.withCentre(getLocalBounds().getCentre()));
        //g.setColour(juce::Colours::whitesmoke);
    }
};

struct AttachedButton
{
    AttachedButton(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& label, juce::LookAndFeel_V4* custom_lnf=nullptr) :
        button(label),
        attachment(*apvts.getParameter(iD), button, nullptr)
    {
        if(custom_lnf)
            button.setLookAndFeel(custom_lnf);
    }
    ~AttachedButton()
    {
        button.setLookAndFeel(nullptr);
    }
    ToggleButtonWithText button;
    juce::ButtonParameterAttachment attachment;
};

struct AttachedOnOffButton
{
    AttachedOnOffButton(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, juce::LookAndFeel_V4* custom_lnf=nullptr) :
        button(),
        attachment(*apvts.getParameter(iD), button, nullptr)
    {
        if(custom_lnf)
            button.setLookAndFeel(custom_lnf);
    }
    ~AttachedOnOffButton()
    {
        button.setLookAndFeel(nullptr);
    }
    OnOffButton button;
    juce::ButtonParameterAttachment attachment;
};

}
