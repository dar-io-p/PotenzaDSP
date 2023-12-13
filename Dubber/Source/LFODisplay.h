#pragma once
#include <JuceHeader.h>

class LFODisplay : public juce::Component, public juce::Timer
{
public:
    LFODisplay(std::function<param::lfo_data(void)>&& callback) :
    getterFunction(std::move(callback))
    {
        startTimerHz(60);
        lfoData = getterFunction();
    }
    
    void timerCallback() override
    {
        lfoData = getterFunction();
        repaint();
    }
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        const int W = getWidth();
        
        int x = (int)juce::jmap(lfoData.phase, 0.f, juce::MathConstants<float>::twoPi, 0.f, (float) W);
        
        //bounds.reduce(3.f, 3.f);
        
        auto yzero = getHeight() / 2;
        float y = yzero - 0.9 * (lfoData.magnitude * lfoData.val * getHeight() / 2);
        
        //draw Lines
        g.setColour(cols::green);
        g.drawLine(0.f, yzero, bounds.getWidth(), yzero, 2.f);
        if(yzero < y)
            g.drawVerticalLine(x, yzero, y);
        else
            g.drawVerticalLine(x, y, yzero);
        
        //Linear interpolate from last sampled point
        auto deltax = x - lastIndex;
        deltax = deltax >= 0 ? deltax : deltax + W;
        lfoBuffer[x] = y;
        auto deltay = (lfoBuffer[x] - lfoBuffer[lastIndex]) / deltax;
        
        float tempY = lfoBuffer[lastIndex];
        while (lastIndex != x) {
            lfoBuffer[lastIndex] = tempY;
            tempY+=deltay;
            lastIndex++;
            if (lastIndex >= W) lastIndex -= W;
        }
        
        lastIndex = x;
        
        //trace path
        juce::Path p;
        p.startNewSubPath(0.f, yzero);
        for (int i = 0; i < getWidth(); i++) {
            if(lfoBuffer[i] != 0)
                p.lineTo(i, lfoBuffer[i]);
            
        }
        g.setColour(cols::red);
        g.strokePath(p, juce::PathStrokeType(2.f));
        
        int thumbSize = 6;
        auto thumbBounds = juce::Rectangle<float>(0, 0, thumbSize, thumbSize).withCentre({(float)x, y});
        g.setColour(cols::green);
        g.fillEllipse(thumbBounds);
    }
    
    void resized() override
    {
//        const auto bounds = getLocalBounds().toFloat();
        lfoBuffer.resize(getWidth());
//        grad = juce::ColourGradient {
//            juce::Colours::red.withAlpha(1.0f),
//            bounds.getBottomLeft(),
//            juce::Colours::red,
//            bounds.getTopLeft(),
//            false
//        };
    }
private:
    std::vector<float> lfoBuffer;
    int lastIndex = 0;
    param::lfo_data lfoData;
//    juce::ColourGradient grad;
    std::function<param::lfo_data(void)> getterFunction;
    
    
};
