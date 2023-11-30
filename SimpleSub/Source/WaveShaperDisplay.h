/*
  ==============================================================================

    WaveShaperDisplay.h
    Created: 5 Oct 2023 3:13:45pm
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

//==============================================================================
/*
*/
class WaveShaperDisplay  : public juce::Component, public juce::AudioProcessorValueTreeState::Listener
{
public:
    WaveShaperDisplay(juce::AudioProcessorValueTreeState& apvts)
    : state(apvts)
    {
        currentIndex = apvts.getParameter(param::toID(param::param_distorionType))->getValue() * (param::NumAlgo - 1);
        
        waveShapers[param::CubicAlgo] = param::getWaveShaper(param::CubicAlgo);
        waveShapers[param::TanhAlgo] = param::getWaveShaper(param::TanhAlgo);
        waveShapers[param::SinAlgo] = param::getWaveShaper(param::SinAlgo);
        
        apvts.addParameterListener(param::toID(param::param_distorionType), this);
    }
    
    ~WaveShaperDisplay()
    {
        state.removeParameterListener(param::toID(param::param_distorionType), this);
    }

    void paint (juce::Graphics& g) override
    {
        auto d = juce::jmin(getWidth(), getHeight());
        juce::Rectangle<int> bounds = juce::Rectangle<int>(0, 0, d, d);
        bounds = bounds.withCentre(getLocalBounds().getCentre());
        
        //fill background
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::mainBackground));
        g.fillRoundedRectangle(bounds.toFloat().reduced(1), 1.f);
        
        //Dashed quadrants
        auto centre = bounds.toFloat().getCentre();
        const float dashLengths[2] = {4.f, 5.f};
        const int numDashLengths = 2;
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::outlineColour).withAlpha(0.5f));
        
        g.drawDashedLine({0.f,              centre.getY(),          (float)bounds.getWidth(),   centre.getY()},             dashLengths, numDashLengths, 2.0f, 0);
        g.drawDashedLine({centre.getX(),    (float)bounds.getY(),   centre.getX(),              (float)bounds.getBottom()}, dashLengths, numDashLengths, 2.0f, 0);

        //Calculate and create path for waveshaper
        const auto startX = bounds.getX();
        const auto endX = bounds.getRight();
        
        const auto startY = bounds.getY();
        const auto endY = bounds.getBottom();
        
        juce::Path p;
        int index = currentIndex;
        for (int i = startX; i < endX; i++) {
            float x = juce::jmap((float)i, (float)startX, (float)endX-1, -3.f, 3.f);
            float y = waveShapers[index](x);
            int y_pixels = (int)juce::jmap(y, -1.2f, 1.2f, (float)endY, (float)startY);
            
            if (i == 0) p.startNewSubPath(i, y_pixels);
            p.lineTo(i, y_pixels);
        }
    
        //stroke the blue waveshaper
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::secondaryAccent));
        g.strokePath(p, juce::PathStrokeType(3.f, juce::PathStrokeType::curved));
        
        //outline
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::outlineColour));
        g.drawRoundedRectangle(bounds.toFloat().reduced(1), 1.f, 3.f);
    }

    void resized() override
    {
        
    }
    
//    void update(int newIndex)
//    {
//        currentIndex = newIndex;
//        repaint();
//    }
    
    void parameterChanged(const juce::String &parameterID, float newValue) override
    {
        if((int)newValue != currentIndex)
        {
            currentIndex = (int) newValue;
        }
        repaint();
    }
    
private:
    juce::AudioProcessorValueTreeState& state;
    SimpleSubLookAndFeel lnf;
    int currentIndex;
    std::array<std::function<float(float)>, 3> waveShapers;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperDisplay)
};
