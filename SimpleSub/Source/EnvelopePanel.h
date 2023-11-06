#pragma once

#include <JuceHeader.h>
#include "ptnz_gui/ptnz_gui.h"
#include "Parameters.h"
#include "EnvelopeViewer.h"
//==============================================================================
/*
*/
class EnvelopePanel  : public juce::Component
{
public:
    EnvelopePanel(juce::AudioProcessorValueTreeState& apvts) :
    attackSlider(apvts, param::toID(param::param_attack), param::toName(param::param_attack)),
    decaySlider(apvts, param::toID(param::param_decay), param::toName(param::param_decay)),
    sustainSlider(apvts, param::toID(param::param_sustain), param::toName(param::param_sustain)),
    releaseSlider(apvts, param::toID(param::param_release), param::toName(param::param_release)),
    curve_A_slider(apvts, param::toID(param::param_A_curve), param::toName(param::param_A_curve)),
    curve_DR_slider(apvts, param::toID(param::param_DR_curve), param::toName(param::param_DR_curve))
    {
        attackParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(param::toID(param::param_attack)));
        decayParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(param::toID(param::param_decay)));
        sustainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(param::toID(param::param_sustain)));
        releaseParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(param::toID(param::param_release)));
        attackCurveParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(param::toID(param::param_A_curve)));
        releaseCurveParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(param::toID(param::param_DR_curve)));
        
        auto valChanged = [this] () {
            envelopeViewer.update(attackParam->get(), decayParam->get(), sustainParam->get(), releaseParam->get(), attackCurveParam->get(), releaseCurveParam->get());
            envelopeViewer.repaint();
        };
        
        addAndMakeVisible(attackSlider.slider);
        attackSlider.slider.onValueChange = valChanged;
        
        addAndMakeVisible(decaySlider.slider);
        decaySlider.slider.onValueChange = valChanged;
        
        addAndMakeVisible(sustainSlider.slider);
        sustainSlider.slider.onValueChange = valChanged;
        
        addAndMakeVisible(releaseSlider.slider);
        releaseSlider.slider.onValueChange = valChanged;
        
        addAndMakeVisible(curve_A_slider.slider);
        curve_A_slider.slider.onValueChange = valChanged;
        
        addAndMakeVisible(curve_DR_slider.slider);
        curve_DR_slider.slider.onValueChange = valChanged;
        
        addAndMakeVisible(envelopeViewer);
        valChanged();
    }

    ~EnvelopePanel() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colours::darkgrey);
        g.drawRoundedRectangle(envelopeViewer.getBounds().expanded(6, 6).toFloat(), 5.f, 3.f);
        
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(12.f));
        g.drawFittedText("A", attackSlider.slider.getBounds(), juce::Justification::topLeft, 1);
        g.drawFittedText("D", decaySlider.slider.getBounds(), juce::Justification::topLeft, 1);
        g.drawFittedText("S", sustainSlider.slider.getBounds(), juce::Justification::topLeft, 1);
        g.drawFittedText("R", releaseSlider.slider.getBounds(), juce::Justification::topLeft, 1);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto w = bounds.getWidth();
        auto h = bounds.getHeight();
        
        envelopeViewer.setBounds(bounds.removeFromLeft(2 * w / 3).reduced(12, 12));
        
        w = bounds.getWidth();
        auto deltaX = w / 3;
        auto deltaY = h / 2;
        
        auto topBounds = bounds.removeFromTop(deltaY);
        
        attackSlider.slider.setBounds(topBounds.removeFromLeft(deltaX));
        sustainSlider.slider.setBounds(topBounds.removeFromLeft(deltaX));
        curve_A_slider.slider.setBounds(topBounds);

        decaySlider.slider.setBounds(bounds.removeFromLeft(deltaX));
        releaseSlider.slider.setBounds(bounds.removeFromLeft(deltaX));
        curve_DR_slider.slider.setBounds(bounds);
    }

private:
    ptnz_gui::AttachedSlider attackSlider;
    ptnz_gui::AttachedSlider decaySlider;
    ptnz_gui::AttachedSlider sustainSlider;
    ptnz_gui::AttachedSlider releaseSlider;
    
    ptnz_gui::AttachedSlider curve_A_slider;
    ptnz_gui::AttachedSlider curve_DR_slider;
    
    juce::AudioParameterFloat* attackParam;
    juce::AudioParameterFloat* decayParam;
    juce::AudioParameterFloat* sustainParam;
    juce::AudioParameterFloat* releaseParam;
    juce::AudioParameterFloat* attackCurveParam;
    juce::AudioParameterFloat* releaseCurveParam;
    
    EnvelopeViewer envelopeViewer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopePanel)
};
