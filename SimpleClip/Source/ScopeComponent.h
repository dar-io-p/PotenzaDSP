#pragma once
#include <JuceHeader.h>

//==============================================================================
template <typename SampleType>
class ScopeComponent  : public juce::Component,
                        private juce::Timer
{
public:
    using Queue = AudioBufferQueue<SampleType>;

    //==============================================================================
    ScopeComponent (Queue& queueToUse, float t)
        : audioBufferQueue (queueToUse), threshold(t)
    {
        sampleData.fill (SampleType (0));
        setFramesPerSecond (40);
    }
    
    ~ScopeComponent()
    {
        
    }
    //==============================================================================
    void setFramesPerSecond (int framesPerSecond)
    {
        jassert (framesPerSecond > 0 && framesPerSecond < 1000);
        startTimerHz (framesPerSecond);
    }

    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        float t = threshold.load();
        
        plot(sampleData.data(), sampleData.size(), g, getLocalBounds().toFloat(), t);
        
        //show peak level
        g.setColour(juce::Colours::grey);
        g.fillRect(0, peak, getWidth(), 2);
    }
    
    void mouseDown (const juce::MouseEvent &event) override
    {
        peak = 500;
    }

    //==============================================================================
    void resized() override {}
    
    void updateThreshold(float t) { threshold.store(t); }

private:
    //==============================================================================
    Queue& audioBufferQueue;
    std::array<SampleType, Queue::bufferSize> sampleData;
    std::atomic<float> threshold;
    
    //Arbitrarily high number
    int peak = 500;
    
    //==============================================================================
    void timerCallback() override
    {
        audioBufferQueue.pop (sampleData.data());

        repaint();
    }

    //==============================================================================
    void plot (const SampleType* data,
                      size_t numSamples,
                      juce::Graphics& g,
                      juce::Rectangle<SampleType> rect,
                      SampleType t)
    {
        auto w = rect.getWidth();
        auto h = rect.getHeight();
        
        for (size_t i = 0; i < numSamples; ++i)
        {
            const auto y_pixels = h - h * std::abs(data[i]);
            const auto t_pixels = h - t * h;
            const auto x = juce::jmap (SampleType (i), SampleType (0), SampleType (numSamples - 1), SampleType (0), SampleType (w));
                                        
            if (y_pixels < peak)
            {
                peak = y_pixels;
            }
            
            if (y_pixels < t_pixels)
            {
                g.setColour(juce::Colours::white);
                g.drawVerticalLine(x, t_pixels, getHeight());
                g.setColour(juce::Colours::red);
                g.drawVerticalLine(x, y_pixels, t_pixels);
            }
            else {
                g.setColour(juce::Colours::white);
                g.drawVerticalLine(x, y_pixels, getHeight());
            }
        }
    }
};
