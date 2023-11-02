/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Clipper.h"
#include "AudioBufferQueue.h"
#include "ScopeDataCollector.h"
#include "StereoLevelMeter.h"
#include "Params.h"

//==============================================================================
/**
*/
class SimpleClipAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
                                , public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    SimpleClipAudioProcessor();
    ~SimpleClipAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;


    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    AudioBufferQueue<float>& getAudioBufferQueue () noexcept { return audioBufferQueue;}
    
    //setters
    
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    
    MultiLevelMeter::RMS_Peak_Info getInRMSPeakLevels();
    MultiLevelMeter::RMS_Peak_Info getOutRMSPeakLevels();
    
    juce::AudioProcessorValueTreeState apvts;
    std::vector<param::RAP*> params;

private:
    std::atomic<float>  inRMSLevel, outRMSLevel,
                        inPeakLevel, outPeakLevel;
    
    Clipper clipper;
    
    AudioBufferQueue<float> audioBufferQueue;
    ScopeDataCollector<float> scopeDataCollector { audioBufferQueue };
    
    juce::AudioParameterFloat* inputGain, *outputGain;
    
    juce::dsp::Oversampling<float> oversampling;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleClipAudioProcessor)
};
