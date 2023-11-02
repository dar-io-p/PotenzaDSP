#include "Clipper.h"

Clipper::Clipper(int numPoints, float t, float k) : shaper(t, k, numPoints){
    threshold.store(t);
    knee.store(k);
}

void Clipper::process(juce::dsp::AudioBlock<float> buffer) {
    for (int c = 0; c < buffer.getNumChannels(); c++)
    {
        auto channelData = buffer.getChannelPointer(c);
        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            float sample = channelData[n];
            //get output apply sign
            channelData[n] =  shaper.getSample(sample) * ((sample > 0) - (sample < 0));
        }
    }
}


