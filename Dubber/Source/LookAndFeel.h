#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

namespace fonts
{
    //const juce::Font labelFont  = juce::Font("Futura", 14.f, juce::Font::plain);
static const juce::Typeface::Ptr getTypeface()
{
    static auto typeface = juce::Typeface::createSystemTypefaceFor (BinaryData::Futura_Heavy_font_ttf, BinaryData::Futura_Heavy_font_ttfSize);
    return typeface;
}

static const juce::Font getLabelFont()     {
    return juce::Font(getTypeface()).withHeight(14.f);
}
//static const juce::Font getTitleFont()     {
//    return juce::Font(getTypeface()).withHeight(16.f);
//}
//static const juce::Font getPlainFont()     {
//    return juce::Font(getTypeface()).withHeight(12.f);
//}
//static const juce::Font getBigTitleFont()  {
//    return juce::Font(getTypeface()).withHeight(25.f);
//}
}

namespace cols
{
const static juce::Colour red = juce::Colour(0xffff2908);
const static juce::Colour gold = juce::Colour(0xffebdd17);
const static juce::Colour gold_dark = juce::Colour(0xffc4ba27);
const static juce::Colour green = juce::Colour(0xff42b027);
const static juce::Colour black = juce::Colour(0xff1a1912);
}

class ChoiceButton : public juce::Component
{
public:
    ChoiceButton()
    {
        
    }
    void update(int val)
    {
        currentIndex = val;
        repaint();
    }
    
    void update()
    {
        currentIndex += 1;
        if (currentIndex >= numOptions)
            currentIndex -= numOptions;
    }
    
    int getValue()
    {
        return currentIndex;
    }
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds.reduced(2.f), 5.f, 2.f);
        
        auto delta = getHeight() / (numOptions);
        
        g.setColour(cols::green);
        
        auto x = 0;
        auto y = currentIndex * delta;
        auto w = getWidth();
        auto h = delta;
        
        auto thumbBounds = juce::Rectangle<float>(x, y, w, h);
        
        g.fillRoundedRectangle(thumbBounds, 5.f);
        
        g.setColour(cols::gold);
        
        thumbBounds.reduce(5.f, h / 4);
        delta = thumbBounds.getHeight() / 5;
        g.fillRoundedRectangle(thumbBounds.removeFromTop(delta), 2.f);
        thumbBounds.removeFromTop(delta);
        g.fillRoundedRectangle(thumbBounds.removeFromTop(delta), 2.f);
        thumbBounds.removeFromTop(delta);
        g.fillRoundedRectangle(thumbBounds.removeFromTop(delta), 2.f); 
        
    }
    
    void setNumOptions(int n) {numOptions = n;}
private:
    int numOptions = 1;
    int currentIndex = 0;
};

struct ChoiceParameterAttachment : private juce::MouseListener
{
public:
    ChoiceParameterAttachment(juce::RangedAudioParameter& param, ChoiceButton& buttonToUse) :
    button(buttonToUse),
    attachment(param, [this] (float f) { setValue (f); })
    {
        auto range = param.getNormalisableRange();
        button.setNumOptions((int)range.end + 1);
        attachment.sendInitialUpdate();
        button.addMouseListener(this, false);
    }
    
    void setValue(float newValue)
    {
        button.update(newValue);
    }
    
    void mouseDown(const juce::MouseEvent &event) override
    {
        button.update();
        if (! ignoreCallbacks)
            attachment.setValueAsCompleteGesture(button.getValue());
    }
    
private:
    ChoiceButton& button;
    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
};


struct AttachedChoiceButton
{
    AttachedChoiceButton(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& name) :
    button(),
    attachment(*apvts.getParameter(iD), button)
    {
    }
    ChoiceButton button;
    ChoiceParameterAttachment attachment;
};


class DubberLookAndFeel : public ptnz_gui::LNF
{
public:
    DubberLookAndFeel() : ptnz_gui::LNF()
    {
        setColour(ptnz_gui::accent, juce::Colours::red);
        juce::LookAndFeel::setDefaultLookAndFeel(this);
        setDefaultSansSerifTypeface(fonts::getTypeface());
    }
    
    juce::Slider::SliderLayout getSliderLayout(juce::Slider &slider) override
    {
        //auto sliderWidth = 80;
        auto textSpacing = 25;
        juce::Rectangle<int> sliderBounds = slider.getLocalBounds();
        sliderBounds.removeFromTop(textSpacing);

        if(slider.getTextBoxPosition() == juce::Slider::TextBoxBelow)
        {
            auto textBoxHeight = 15;
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
};

class DubButton : public juce::ToggleButton
{
public:
    DubButton(const juce::String& buttonText) :
    juce::ToggleButton(buttonText)
    {
        setLookAndFeel(&lnf);
    }
    
    ~DubButton()
    {
        setLookAndFeel(nullptr);
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();
        juce::Colour bgColour = cols::gold;
        if (getToggleState())
            bgColour = cols::gold;// Set a different color for the "on" state
        if (isMouseOver())
            bgColour = cols::gold;
        if(isMouseButtonDown()){
            bounds.reduce(4, 4);
            bgColour = cols::gold_dark;
        }
        g.setColour(bgColour);
        g.fillEllipse(bounds);
        g.setColour(cols::black);
        g.drawEllipse(bounds, 4.f);
        
        
        if(getToggleState())
        {
            g.setColour(cols::green);
            //g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(5), 5.f, 2.f);
            g.drawEllipse(bounds.reduced(8), 4.f);
        }
        
        g.setColour(cols::red);
        g.setFont(19.f);
        g.drawFittedText(getButtonText(), bounds.toNearestInt(), juce::Justification::centred, 1);
    }
private:
    DubberLookAndFeel lnf;
};

struct AttachedDubButton
{
    AttachedDubButton(juce::AudioProcessorValueTreeState& apvts, const juce::String& iD, const juce::String& label) :
        button(label),
        attachment(*apvts.getParameter(iD), button, nullptr)
    {

    }
    ~AttachedDubButton()
    {
    }
    DubButton button;
    juce::ButtonParameterAttachment attachment;
};
