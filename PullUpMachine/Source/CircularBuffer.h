#pragma once
#include <JuceHeader.h>

class CircularBuffer
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        bufferLength = static_cast<int>(4 * sampleRate);
        residual.reset(sampleRate, 0.001);
        buffer.resize(bufferLength);
        i = 0;
        cut = false;
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
        
        buffer[static_cast<int>(i)] = value;
        i++;
    } 
    
    void reset(){
        i = 0;
        cut = false;
    }
    
    void updateParams(double bpm, float length)
    {
        bufferLength = static_cast<int>(length * (60.0 / bpm) * 4 * sampleRate);
        bufferLength = juce::jmin(bufferLength, static_cast<int>(4 * sampleRate));
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
    juce::LinearSmoothedValue<float> residual;
    
    float interp(float index)
    {
        int x0 = static_cast<int>(index);
        int x1 = x0+1;
        float f = index - static_cast<float>(x0);
        return buffer[x0] * (1.0f - f) + buffer[x1] * f;
    }
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