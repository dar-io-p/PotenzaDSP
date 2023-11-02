/*
  ==============================================================================

    LevelMeter.h
    Created: 21 Aug 2023 10:38:17am
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "consts.h"

class LevelMeter : public juce::Component
{
public:
    LevelMeter()
    {
        level.reset(24, 0.6);
        peak.reset(24, 3.0);
        range.setSkewForCentre(-12.f);
    }
    
    ~LevelMeter()
    {
    }
    
    void paint(juce::Graphics& g) override
    {        
        const auto rms_db = level.getNextValue();
        const auto peak_db = peak.getNextValue();
        
        const auto rms_norm = range.convertTo0to1(rms_db);
        const auto peak_norm = range.convertTo0to1(peak_db);
        
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds, 1.f, 2.f);
        
        g.setColour(juce::Colours::black);
        g.fillRect(bounds);
        
        g.setGradientFill(gradient);
        
        const auto rms_scaled = rms_norm * getHeight();
        g.fillRect(bounds.removeFromBottom(rms_scaled));
        
        const int peak_scaled = (1.0 - peak_norm) * getHeight();
        g.setColour(juce::Colours::white);
        g.fillRect((int)bounds.getX(), peak_scaled, (int)bounds.getWidth(), 2);
        
//        const int zero_db = (1 - range.convertTo0to1(0.f)) * getHeight();
//        g.setColour(juce::Colours::blue);
//        g.fillRect((int)bounds.getX(), zero_db, (int)bounds.getWidth(), 2);
//        
//        const int m12_db = (1 - range.convertTo0to1(-12.f)) * getHeight();
//        g.setColour(juce::Colours::green);
//        g.fillRect((int)bounds.getX(), m12_db, (int)bounds.getWidth(), 2);
//        //g.drawFittedText(juce::String(juce::Decibels::gainToDecibels(highestPeak), 2), 0, 0, getWidth(), v, juce::Justification::centred, 1);

    }
    
    void update(float level_in, float peak_in)
    {
        if (level_in > level.getCurrentValue())
            level.setCurrentAndTargetValue(level_in);
        else
            level.setTargetValue(level_in);
        
        if (peak_in > peak.getCurrentValue())
        {
            peak.setCurrentAndTargetValue(peak_in);
            //highestPeak = peak_in;
        }
        else
            peak.setTargetValue(peak_in);

        repaint();
    }
    
    void resized() override
    {
        const auto bounds = getLocalBounds().toFloat();
        gradient = juce::ColourGradient {
            juce::Colours::green,
            bounds.getBottomLeft(),
            juce::Colours::red,
            bounds.getTopLeft(),
            false
        };
        gradient.addColour(0.5, juce::Colours::yellow);
    };
    
    
private:
    juce::NormalisableRange<float> range {-100.0f, 12.f, 0.1};

    juce::LinearSmoothedValue<float> level, peak;
    juce::ColourGradient gradient;
    bool meterDisplayLeft = true;
    //float highestPeak = 0;
};

class LevelSlider : juce::Slider
{
public:
    LevelSlider()
    {
        
    }
    
    void paint(juce::Graphics& g)
    {
        
    }
private:
};
