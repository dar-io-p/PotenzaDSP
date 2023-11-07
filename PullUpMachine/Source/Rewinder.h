#pragma once
#include <JuceHeader.h>
#include "CircularBuffer.h"
#include "SpeedEnvelope.h"
class Rewinder
{
public:
    Rewinder() : envelope()
    {
        
    }
    
    void prepare(double newSampleRate)
    {
        for (int c = 0; c<2; c++)
        {
            cBuffer[c].prepare(newSampleRate);
            envelope[c].prepare(newSampleRate);
        }
    }
    
    void updateParams(double bpm, float startSpeed, float duration, float power, float bufferLength)
    {
        for (unsigned c = 0; c < 2; c++)
        {
            envelope[c].updateParams(bpm, startSpeed, duration, power);
            cBuffer[c].updateParams(bpm, bufferLength);
        }
    }
    
    void process(juce::AudioBuffer<float> buffer, bool trig)
    {
        int numSamples = buffer.getNumSamples();
        int numChannels = buffer.getNumChannels();

        for (unsigned c = 0; c < numChannels; c++)
        {
            float* data = buffer.getWritePointer(c);
            for (unsigned i = 0; i < numSamples; i++)
            {
                if (!trig)
                {
                    cBuffer[c].store(data[i]);
                }
                else{
                    data[i] = cBuffer[c].read(envelope[c].getNextValue());
                }
            }
        }
    }
    
    void reset()
    {
        for (auto& env : envelope)
            env.reset();
        
        for  (auto& buff : cBuffer)
            buff.reset();
    }
    
    void changeDirection(SpeedEnvelope::Direction dir)
    {
        for (auto& env : envelope)
            env.changeDirection(dir);
    }
    
    CircularBuffer& getBuffer() { return cBuffer[0]; }
private:
    CircularBuffer cBuffer[2];
    SpeedEnvelope envelope[2];
};
