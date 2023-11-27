/*
  ==============================================================================

    Siren.h
    Created: 9 Nov 2023 12:58:42pm
    Author:  Dario Potenza

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

class LFO
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate / spec.maximumBlockSize;
    }
    
    void reset()
    {
        phase.reset();
    }
    
    float getLFOValue()
    {
        float inc = juce::MathConstants<float>::twoPi * freq / sampleRate;
        float phi = phase.advance(inc);
        float mod = (1.0f - lfoBlend) * lfo1Funcs[type1](phi) + (lfoBlend) * lfo2Funcs[type2](phi);
        return magnitude * mod;
    }
    
    void update(float fre, float mag, int index1, int index2, float blend)
    {
        freq = fre;
        magnitude = mag;
        lfoBlend = blend;
        type1 = index1;
        type2 = index2;
    }
    
private:
    double sampleRate = 44100.0 / 250;
    float freq = 1.0f;
    float magnitude = 1.0f;
    int type1 = 0, type2 = 0;
    float lfoBlend = 0.0f;
    juce::dsp::Phase<float> phase;
    
    //lfo 1 = square, triangle, sine
    std::array<std::function<float(float)>, 3> lfo1Funcs = {
        [] (float x) {
            //SQUARE
            return (x - juce::MathConstants<float>::pi) < 0 ? 1 : -1;
        },
        [] (float x) {
            //TRIANGLE
            float x_t = (x / juce::MathConstants<float>::twoPi) + 0.25f;
            float inner = 2 * (x_t - floor(x_t + 0.5f));
            return 2 * fabsf(inner) - 1;
        },
        [] (float x) {
            return sinf(x);
        },
    };
    
    //lfo 2 = saw up, saw down, square
    std::array<std::function<float(float)>, 3> lfo2Funcs = {
        [] (float x) {
            float x_t = (x / juce::MathConstants<float>::twoPi);
            return 2 * (x_t - floor(0.5f + x_t));
        },
        [] (float x) {
            float x_t = (x / juce::MathConstants<float>::twoPi);
            return - 2 * (x_t - floor(0.5f + x_t));
        },
        [] (float x) {
            return (x - juce::MathConstants<float>::pi) < 0 ? 1 : -1;
        },
    };
    
};

constexpr int numWaveTableSlots = 16;
typedef struct {
    double topFreq;
    int waveTableLen;
    float *waveTable;
} waveTable;
class BandLimitedOscillator
{
protected:
    double phasor;
    juce::LinearSmoothedValue<double> phaseInc;
    
    double sampleRate;
    
    int numWaveTables;
    waveTable waveTables[numWaveTableSlots];
public:
    BandLimitedOscillator() {
        phasor = 0;
        phaseInc.setCurrentAndTargetValue(0.0f);
        numWaveTables = 0;
        for (int idx = 0; idx <= numWaveTableSlots; idx++) {
            waveTables[idx].topFreq = 0;
            waveTables[idx].waveTableLen = 0;
            waveTables[idx].waveTable = 0;
        }
        
    }
    
    ~BandLimitedOscillator() {
        for (int idx = 0; idx < numWaveTableSlots; idx++) {
            float *temp = waveTables[idx].waveTable;
            if (temp != 0)
                delete [] temp;
        }
    }
    
    void prepare(double sr) {
        sampleRate = sr; 
        phaseInc.reset(sr, 0.01);
        setSawTooth();
    }
    
    void reset() {
        phaseInc.reset(sampleRate, 0.01);
    }
    
    void process(const juce::dsp::ProcessContextReplacing<float>& ctx)
    {
        auto block = ctx.getOutputBlock();
        auto numSamples = block.getNumSamples();
        
        for (int i = 0; i < numSamples; i++)
        {
            float output = getOutput();
            updatePhase();
        
            for (int c = 0; c < block.getNumChannels(); c++)
            {
                auto* channelData = block.getChannelPointer(c);
                channelData[i] = output;
            }
        }
    }
    
    void update(float freq, float modAmount)
    {
        float f = freq + freq * modAmount;
        setFrequency(f / sampleRate);
    }

private:
    inline void setFrequency(double inc){ phaseInc.setTargetValue(inc); }
    
    inline void updatePhase(void)
    {
        phasor += phaseInc.getNextValue();
        
        if (phasor >= 1.0)
            phasor -= 1.0;
    }
    
    float getOutput(void)
    {
        int waveTableIdx = 0;
        double inc = phaseInc.getCurrentValue();
        while ((inc >= waveTables[waveTableIdx].topFreq) && (waveTableIdx < (numWaveTables - 1))) {
            ++waveTableIdx;
        }
        
        waveTable* waveTable = &waveTables[waveTableIdx];
        
        double temp = phasor * waveTable->waveTableLen;
        int intPart = temp;
        double fracPart = temp - intPart;
        float samp0 = waveTable->waveTable[intPart];
        if (++intPart >= waveTable->waveTableLen)
            intPart = 0;
            
        float samp1 = waveTable->waveTable[intPart];
        return samp0 + (samp1 - samp0) * fracPart;
    }
    
    int addWaveTable(int len, float* waveTableIn, double topFreq)
    {
        if (numWaveTables < numWaveTableSlots) {
            float* waveTable = waveTables[numWaveTables].waveTable = new float[len];
            waveTables[numWaveTables].waveTableLen = len;
            waveTables[numWaveTables].topFreq = topFreq;
            ++numWaveTables;
            
            for (long idx = 0; idx < len; idx++) {
                waveTable[idx] = waveTableIn[idx];
            }
            
            return 0;
        }
        return numWaveTables;
    }
    
    void setSawTooth()
    {
        double baseFrequency = 20.0;
        int maxHarms = sampleRate / (3.0 * baseFrequency) + 0.5;
        
        unsigned int v = maxHarms;
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        int tableLen = v * 2;
        
        double ar[tableLen], ai[tableLen];
        
        double topFreq = baseFrequency * 2.0 / sampleRate;
        double scale = 0.0;
        for (; maxHarms >= 1; maxHarms >>= 1) {
            defineSawtooth(tableLen, maxHarms, ar, ai);
            scale = makeWaveTable(tableLen, ar, ai,scale, topFreq);
            topFreq *= 2;
            if (tableLen > 999999)
                tableLen >>= 1;
        }
    }
    
    void defineSawtooth(int len, int numHarmonics, double* ar, double* ai)
    {
        if (numHarmonics > (len >> 1))
            numHarmonics = (len >> 1);
        
        for (int idx = 0; idx < len; idx++) {
            ar[idx] = 0;
            ai[idx] = 0;
        }
        
//        for (int idx = 1, jdx = len-1; idx <= numHarmonics; idx++, jdx--) {
//            double temp = -1.0 / idx;
//            ar[idx] = -temp;
//            ar[jdx] = temp;
//        }
        
        for (int idx = 1, jdx = len - 1; idx <= numHarmonics; idx++, jdx--) {
            double temp = idx & 0x01 ? 1.0 / idx : 0.0;
            ar[idx] = -temp;
            ar[jdx] = temp;
        }
    }
    
    float makeWaveTable(int len, double* ar, double* ai, double scale, double topFreq) {
        fft(len, ar, ai);
        
        if (scale == 0.0)
        {
            double max = 0;
            for (int idx = 0; idx < len; idx++) {
                double temp = fabs(ai[idx]);
                if (max < temp)
                    max = temp;
            }
            scale = 1.0 / max * 0.999;
        }
        
        float wave[len];
        for (int idx=0; idx < len; idx++) {
            wave[idx] = ai[idx] * scale;
        }
        
        if (addWaveTable(len, wave, topFreq))
            scale = 0.0;
        
        return scale;
    }
    
    void fft(int N, double *ar, double *ai)
    /*
     in-place complex fft
     
     After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
     
     program adapted from FORTRAN
     by K. Steiglitz  (ken@princeton.edu)
     Computer Science Dept.
     Princeton University 08544          */
    {
        int i, j, k, L;            /* indexes */
        int M, TEMP, LE, LE1, ip;  /* M = log N */
        int NV2, NM1;
        double t;               /* temp */
        double Ur, Ui, Wr, Wi, Tr, Ti;
        double Ur_old;
        
        // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
        
        NV2 = N >> 1;
        NM1 = N - 1;
        TEMP = N; /* get M = log N */
        M = 0;
        while (TEMP >>= 1) ++M;
        
        /* shuffle */
        j = 1;
        for (i = 1; i <= NM1; i++) {
            if(i<j) {             /* swap a[i] and a[j] */
                t = ar[j-1];
                ar[j-1] = ar[i-1];
                ar[i-1] = t;
                t = ai[j-1];
                ai[j-1] = ai[i-1];
                ai[i-1] = t;
            }
            
            k = NV2;             /* bit-reversed counter */
            while(k < j) {
                j -= k;
                k /= 2;
            }
            
            j += k;
        }
        
        LE = 1.;
        for (L = 1; L <= M; L++) {            // stage L
            LE1 = LE;                         // (LE1 = LE/2)
            LE *= 2;                          // (LE = 2^L)
            Ur = 1.0;
            Ui = 0.;
            Wr = cos(M_PI/(float)LE1);
            Wi = -sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
            for (j = 1; j <= LE1; j++) {
                for (i = j; i <= N; i += LE) { // butterfly
                    ip = i+LE1;
                    Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                    Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                    ar[ip-1] = ar[i-1] - Tr;
                    ai[ip-1] = ai[i-1] - Ti;
                    ar[i-1]  = ar[i-1] + Tr;
                    ai[i-1]  = ai[i-1] + Ti;
                }
                Ur_old = Ur;
                Ur = Ur_old * Wr - Ui * Wi;
                Ui = Ur_old * Wi + Ui * Wr;
            }
        }
    }
};

class Siren
{
public:
    Siren(juce::AudioProcessorValueTreeState& apvts)
    {
        auto floatHelper = [&state = apvts] (auto& param, const param::PID pid)
        {
            param = dynamic_cast<juce::AudioParameterFloat*>(state.getParameter(param::toID(pid)));
            jassert(param!=nullptr);
        };
        
        floatHelper(baseFreqParam, param::p_oscFreq);
        floatHelper(modAmountParam, param::p_LFODepth);
        floatHelper(modRateParam, param::p_LFORate);
        
        floatHelper(lfoBlendParam, param::p_LFOBlend);
        
        floatHelper(outGainParam, param::p_outGain);
        
        activateParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(param::toID(param::p_active)));
        lfoBypassParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(param::toID(param::p_LFOBypass)));
        
        lfo1TypeParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(param::toID(param::p_LFO1Type)));
        lfo2TypeParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(param::toID(param::p_LFO2Type)));
    }
    
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        lfo.prepare(spec);
        //osc.prepare(spec.sampleRate);
        osc.prepare(spec.sampleRate);
        outGain.prepare(spec);
    }
    
    void process(const juce::dsp::ProcessContextReplacing<float>& ctx)
    {
        update();
        
        osc.process(ctx);
        outGain.process(ctx);
    }
    
    void reset()
    {
        lfo.reset();
        osc.reset();
        outGain.reset();
    }
    
    void update()
    {
        lfo.update(modRateParam->get(), modAmountParam->get(), lfo1TypeParam->getIndex(), lfo2TypeParam->getIndex(), lfoBlendParam->get());
        
        float modAmount = 0;
        
        //if not bypassed get LFO value
        if(!lfoBypassParam->get())
        {
            modAmount = lfo.getLFOValue();
        }
        
        lfoVal.store(modAmount);
        osc.update(baseFreqParam->get(), modAmount);
        
        if(activateParam->get())
        {
            outGain.setGainDecibels(outGainParam->get());
        }
        else
        {
            outGain.setGainLinear(0.f);
        }
    }
    
    float getLFOValue()
    {
        return lfoVal.load();
    }
private:
    LFO lfo;
    //Osc osc;
    BandLimitedOscillator osc;
    std::atomic<float> lfoVal;
    juce::dsp::Gain<float> outGain;
    
    juce::AudioParameterFloat*  modAmountParam;
    juce::AudioParameterFloat*  modRateParam;
    juce::AudioParameterFloat*  baseFreqParam;
    
    juce::AudioParameterFloat*  outGainParam;
    
    juce::AudioParameterFloat*  lfoBlendParam;
    
    juce::AudioParameterBool* activateParam;
    juce::AudioParameterBool* lfoBypassParam;
    
    juce::AudioParameterChoice* lfo1TypeParam;
    juce::AudioParameterChoice* lfo2TypeParam;
};
