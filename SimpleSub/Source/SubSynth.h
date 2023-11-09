#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

namespace SubSynth {

template <typename Type>
class Osc
{
public:
    //==============================================================================
    Osc() 
    {
        auto& osc1 = processorChain.template get<osc1Index>();
        osc1.initialise ([] (Type x) { return std::sin(x);}, 256);
        
        auto& gain = processorChain.template get<gainIndex>();
        gain.setRampDurationSeconds(0.05);
    }
    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);

        processorChain.prepare(spec);
    }
    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        auto outputBlock = context.getOutputBlock();
        auto numSamples = outputBlock.getNumSamples();
        auto block = tempBlock.getSubBlock (0, (size_t) numSamples);
        block.clear();
        
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        processorChain.process (ctx);
        
        outputBlock
            .getSubBlock ((size_t) 0, (size_t) numSamples)
            .add (tempBlock);
    }
    
    //==============================================================================
    void reset() noexcept 
    {
        processorChain.reset();
    }
    
    //==============================================================================
    void setFrequency (Type newValue, bool force = false)
    {
        auto& osc = processorChain.template get<osc1Index>();
        osc.setFrequency (newValue, force);
    }
    
    //==============================================================================
    void setLevel (Type newValue, bool force=false)
    {
        processorChain.template get<gainIndex>().setGainLinear (newValue);
    }
    
private:
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;
    
    enum
    {
        osc1Index,
        gainIndex
    };
    
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<Type>> processorChain;
    //==============================================================================
};

class Chorus
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        reset();
        for (auto& ch : processors)
            ch.prepare({spec.sampleRate, spec.maximumBlockSize, 1});
    }
    
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        juce::dsp::ProcessContextReplacing<float> ctx(context);
        juce::dsp::AudioBlock<float> leftBlock = ctx.getOutputBlock().getSingleChannelBlock(0);
        if (ctx.getOutputBlock().getNumChannels() > 1)
        {
            juce::dsp::AudioBlock<float> rightBlock = ctx.getOutputBlock().getSingleChannelBlock(1);
            processors[1].process(juce::dsp::ProcessContextReplacing<float>(rightBlock));
        }
        
        processors[0].process(juce::dsp::ProcessContextReplacing<float>(leftBlock));
    }
    
    //==============================================================================
    void reset() noexcept
    {
        for (auto& ch : processors)
            ch.reset();
    }
    void setWidth(float w)
    {
        auto rateL =     juce::jmap  (w,     0.f,    1.f,    0.5f,   3.f);
        auto rateR =     juce::jmap  (w,     0.f,    1.f,    0.5f,   4.f);
        auto depth =    juce::jmap  (w,     0.f,    1.f,    0.f,    0.25f);
        auto delayL =   juce::jmap  (w,     0.f,    1.f,    5.f,    7.f);
        auto delayR =   juce::jmap  (w,     0.f,    1.f,    5.5f,   9.f);
        auto mix =      juce::jmap  (w,     0.f,    1.f,    0.f,    0.6f);
        for (auto& ch : processors)
        {
            ch.setDepth(depth);
            ch.setMix(mix);
        }
        processors[0].setCentreDelay(delayL);
        processors[1].setCentreDelay(delayL);
        processors[0].setRate(rateL);
        processors[1].setRate(rateR);

    }
private:
    std::array<juce::dsp::Chorus<float>, 2> processors;
};

class HarmonicOscillator
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
        mainChain.prepare(spec);
        
        harmonicChain.prepare(spec);
        velocityGain.setRampDurationSeconds(0);
        
        velocityGain.prepare(spec);
    }
    
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        auto outBlock = context.getOutputBlock();
        auto numSamples = outBlock.getNumSamples();
        
        mainChain.process(context);
        
        auto block = tempBlock.getSubBlock (0, (size_t) numSamples);
        block.clear();
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        harmonicChain.process (ctx);
        
        outBlock
            .getSubBlock ((size_t) 0, (size_t) numSamples)
            .add (tempBlock);
        
        velocityGain.process(context);
    }
    
    void reset() noexcept
    {
        mainChain.reset();
        harmonicChain.reset();
        velocityGain.reset();
    }
    
    void setFrequency (float newValue, bool force = false)
    {
        mainChain.get<main_osc_index>().setFrequency(newValue, force);
        harmonicChain.get<h2index>().setFrequency(newValue * 2, force);
        harmonicChain.get<h3index>().setFrequency(newValue * 3, force);
        harmonicChain.get<h4index>().setFrequency(newValue * 4, force);
        harmonicChain.get<h5index>().setFrequency(newValue * 5, force);
    }
    
    void setLevels (float h1, float h2, float h3, float h4, float h5)
    {
        mainChain.get<main_osc_index>().setLevel(h1);
        harmonicChain.get<h2index>().setLevel(h2);
        harmonicChain.get<h3index>().setLevel(h3);
        harmonicChain.get<h4index>().setLevel(h4);
        harmonicChain.get<h5index>().setLevel(h5);
    }
    
    void setWidth(float w)
    {
        auto& chorus = harmonicChain.get<chorusIndex>();
        chorus.setWidth(w);
    }
    
    void setVelocity(float newV) 
    {
        velocityGain.setGainLinear(newV);
    }
  
private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;
    
    enum
    {
        main_osc_index = 0,
    };
    
    enum
    {
        h2index,
        h3index,
        h4index,
        h5index,
        chorusIndex,
    };
    juce::dsp::ProcessorChain<Osc<float>> mainChain;
    
    juce::dsp::ProcessorChain<Osc<float>, Osc<float>, Osc<float>, Osc<float>, Chorus> harmonicChain;
    
    juce::dsp::Gain<float> velocityGain;
    //==============================================================================
};
//==============================================================================
class Envelope : public juce::dsp::ProcessorBase
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec) override
    {
        reset();
        sampleRate = spec.sampleRate;
        samplesPerBlock = spec.maximumBlockSize;
        outputChannels = spec.numChannels;
    }
    
    inline float getNextValue()
    {
        switch (state) {
            case Idle:
                break;
            case Attack:
                output = attackBase + output * attackCoef;
                if (output >= 1.0) {
                    output = 1.0;
                    state = Decay;
                }
                break;
            case Decay:
                output = decayBase + output*decayCoef;
                if (output <= sustainLevel) {
                    output = sustainLevel;
                    state = Sustain;
                }
                break;
            case Sustain:
                output = sustainLevel;
                break;
            case Release:
                output = releaseBase + output * releaseCoef;
                if (output <= 0.00001) {
                    output = 0.0;
                    state = Idle;
                }
        }
        
        return output;
    }
    
    void process (const juce::dsp::ProcessContextReplacing<float>& ctx) override
    {
        auto outputBlock = ctx.getOutputBlock();
        
        for (int i = 0; i < outputBlock.getNumSamples(); i++) {
            getNextValue();//updates output;
            for (size_t c = 0; c < outputChannels; c++)
                outputBlock.getChannelPointer(c)[i] *= output;
        }
    }
    
    void reset() override
    {
        state = Idle;
        output = 0.0;
    }
    
    inline void gate(int gate)
    {
        if (gate)
            state = Attack;
        else if (state != Idle)
            state = Release;
    }

    
    void update(float a, float d, float s, float r)
    {
        //a, d, and r in ms
        attackRate = sampleRate * a * 0.001;
        attackCoef = calcCoef(a, targetRatioA);
        attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
        
        sustainLevel = juce::Decibels::decibelsToGain(s);
        
        decayRate = sampleRate * d* 0.001;
        decayCoef = calcCoef(d, targetRatioDR);
        decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
                
        releaseRate = sampleRate * r * 0.001;
        releaseCoef = calcCoef(r, targetRatioDR);
        releaseBase = -targetRatioDR * (1.0 - releaseCoef);
    }
    
    void setTargetRatioA(double targetRatio)
    {
        if (targetRatio < 0.000000001)
            targetRatio = 0.000000001;  // -180 dB
        targetRatioA = targetRatio;
        attackCoef = calcCoef(attackRate, targetRatioA);
        attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
    }
    void setTargetRatioDR(double targetRatio)
    {
        if (targetRatio < 0.000000001)
            targetRatio = 0.000000001;  // -180 dB
        targetRatioDR = targetRatio;
        decayCoef = calcCoef(decayRate, targetRatioDR);
        releaseCoef = calcCoef(releaseRate, targetRatioDR);
        decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
        releaseBase = -targetRatioDR * (1.0 - releaseCoef);
    }
    
    enum EnvState
    {
        Idle = 0,
        Attack,
        Decay,
        Sustain,
        Release,
    };
    
    int getState() {return state;}
private:
    double sampleRate = 44100;
    juce::uint32 samplesPerBlock;
    int outputChannels;
    
    //state
    int state;
    double output;
    double attackRate;
    double decayRate;
    double releaseRate;
    double attackCoef;
    double decayCoef;
    double releaseCoef;
    double sustainLevel;
    double targetRatioA;
    double targetRatioDR;
    double attackBase;
    double decayBase;
    double releaseBase;
    
    double calcCoef(double rate, double targetRatio) {
        return (rate <= 0) ? 0.0 : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
    }
};

class Distortion
{
public:
    //==============================================================================
    Distortion()
    {
        drive.setRampDurationSeconds(0.05);
        waveShapers[param::CubicAlgo].functionToUse = param::getWaveShaper(param::CubicAlgo);
        waveShapers[param::TanhAlgo].functionToUse = param::getWaveShaper(param::TanhAlgo);
        waveShapers[param::SinAlgo].functionToUse = param::getWaveShaper(param::SinAlgo);
    }
    
    void update(float driveDB, int algorithmIndex, bool isActive)
    {
        if (currentIndex != algorithmIndex)
        {
            currentIndex = algorithmIndex;
            reset();
        }
        drive.setGainDecibels(driveDB);
        bypass = !isActive;
    }
    
    //==============================================================================
    void reset()
    {
        drive.reset();
        waveShapers[currentIndex].reset();
    }
    
    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context)
    {
        if(bypass) return;
        drive.process(context);
        waveShapers[currentIndex].process(context);
    }
    
    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        for (auto& ws : waveShapers)
            ws.prepare(spec);
        drive.prepare(spec);
    }
    
private:
    bool bypass = false;
    int currentIndex = 0;
    
    juce::dsp::Gain<float> drive;
    std::array<juce::dsp::WaveShaper<float, std::function<float(float)>>, 4> waveShapers;
    //==============================================================================
};

struct NoteStack
{
static constexpr auto maxNumNotes = 16;
public:
    NoteStack()
    {
        notes.fill(-1);
    }
    
    void push(int newNote)
    {
        top++;
        notes[top] = newNote;
    }
    
    int pop()
    {
        if (top == -1) return -1;
        
        notes[top] = -1;
        top--;
        if (top == -1) return -1;
        else return notes[top];
    }
    
    int peak()
    {
        if (top == -1) return -1;
        return notes[top];
    }
    
    void remove(int newNote)
    {
        for (int i = 0; i < maxNumNotes; i++)
        {
            if(notes[i] == newNote)
            {
                notes[i] = -1;
                compact(i);
            }
        }
    }
    
    void compact(int index)
    {
        int i = index;
        while (notes[i+1] != -1 && i < maxNumNotes - 2)
        {
            notes[i] = notes[i+1];
            i++;
        }
        // Fill the remaining part of the array with -1
        while (i < maxNumNotes) {
            notes[i] = -1;
            i++;
        }
        top--;
    }
    
    int getNumHeldDown() {return top+1;}

private:
    int top = -1;
    std::array<int, maxNumNotes> notes;
};

//==============================================================================
class Voice  : public juce::MPESynthesiserVoice
{
    static constexpr int MaxNotes = 32;
public:
    Voice()
    {
        processorChain.get<masterGainIndex>().setRampDurationSeconds(0.01);
    }
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
        noteSmoother.reset(currentSampleRate);
        processorChain.prepare (spec);
    }
    
    //==============================================================================
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        auto block = tempBlock.getSubBlock (0, (size_t) numSamples);
        block.clear();
        juce::dsp::ProcessContextReplacing<float> context (block);
        processorChain.process (context);
        
        juce::dsp::AudioBlock<float> (outputBuffer)
            .getSubBlock ((size_t) startSample, (size_t) numSamples)
            .add (tempBlock);
        
        if (processorChain.get<envIndex>().getState() == Envelope::Idle)
        {
            clearCurrentNote();
        }
    }
    
    void noteStarted() override
    {
        const auto noteToStart = getCurrentlyPlayingNote();
        
        auto velocity = 1.0f;//noteToStart.noteOnVelocity.asUnsignedFloat();
        
        auto& osc = processorChain.get<oscIndex>();
        auto& env = processorChain.get<envIndex>();
        
        noteSmoother.reset(glideParam->get() * 100);
        
        //noteSmoother.setTargetValue(freqHz);
        int noteValue = noteToStart.initialNote;
        noteStack.push(noteValue);
        
        if(glideEnabled)
        {
            if (noteStack.getNumHeldDown() == 1)
            {
                if(env.getState() == Envelope::Idle)
                    osc.reset();
                env.gate(true);
                osc.setVelocity(velocity);
                noteSmoother.setCurrentAndTargetValue(noteValue);
                osc.setFrequency(getFrequencyFromMidi(noteSmoother.getCurrentValue()), true);
            }
            else{
                if(env.getState() == Envelope::Release)
                    env.gate(true);
                noteSmoother.setTargetValue(noteValue);
            }
        }
        else{
            if(env.getState() == Envelope::Idle)
                osc.reset();
            if (noteStack.getNumHeldDown() == 1)
                osc.setVelocity(velocity);
            env.gate(true);
            noteSmoother.setCurrentAndTargetValue(noteValue);
        }
    }
    
    void noteStopped (bool allowTailOff) override
    {
        if(!allowTailOff) { clearCurrentNote(); return; }
        
        auto& env = processorChain.get<envIndex>();
        
        noteSmoother.reset(glideParam->get() * 100);
        
        int noteValue = getCurrentlyPlayingNote().initialNote;
        
        if(noteValue == noteStack.peak())
        {
            int newNote = noteStack.pop();
            if(newNote == -1)
            {
                env.gate(false);
            }
            else{
                if(glideEnabled)
                    noteSmoother.setTargetValue(newNote);
                else
                    noteSmoother.setCurrentAndTargetValue(newNote);
            }
        }
        else{
            noteStack.remove(noteValue);
        }
    }
    
    //==============================================================================
    void notePitchbendChanged() override
    {
        auto freqHz = (float) getCurrentlyPlayingNote().getFrequencyInHertz();
        processorChain.get<oscIndex>().setFrequency (freqHz);
    }
    void notePressureChanged() override {}
    void noteTimbreChanged()   override {}
    void noteKeyStateChanged() override {}
    
    void setParamPointers(juce::AudioProcessorValueTreeState& apvts)
    {
        auto floatHelper = [&state = apvts] (auto& param, const param::PID pid)
        {
            param = dynamic_cast<juce::AudioParameterFloat*>(state.getParameter(param::toID(pid)));
            jassert(param!=nullptr);
        };
        
        floatHelper(attackParam, param::param_attack);
        floatHelper(decayParam, param::param_decay);
        floatHelper(sustainParam, param::param_sustain);
        floatHelper(releaseParam, param::param_release);
        
        floatHelper(AttackCurveParam, param::param_A_curve);
        floatHelper(DecayReleaseCurveParam, param::param_DR_curve);
        
        floatHelper(harmonic1, param::param_harmonic_1);
        floatHelper(harmonic2, param::param_harmonic_2);
        floatHelper(harmonic3, param::param_harmonic_3);
        floatHelper(harmonic4, param::param_harmonic_4);
        floatHelper(harmonic5, param::param_harmonic_5);
        
        floatHelper(widthParam, param::param_width);
        
        floatHelper(driveParam, param::param_drive);
        floatHelper(outGainParam, param::param_out_gain);
        
        floatHelper(glideParam, param::param_glide);
        glideEnabledParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(param::toID(param::param_glide_active)));
        
        distortionEnabledParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(param::toID(param::param_distortionIsActive)));
        distortionTypeParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(param::toID(param::param_distorionType)));
    }
    
    void updateParams(int blockSize)
    {
        auto& env = processorChain.get<envIndex>();
        env.update(attackParam->get(), decayParam->get(), sustainParam->get(), releaseParam->get());
        env.setTargetRatioA(AttackCurveParam->get());
        env.setTargetRatioDR(DecayReleaseCurveParam->get());
        
        processorChain.get<distortionIndex>().update(driveParam->get(), distortionTypeParam->getIndex(), distortionEnabledParam->get());
        
        glideEnabled = glideEnabledParam->get();
        
        auto& osc = processorChain.get<oscIndex>();
        osc.setLevels(harmonic1->get(), harmonic2->get(), harmonic3->get(), harmonic4->get(), harmonic5->get());
        osc.setWidth(widthParam->get());
        auto freqHz = getFrequencyFromMidi(noteSmoother.getCurrentValue());
        osc.setFrequency(freqHz, !glideEnabled);
        
        processorChain.get<masterGainIndex>().setGainDecibels(outGainParam->get());
        
        noteSmoother.skip(blockSize);
    }
    
    double getFrequencyFromMidi (float midiNote) const noexcept
    {
        auto pitchInSemitones = double (midiNote) + getCurrentlyPlayingNote().totalPitchbendInSemitones;
        return 440 * std::pow (2.0, (pitchInSemitones - 69.0) / 12.0);
    }

private:
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;
    
    NoteStack noteStack;
    
    juce::SmoothedValue<float> noteSmoother;
    bool glideEnabled = false;
    
    enum
    {
        oscIndex,
        envIndex,
        distortionIndex,
        masterGainIndex
    };
    
    juce::dsp::ProcessorChain<  HarmonicOscillator,
                                Envelope,
                                Distortion,
                                juce::dsp::Gain<float>> processorChain;
    
    juce::AudioParameterFloat* attackParam;
    juce::AudioParameterFloat* decayParam;
    juce::AudioParameterFloat* sustainParam;
    juce::AudioParameterFloat* releaseParam;
    
    juce::AudioParameterFloat* AttackCurveParam;
    juce::AudioParameterFloat* DecayReleaseCurveParam;
    
    juce::AudioParameterFloat* driveParam;
    juce::AudioParameterBool* distortionEnabledParam;
    juce::AudioParameterChoice* distortionTypeParam;
    
    juce::AudioParameterFloat* outGainParam;
    
    juce::AudioParameterFloat* widthParam;
    
    juce::AudioParameterFloat* harmonic1;
    juce::AudioParameterFloat* harmonic2;
    juce::AudioParameterFloat* harmonic3;
    juce::AudioParameterFloat* harmonic4;
    juce::AudioParameterFloat* harmonic5;
    
    juce::AudioParameterFloat* glideParam;
    juce::AudioParameterBool* glideEnabledParam;
};

//==============================================================================
class Synth  : public juce::MPESynthesiser
{
public:
    
    //==============================================================================
    Synth()
    {
        addVoice(new Voice);
        setVoiceStealingEnabled (true);
    }
    
    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate (spec.sampleRate);
        
        for (auto* v : voices)
            dynamic_cast<Voice*> (v)->prepare (spec);
    }
    
    void setParamPointers(juce::AudioProcessorValueTreeState& apvts)
    {
        for (auto* voice : voices)
        {
            dynamic_cast<Voice*>(voice)->setParamPointers(apvts);
        }
    }
    
    void updateParams(int blockSize)
    {
        for (auto* voice : voices)
        {
            dynamic_cast<Voice*>(voice)->updateParams(blockSize);
        }
    }
    
    void noteAdded (juce::MPENote newNote) override
    {
        const juce::ScopedLock sl (voicesLock);

        auto* voice = voices[0];
        startVoice(voice, newNote);
    }
    
    void noteReleased (juce::MPENote finishedNote) override
    {
        const juce::ScopedLock sl (voicesLock);

        auto* voice = voices[0];
        stopVoice(voice, finishedNote, true);
    }
    
private:

    
    //=============== ===============================================================
    void renderNextSubBlock (juce::AudioBuffer<float>& outputAudio, int startSample, int numSamples) override
    {
        updateParams(numSamples);
        MPESynthesiser::renderNextSubBlock (outputAudio, startSample, numSamples);
    }
};

}
