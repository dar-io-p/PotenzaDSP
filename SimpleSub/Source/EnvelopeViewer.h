#pragma once

#include <JuceHeader.h>
#include "Parameters.h"
#include "SimpleSubLookAndFeel.h"

struct EnvelopeAttachment : public juce::Slider::Listener
{
public:
    EnvelopeAttachment(juce::RangedAudioParameter& param, juce::Slider& sliderToUse, std::function<void(float)>&& func) :
    slider(sliderToUse),
    functionToUse(std::move(func)),
    attachment(param, [this] (float f) { setValue (f); })
    {
        slider.addListener(this);
        attachment.sendInitialUpdate();
        slider.valueFromTextFunction = [&param] (const juce::String& text) { return (double) param.convertFrom0to1 (param.getValueForText (text)); };
        slider.textFromValueFunction = [&param] (double value) { return param.getText (param.convertTo0to1 ((float) value), 0); };
        slider.setDoubleClickReturnValue (true, param.convertFrom0to1 (param.getDefaultValue()));

        auto range = param.getNormalisableRange();

        auto convertFrom0To1Function = [range] (double currentRangeStart,
                                                double currentRangeEnd,
                                                double normalisedValue) mutable
        {
            range.start = (float) currentRangeStart;
            range.end = (float) currentRangeEnd;
            return (double) range.convertFrom0to1 ((float) normalisedValue);
        };

        auto convertTo0To1Function = [range] (double currentRangeStart,
                                              double currentRangeEnd,
                                              double mappedValue) mutable
        {
            range.start = (float) currentRangeStart;
            range.end = (float) currentRangeEnd;
            return (double) range.convertTo0to1 ((float) mappedValue);
        };

        auto snapToLegalValueFunction = [range] (double currentRangeStart,
                                                 double currentRangeEnd,
                                                 double mappedValue) mutable
        {
            range.start = (float) currentRangeStart;
            range.end = (float) currentRangeEnd;
            return (double) range.snapToLegalValue ((float) mappedValue);
        };

        juce::NormalisableRange<double> newRange { (double) range.start,
                                             (double) range.end,
                                             std::move (convertFrom0To1Function),
                                             std::move (convertTo0To1Function),
                                             std::move (snapToLegalValueFunction) };
        newRange.interval = range.interval;
        newRange.skew = range.skew;
        newRange.symmetricSkew = range.symmetricSkew;

        slider.setNormalisableRange (newRange);

        attachment.sendInitialUpdate();
        slider.valueChanged();
        slider.addListener (this);
    }
    
    void setValue(float newValue)
    {
        functionToUse(newValue);
        const juce::ScopedValueSetter<bool> svs (ignoreCallbacks, true);
        slider.setValue (newValue, juce::sendNotificationSync);
    }
    
    void sliderValueChanged(juce::Slider*) override
    {
        if (! ignoreCallbacks)
        {
            float val = (float) slider.getValue();
            attachment.setValueAsPartOfGesture (val);
            functionToUse(val);
        }
    }
    
    
    void sliderDragStarted(juce::Slider *) override { attachment.beginGesture(); }
    
    void sliderDragEnded(juce::Slider *) override { attachment.endGesture(); }
    
private:
    juce::Slider& slider;
    std::function<void(float)> functionToUse;
    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
    //WaveShaperDisplay& viewer;
};

class EnvelopeSlider : public juce::Slider
{
public:
    EnvelopeSlider() : juce::Slider() 
    {
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        setSliderStyle(juce::Slider::RotaryVerticalDrag);
    }
    
    void mouseDown(const juce::MouseEvent &event) override
    {
        juce::Slider::mouseDown(event);
        setMouseCursor(juce::MouseCursor::NoCursor);
        event.source.enableUnboundedMouseMovement(true);
        mousePos = event.source.getScreenPosition();
    }
    
    void mouseUp(const juce::MouseEvent &event) override
    {
        juce::Slider::mouseUp(event);
        juce::MouseInputSource src = juce::MouseInputSource(event.source);
        src.enableUnboundedMouseMovement(false);
        src.setScreenPosition(mousePos);
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
private:
    juce::Point<float> mousePos;
};

struct AttachedEnvelopeSlider
{
public:
    AttachedEnvelopeSlider(juce::RangedAudioParameter& param, std::function<void(float)>&& func, juce::LookAndFeel_V4* custom_lnf=nullptr) :
    slider(),
    attachment(param, slider, std::move(func))
    //attachment(*apvts.getParameter(iD), slider)
    {
        if(custom_lnf)
            slider.setLookAndFeel(custom_lnf);
    }
    
    ~AttachedEnvelopeSlider()
    {
        slider.setLookAndFeel(nullptr);
    }
    
    EnvelopeSlider slider;
    EnvelopeAttachment attachment;
};

class EnvelopeViewer  : public juce::Component
{
public:
    EnvelopeViewer(juce::AudioProcessorValueTreeState& apvts) :
    attackSlider(   *apvts.getParameter(param::toID(param::param_attack)),      [this] (float attack)   { return setAttack(attack); },  &lnf),
    decaySlider(    *apvts.getParameter(param::toID(param::param_decay)),       [this] (float decay)    { return setDecay(decay); },   &lnf),
    sustainSlider(  *apvts.getParameter(param::toID(param::param_sustain)),     [this] (float sustain)  { return setSustain(sustain); }, &lnf),
    releaseSlider(  *apvts.getParameter(param::toID(param::param_release)),     [this] (float release)  { return setRelease(release); }, &lnf)
    {
        envelope.prepare({100, 100, 1});
        addAndMakeVisible(attackSlider.slider);
        addAndMakeVisible(decaySlider.slider);
        addAndMakeVisible(sustainSlider.slider);
        addAndMakeVisible(releaseSlider.slider);
        
        setAttack   (attackSlider.slider.getValue());
        setDecay    (decaySlider.slider.getValue());
        setSustain  (sustainSlider.slider.getValue());
        setRelease  (releaseSlider.slider.getValue());
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        auto w = bounds.getWidth();
        auto h = bounds.getHeight();
        auto buttonDelta = w / 4;
        juce::Path p;
        int sustainCount = 0;
        
        auto pad = 10;
        auto bottompad = 40.f;
        auto buttonHeight = 30.f;
        
        p.startNewSubPath(pad, h - bottompad);

        update();
        envelope.reset();
        envelope.gate(true);
        for (int x = pad; x < w - pad; x++)
        {
            if (envelope.getState() == SubSynth::Envelope::Sustain)
            {
                sustainCount++;
            }
            if (sustainCount > 75)
                envelope.gate(false);
            float val = envelope.getNextValue();
            auto y = juce::jmap(val, 0.f, 1.f, (float) h - bottompad, (float) pad);
            p.lineTo(x, y);
        }
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::accent));
        g.strokePath(p, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved));
        
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::white).withAlpha(0.6f));
        g.drawLine(pad, h - buttonHeight, w - pad, h - buttonHeight, 0.5f);
        
        auto linepad = 7;
        g.drawLine(buttonDelta,     h - buttonHeight + linepad,    buttonDelta,    h - linepad, 0.5f);
        g.drawLine(2*buttonDelta,   h - buttonHeight + linepad,  2*buttonDelta,  h - linepad, 0.5f);
        g.drawLine(3*buttonDelta,   h - buttonHeight + linepad,  3*buttonDelta,  h - linepad, 0.5f);
        
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::white));
        g.setFont(13.f);
        g.drawFittedText("A:", attackSlider.slider.getBounds().reduced(5, 0), juce::Justification::centredLeft, 1);
        g.drawFittedText("D:", decaySlider.slider.getBounds().reduced(5, 0), juce::Justification::centredLeft, 1);
        g.drawFittedText("S:", sustainSlider.slider.getBounds().reduced(5, 0), juce::Justification::centredLeft, 1);
        g.drawFittedText("R:", releaseSlider.slider.getBounds().reduced(5, 0), juce::Justification::centredLeft, 1);
        
        g.setFont(10.f);
        g.drawFittedText("ms", attackSlider.slider.getBounds().reduced(5, 0), juce::Justification::centredRight, 1);
        g.drawFittedText("ms", decaySlider.slider.getBounds().reduced(5, 0), juce::Justification::centredRight, 1);
        g.drawFittedText("dB", sustainSlider.slider.getBounds().reduced(5, 0), juce::Justification::centredRight, 1);
        g.drawFittedText("ms", releaseSlider.slider.getBounds().reduced(5, 0), juce::Justification::centredRight, 1);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        
        auto buttonWidth = bounds.getWidth() / 4;
        int buttonHeight = 30.f;
        
        bounds = bounds.removeFromBottom(buttonHeight);
        attackSlider.slider.setBounds(bounds.removeFromLeft(buttonWidth).reduced(3, 2));
        decaySlider.slider.setBounds(bounds.removeFromLeft(buttonWidth).reduced(3, 2));
        sustainSlider.slider.setBounds(bounds.removeFromLeft(buttonWidth).reduced(3, 2));
        releaseSlider.slider.setBounds(bounds.reduced(3, 2));
    }
    
    void setAttack(float newAttack)
    {
        a = newAttack;
        repaint();
    }
    void setDecay(float newDecay)
    {
        d = newDecay;
        repaint();
    }
    void setSustain(float newSustain)
    {
        s = newSustain;
        repaint();
    }
    void setRelease(float newRelease)
    {
        r = newRelease;
        repaint();
    }
    
    void set_a_curve(float newVal)
    {
        a_curve = newVal;
        repaint();
    }
    
    void set_dr_curve(float newVal)
    {
        dr_curve = newVal;
        repaint();
    }
    
    void update()
    {
        envelope.update(a, d, s, r);
        envelope.setTargetRatioA(a_curve);
        envelope.setTargetRatioDR(dr_curve);
    }

private:
    EnvelopeLookAndFeel lnf;
    SubSynth::Envelope envelope;
    
    AttachedEnvelopeSlider attackSlider;
    AttachedEnvelopeSlider decaySlider;
    AttachedEnvelopeSlider sustainSlider;
    AttachedEnvelopeSlider releaseSlider;
    float a,d,s,r;
    float a_curve, dr_curve;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeViewer)
};
