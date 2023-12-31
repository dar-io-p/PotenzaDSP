/*
  ==============================================================================

    BufferView.h
    Created: 8 Sep 2023 11:01:57am
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CircularBuffer.h"
#include "WheelUpLookAndFeel.h"

class BufferView  : public juce::Component, public juce::Timer
{
public:
    BufferView(CircularBuffer& bufferToUse) : buffer(bufferToUse)
    {
        startTimerHz(30);
    }

    ~BufferView() override
    {
        
    }

    void paint (juce::Graphics& g) override
    {

        //g.fillAll (juce::Colours::black);   // clear the background

        //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour(lnf.findColour(ptnz_gui::colour_ids::white));
        plot(g);
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
    }

private:
    WheelUpLookAndFeel lnf;
    CircularBuffer& buffer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BufferView)
    
    void plot(juce::Graphics& g)
    {
        float W = static_cast<float> (getWidth());
        float H = static_cast<float> (getHeight());
        
        std::vector<float>& data = buffer.getBuffer();
        int samplesPerPixel = buffer.getBufferLength() / W;
        int window = 0;
        int pixel_x = 0;
        float accum = 0;
        for (int i = 0; i < buffer.getBufferLength(); i++)
        {
            if (window < samplesPerPixel)
            {
                //take average
                accum += fabs (data[i]) / samplesPerPixel;
                window++;
            }
            else{
                float h = H - (accum * 2.f) * H;
                g.drawVerticalLine(pixel_x, h / 2, H - h / 2);
                window = 0;
                accum = 0.f;
                pixel_x++;
            }
        }
        
        float index = juce::jmap(buffer.getIndex(), 0.0f, static_cast<float> (buffer.getBufferLength()), 0.0f, W);
        
        g.setColour(lnf.findColour(ptnz_gui::colour_ids::accent));
        g.drawVerticalLine(index, 5.f, H - 5.f);
        g.drawLine(index, 5.f, index, H - 5.f, 2.f);
    }
    
    void timerCallback() override
    {
        repaint();
    }
};
