/*
  ==============================================================================

    StereoLevelMeter.h
    Created: 28 Aug 2023 12:50:07pm
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LevelMeter.h"

class MultiLevelMeter : public juce::Component, juce::Timer
{
public:
    struct RMS_Peak_Info {
        float rmsDB;
        float rmsLin;
        
        float peakDB;
        float peakLin;

    };
    MultiLevelMeter(std::function<RMS_Peak_Info()>&& infoFunction, bool onLeft) :
    getInfo(std::move(infoFunction)),
    meterDisplayLeft(onLeft)
    {
        addAndMakeVisible(meter);
        
        startTimerHz(30);
        range.setSkewForCentre(-12.f);
    }
    
    ~MultiLevelMeter()
    {
        
    }
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto lineBounds = meterDisplayLeft ? bounds.removeFromLeft(20) : bounds.removeFromRight(20);
        drawLines(g, lineBounds);
    }
    
    void paintOverChildren(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        meterDisplayLeft ? bounds.removeFromLeft(20) : bounds.removeFromRight(20);
        g.setColour(juce::Colours::grey);
    
        if (meterDisplayLeft)
            g.drawVerticalLine(bounds.getX(), bounds.getY(), bounds.getBottom());
        else
            g.drawVerticalLine(bounds.getRight(), bounds.getX(), bounds.getBottom());
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        meterDisplayLeft ? bounds.removeFromLeft(20) : bounds.removeFromRight(20);
        meter.setBounds(bounds);
    }
    
    void drawLines(juce::Graphics& g, juce::Rectangle<float>& bounds)
    {
        float lines[8] = {-100.f, -48.f, -24.f, -12.f, -6.f, 0.f, 6.f, 12.f};
        
        for (int i = 0; i < 8; i++)
        {
            int norm = (1 - range.convertTo0to1(lines[i])) * bounds.getHeight();
            int w = i % 2 == 0 ? bounds.getWidth() / 2 : bounds.getWidth() / 4;
            int y = (int)(bounds.getY() + norm);
            g.setColour(juce::Colours::white);
            int x = meterDisplayLeft ? bounds.getRight() - w : bounds.getX();
            
            g.fillRect(x, y, w, 2);
            
            x = bounds.getX();
            g.setFont(juce::Font(9.f));
            g.drawFittedText(juce::String(lines[i], 0), x, y+2, bounds.getWidth(), 8 , meterDisplayLeft ?  juce::Justification::centredLeft : juce::Justification::centredRight, 1);
        }
    }
    
    float getPeak() { return prevPeak; }
    
private:
    std::function<RMS_Peak_Info()> getInfo;
    LevelMeter meter;
    int meterHeight;
    float prevPeak = -100.f;
    
    bool meterDisplayLeft;
    
    juce::NormalisableRange<float> range {-100.0f, 12.f, 0.1 };
    
    void timerCallback() override
    {
        RMS_Peak_Info info = getInfo();
        meter.update(info.rmsDB + 3.f, info.peakDB);
        
        if (info.peakDB > prevPeak)
        {
            prevPeak = info.peakDB;
        }

    }
};
