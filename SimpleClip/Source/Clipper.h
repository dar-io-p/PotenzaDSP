/*
  ==============================================================================

    Clipper.h
    Created: 17 Aug 2023 10:06:13am
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WaveShaper.h"

class Clipper
{
public:
    Clipper(int numPoints, float t, float k);
    
    void process(juce::dsp::AudioBlock<float> buffer);
        
    void thresholdChanged(float newValue) {
        threshold.store(newValue);
        shaper.createWaveShaper(threshold, knee);
    }
    void kneeChanged(float newValue) {
        knee.store(newValue);
        shaper.createWaveShaper(threshold, knee);
    }
    
private:
    std::atomic<float> threshold;
    std::atomic<float> knee;
    WaveShaper shaper;
};
