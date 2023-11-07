#pragma once
#include <JuceHeader.h>

class SpeedEnvelope
{
public:
    enum Direction {
        forwards = 1,
        backwards = -1
    };
    
    SpeedEnvelope()
    {
        index = 0;
    }
    
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
    }
    
    void updateParams(double newBPM, float startSpeed, float duration, float pow)
    {
        start = startSpeed;
        bpm = newBPM;
        timeInSamples = duration * ((60.0 / bpm) * 4 * sampleRate);
        power = pow;
    }
    
    float getNextValue(bool trig=true)
    {
        return trig ? (powerScale(index++) * direction) : 1.0f;
    }
    
    void reset()
    {
        index = 0;
    }
    
    float powerScale(float value) {
        float kMinPower = 0.01f;

        if (value >= timeInSamples) {
            return 0;
        }
        
        if (fabsf(power) < kMinPower){
          return (start - (start / timeInSamples) * value);
        }
        
        float numerator = exp(power * (value / timeInSamples)) - 1.0f;
        float denominator = exp(power) - 1.0f;
        return start * (-(numerator / denominator) + 1);
    }
    
    void changeDirection(Direction dir)
    {
        direction = dir;
    }
    
private:
    int index;
    
    float start;
    float timeInSamples;
    float power;
        
    double bpm;
    double sampleRate;
    Direction direction = backwards;
};
