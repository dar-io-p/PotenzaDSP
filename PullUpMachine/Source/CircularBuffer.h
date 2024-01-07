#pragma once
#include <JuceHeader.h>

#define MAX_BUFFER_SECONDS 10

class CircularBuffer
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        bufferLength = static_cast<int>(MAX_BUFFER_SECONDS * sampleRate);
        buffer.resize(bufferLength);
        residual.reset(sampleRate, 0.001);
        i = 0;
        cut = false;
        gain.reset(sampleRate, 0.05);
    }
    
    float read(float speed)
    {
        if (cut)
            return residual.getNextValue();
        
        float u = i + speed;
        
        while (u < 0)
            u += bufferLength;
        
        while ( u >= bufferLength)
            u-= bufferLength;
        
        int n0 = static_cast<int>(u);
        float f = u - n0;
        
        int nm1 = n0 - 1 < 0 ? bufferLength - 1 : n0 - 1;
        int n1 = n0+1 >= bufferLength ? 0 : n0+1;
        int n2 = n1 + 1;
        
        float val = hermite(f, buffer[nm1], buffer[n0], buffer[n1], buffer[n2]);
        i = u;
        
        if (std::abs(speed) < 0.01 && !cut){
            cut = true;
            residual.setCurrentAndTargetValue(val);
            residual.setTargetValue(0.0f);
        }
        
        return val;
    }
        
    void store(float value)
    {
        while (i >= bufferLength)
            i-=bufferLength;
        
        buffer[static_cast<int>(i)] = value * gain.getNextValue();
        i++;
    } 
    
    void reset(){
        i = 0;
        cut = false;
    }
    
    void updateParams(double bpm, float length, float clipGain)
    {
        bufferLength = static_cast<int>(length * (60.0 / bpm) * 4 * sampleRate);
        bufferLength = juce::jmin(bufferLength, static_cast<int>(MAX_BUFFER_SECONDS * sampleRate));
        gain.setTargetValue(clipGain);
    }
    
    std::vector<float>& getBuffer() { return buffer; }
    float getIndex () { return i; }
    int getBufferLength() { return bufferLength; }
    
private:
    std::vector<float> buffer;
    double sampleRate;
    int bufferLength;
    float i;
    
    bool cut;
    //residual is the value that get smoothed to 0 once the rewind is completed
    juce::LinearSmoothedValue<float> residual;
    juce::LinearSmoothedValue<float> gain;
    
    /*
     
     xm1 ---> x[n-1]
     x0  ---> x[n]
     x1  ---> x[n+1]
     x2  ---> x[n+2]
     
     */
    // laurent de soras
    inline float hermite(float frac_pos, float xm1, float x0, float x1, float x2)
    {
       const float    c     = (x1 - xm1) * 0.5f;
       const float    v     = x0 - x1;
       const float    w     = c + v;
       const float    a     = w + v + (x2 - x0) * 0.5f;
       const float    b_neg = w + a;

       return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
    }
};
