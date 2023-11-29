#pragma once
#include "ptnz_gui/ptnz_gui.h"
#include "Parameters.h"

namespace colours
{
const juce::Colour background = juce::Colour(0xff0b0b0d);
const juce::Colour grey1 = juce::Colour(0xff3b3b4f);
const juce::Colour grey2 = juce::Colour(0xff1c1b21);

const juce::Colour mainBlue = juce::Colour(0xff0a5fcc);//juce::Colour(0xff084696);
const juce::Colour secondaryBlue = juce::Colour(0xff2292d4);

const juce::Colour textColour = juce::Colours::whitesmoke;
}

class SimpleSubLookAndFeel : public ptnz_gui::LNF
{
public:
    SimpleSubLookAndFeel()
    : ptnz_gui::LNF()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
        //setColour(juce::Slider::trackColourId, juce::Colour(0xff2e5090));
        
        //track colour is the partially filled colour
        setColour(juce::Slider::trackColourId, colours::mainBlue);
        //
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::whitesmoke);
        //setColour(juce::Slider::backgroundColourId, juce::Colour(0xff084696));
        
        setColour(juce::Label::textColourId, colours::textColour);
        setColour(juce::Label::outlineColourId, colours::mainBlue);
        
        using c = ptnz_gui::colour_ids;
        
        setColour(c::buttonAccent, colours::mainBlue);
        
        setColour(c::accent, colours::mainBlue);
        setColour(c::secondaryAccent, colours::secondaryBlue);
        
        setColour(c::mainBackground, colours::background);
        setColour(c::secondaryBackground, colours::grey2);
        
        setColour(c::outlineColour, colours::grey1);
        setColour(c::white, colours::textColour);
    }
};

class EnvelopeLookAndFeel : public SimpleSubLookAndFeel
{
public:
    EnvelopeLookAndFeel() :
    SimpleSubLookAndFeel()
    {}
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &slider) override
    {
        auto bounds = slider.getLocalBounds();
        g.setColour(findColour(ptnz_gui::colour_ids::secondaryAccent));
        //g.setFont(12.f);
        g.setFont(juce::Font("Futura", 14.f, juce::Font::plain));
        g.drawFittedText(slider.getTextFromValue(slider.getValue()), bounds, juce::Justification::centred, 1);
    }
};

struct ChoiceButtonParameterAttachment : private juce::Button::Listener
{
public:
    ChoiceButtonParameterAttachment(juce::RangedAudioParameter& param, ptnz_gui::ToggleButtonWithText& buttonToUse, int idx) :
    button(buttonToUse),
    attachment(param, [this, idx] (float f) { setValue (f); }),
    index(idx)
    {
        button.addListener(this);
        attachment.sendInitialUpdate();
    }
    
    void setValue(float newValue)
    {
        //button.setToggleState(false, juce::NotificationType::dontSendNotification);

        if ((int) newValue != index)
        {
            button.setToggleState(false, juce::NotificationType::dontSendNotification);
            button.setEnabled(true);
        }
        else
        {
            button.setToggleState(true, juce::NotificationType::dontSendNotification);
            button.setEnabled(false);
        }
    }
    
    void buttonClicked(juce::Button* button) override
    {
        attachment.setValueAsCompleteGesture(index);
    }
    
    void buttonStateChanged(juce::Button* ) override
    {
        //DBG("button state changed");
    }
    
private:
    ptnz_gui::ToggleButtonWithText& button;
    juce::ParameterAttachment attachment;
    int index;
    bool ignoreCallbacks = false;
};

struct AttachedChoiceButton
{
    AttachedChoiceButton(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& label, int idx, juce::LookAndFeel_V4* custom_lnf=nullptr) :
        button(label),
        attachment(*apvts.getParameter(iD), button, idx)
    {
        if(custom_lnf)
            button.setLookAndFeel(custom_lnf);
    }
    ~AttachedChoiceButton()
    {
        button.setLookAndFeel(nullptr);
    }
    ptnz_gui::ToggleButtonWithText button;
    ChoiceButtonParameterAttachment attachment;
};

inline void drawDivider(juce::Graphics& g, const juce::Rectangle<float>& bounds, float x)
{
    g.setColour(colours::textColour.withAlpha(0.2f));
    g.drawLine(x, 15.f, x, bounds.getHeight() - 15.f, 1.f);
}

inline void placeSlider(juce::Slider& slider, juce::Rectangle<int>& initBounds)
{
    const int sliderWidth = 60;
    const int sliderHeight = 110;
        
    auto bounds = initBounds.withSizeKeepingCentre(sliderWidth, sliderHeight);
    
    bounds.removeFromTop(10);
    slider.setBounds(bounds);
}

inline void drawSliderTitle(juce::Graphics& g, juce::Slider& slider, const juce::String& text)
{
    const int vertpad = 7;
    auto x = slider.getX();
    auto y = slider.getY() - vertpad;
    auto w = slider.getWidth();
    auto h = 20;
    g.drawFittedText(text, x, y, w, h, juce::Justification::centred, 1);
}
