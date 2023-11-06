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
class WaveShaperDisplay  : public juce::Component
{
public:
    WaveShaperDisplay()
    {
        waveShapers[param::CubicAlgo] = param::getWaveShaper(param::CubicAlgo);
        waveShapers[param::TanhAlgo] = param::getWaveShaper(param::TanhAlgo);
        waveShapers[param::SinAlgo] = param::getWaveShaper(param::SinAlgo);
    }
    
    WaveShaperDisplay(param::distortionIndices newIndex)
    {
        waveShapers[param::CubicAlgo] = param::getWaveShaper(param::CubicAlgo);
        waveShapers[param::TanhAlgo] = param::getWaveShaper(param::TanhAlgo);
        waveShapers[param::SinAlgo] = param::getWaveShaper(param::SinAlgo);
        update(newIndex);
    }

    void paint (juce::Graphics& g) override
    {
        auto d = juce::jmin(getWidth(), getHeight());
        juce::Rectangle<int> bounds = juce::Rectangle<int>(0, 0, d, d);
        bounds = bounds.withCentre(getLocalBounds().getCentre());
        
        const auto startX = bounds.getX();
        const auto endX = bounds.getRight();
        
        const auto startY = bounds.getY();
        const auto endY = bounds.getBottom();
        
        juce::Path p;
        for (int i = startX; i < endX; i++) {
            float x = juce::jmap((float)i, (float)startX, (float)endX-1, -3.f, 3.f);
            float y = waveShapers[currentIndex](x);
            int y_pixels = (int)juce::jmap(y, -1.2f, 1.2f, (float)endY, (float)startY);
            
            if (i == 0) p.startNewSubPath(i, y_pixels);
            p.lineTo(i, y_pixels);
        }
        p.startNewSubPath(0, 0);
        g.setColour(juce::Colours::white);
        g.strokePath(p, juce::PathStrokeType(2.f, juce::PathStrokeType::curved));
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds.toFloat().reduced(1), 1.f, 3.f);
    }

    void resized() override
    {
        
    }
    
    void update(int newIndex)
    {
        currentIndex = newIndex;
        repaint();
    }
    
private:
    int currentIndex;
    std::array<std::function<float(float)>, 3> waveShapers;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperDisplay)
};
