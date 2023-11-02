#pragma once
#include <JuceHeader.h>
#include "WaveShaper.h"
#include "ScopeComponent.h"
#include "consts.h"

class ShaperView : public juce::Component
{
public:
    ShaperView(float threshold, float knee, AudioBufferQueue<float>& queueToUse) :
    scopeComponent(queueToUse, threshold),
    waveShaper(threshold, knee, 512)
    {
        addAndMakeVisible(scopeComponent);
    }
    ShaperView(AudioBufferQueue<float>& queueToUse) : scopeComponent(queueToUse, 1.f),  waveShaper(512)
    {
        addAndMakeVisible(scopeComponent);
    }
    
    ~ShaperView()
    {
    }
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(consts::black);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 15.f);
    }
    void paintOverChildren(juce::Graphics& g) override
    {
        /* Paint Wave Shaper*/
        g.setColour (consts::lightindigo);
        
        juce::Path path;
        
        auto& lut = waveShaper.getWaveShaper();
        
        auto bounds = getLocalBounds().reduced(3.f);
        
        for (int i = 0; i < lut.size(); ++i)
        {
            float x = bounds.getX() + bounds.getWidth() * (static_cast<float>(i) / (lut.size() - 1));
            float y = bounds.getY() + bounds.getHeight() - lut[i] * bounds.getHeight() * 0.99f;
            
            if (i == 0)
                path.startNewSubPath(x, y);
            else
                path.lineTo(x, y);
        }
        g.strokePath(path, juce::PathStrokeType(3.0f));
    }
    
    void resized() override
    {
        scopeComponent.setBounds(getLocalBounds().reduced(3.f));
    }
    
    void updateGraph(float knee, float threshold)
    {
        waveShaper.createWaveShaper(threshold, knee);
        scopeComponent.updateThreshold(threshold);
        repaint(); 
    }
    
private:
    ScopeComponent<float> scopeComponent;
    WaveShaper waveShaper;
    
    juce::NormalisableRange<float> range {-100.0f, 12.f, 0.1 };
    juce::NormalisableRange<float> rangex {-100.0f, 6.f, 0.1 };
};
