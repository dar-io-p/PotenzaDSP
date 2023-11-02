#pragma once

namespace ptnz
{
struct Point {
    float x, y;
};
}


class WaveShaper
{
public:
    WaveShaper(float t, float k, int numPoints) : numPoints(numPoints)
    {
        threshold = t;
        knee = k;
        waveShaper.resize(numPoints);
        createWaveShaper(threshold, knee);
    }
    
    WaveShaper(int numPoints) : numPoints(numPoints)
    {
        waveShaper.resize(numPoints);
    }
    
    std::vector<float>& getWaveShaper() {return waveShaper;}
    
    float getSample(float sample)
    {
        float index = std::abs(sample) * 0.5f * static_cast<float>(numPoints);
        index = juce::jmin(index, static_cast<float>(numPoints-1));
        int x0 = static_cast<int>(index);
        int x1 = x0+1;
        float f = index - static_cast<float>(x0);
        
        jassert(x0 < waveShaper.size() && x1 < waveShaper.size());
        
        return waveShaper[x0] * (1.0f - f) + waveShaper[x1] * f;
    }
    
    void createWaveShaper(float t, float k)
    {
        knee = k;
        threshold = t;
        
        float p = threshold -  knee * (std::sqrt(2.0f) / 2.0f);
        p = juce::jmax(p, 0.0f);
        float p_index = inputToIndex(p);
        
        //LINEAR REGION
        int i = 0;
        while (i <= p_index)
        {
            float x = 2.0f * static_cast<float>(i) / static_cast<float>(numPoints);
            jassert(i < waveShaper.size());
            waveShaper[i] = x;
            i++;
        }
        
        
        float p_quantised = waveShaper[p_index];
        const ptnz::Point p0 = {p_quantised,p_quantised};
        const ptnz::Point p1 = {threshold, threshold};
        const ptnz::Point p2 = {threshold+knee,threshold};
//
        populateQuadraticBezierCurve(p0, p1, p2);
//
        i = inputToIndex(threshold + knee);
        while (i < numPoints)
        {
            jassert(i < waveShaper.size());
            waveShaper[i] = threshold;
            i++;
        }
    }
private:
    ptnz::Point quadraticBezier(const ptnz::Point& p0, const ptnz::Point& p1, const ptnz::Point& p2, float t)
    {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;

        ptnz::Point p;
        p.x = uu * p0.x + 2 * u * t * p1.x + tt * p2.x;
        p.y = uu * p0.y + 2 * u * t * p1.y + tt * p2.y;

        return p;
    }
    
    void populateQuadraticBezierCurve(const ptnz::Point& p0, const ptnz::Point& p1, const ptnz::Point& p2)
    {
        const int bezierSamples = numPoints * 2 * knee;
        
        int prev = -1;
        
        for (int i = 0; i < bezierSamples; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(bezierSamples);
            ptnz::Point p = quadraticBezier(p0, p1, p2, t);
            
            int x = inputToIndex(p.x);
            
            if (x >= 0 && x < numPoints){
                if (x != prev)
                {
                    jassert(x < waveShaper.size());
                    waveShaper[x] = p.y;
                    prev = x;
                }
            }
        }
    }
    
    int inputToIndex(float x){
        return std::floor(x * (static_cast<float>(numPoints) / 2.0f));
    }
    
    int numPoints;
    float knee, threshold;
    std::vector<float> waveShaper;
};
