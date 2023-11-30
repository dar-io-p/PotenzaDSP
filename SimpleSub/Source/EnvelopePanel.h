#pragma once

#include <JuceHeader.h>
#include "ptnz_gui/ptnz_gui.h"
#include "Parameters.h"
#include "EnvelopeViewer.h"
#include "SimpleSubLookAndFeel.h"

//==============================================================================
/*
*/
class EnvelopePanel  : public juce::Component
{
public:
    EnvelopePanel(juce::AudioProcessorValueTreeState& apvts) :
    curve_A_slider(apvts, param::toID(param::param_A_curve), param::toName(param::param_A_curve), &lnf),
    curve_DR_slider(apvts, param::toID(param::param_DR_curve), param::toName(param::param_DR_curve), &lnf),
    envelopeViewer(apvts)
    {
        addAndMakeVisible(curve_A_slider.slider);
        addAndMakeVisible(curve_DR_slider.slider);
        
        addAndMakeVisible(envelopeViewer);
        
        curve_A_slider.slider.onValueChange = [this] () {
            envelopeViewer.set_a_curve(curve_A_slider.slider.getValue());
        };
        
        curve_DR_slider.slider.onValueChange = [this] () {
            envelopeViewer.set_dr_curve(curve_DR_slider.slider.getValue());
        };
        
        envelopeViewer.set_a_curve(curve_A_slider.slider.getValue());
        envelopeViewer.set_dr_curve(curve_DR_slider.slider.getValue());
    }

    ~EnvelopePanel() override
    {
    }

    void paint (juce::Graphics& g) override
    {
//        g.setColour(lnf.findColour(ptnz_gui::colour_ids::outlineColour));
//        g.fillRoundedRectangle(envelopeViewer.getBounds().expanded(6, 6).toFloat(), 8.f);
//        g.setColour(lnf.findColour(ptnz_gui::colour_ids::secondaryBackground));
//        g.fillRoundedRectangle(envelopeViewer.getBounds().expanded(2, 2).toFloat(), 8.f);
        
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::white).withAlpha(0.6f));
        g.setFont(ptnz_gui::styles::getPlainFont());
        g.drawFittedText("A CURVE", curve_A_slider.slider.getX(), curve_A_slider.slider.getY() - 16, curve_A_slider.slider.getWidth(), 20, juce::Justification::centred, 1);
        g.drawFittedText("D/R CURVE", curve_DR_slider.slider.getX(), curve_DR_slider.slider.getY() - 16, curve_DR_slider.slider.getWidth(), 20, juce::Justification::centred, 1);
        
        drawDivider(g, getLocalBounds().toFloat(), (float)envelopeViewer.getRight() + 5.f);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto w = bounds.getWidth();
        auto h = bounds.getHeight();
        
        envelopeViewer.setBounds(bounds.removeFromLeft(5 * w / 6).reduced(5.f));
        
        auto sliderHeight = 60;
        auto topBounds = bounds.removeFromTop(h / 2);
        bounds.removeFromBottom(5);
        curve_A_slider.slider.setBounds(topBounds.removeFromBottom(sliderHeight));

        curve_DR_slider.slider.setBounds(bounds.removeFromBottom(sliderHeight));
    }

private:
    SimpleSubLookAndFeel lnf;
    
    ptnz_gui::AttachedSlider curve_A_slider;
    ptnz_gui::AttachedSlider curve_DR_slider;
    
    EnvelopeViewer envelopeViewer;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopePanel)
};
