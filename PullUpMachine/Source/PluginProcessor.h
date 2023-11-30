#pragma once

#include <JuceHeader.h>
#include "Rewinder.h"
#include "Params.h"
//==============================================================================

class PullUpMachineAudioProcessor  :
    public juce::AudioProcessor,
    juce::AudioProcessorParameter::Listener
    #if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
    #endif
{
public:
    //==============================================================================
    PullUpMachineAudioProcessor();
    ~PullUpMachineAudioProcessor() override;

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
    
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;
    
    double getBPM () { return bpm.load(); }
    
    float getStartSpeed() { return startSpeedParam->get(); }
    float getDuration() { return durationParam->get(); }
    float getPower() { return powerParam->get(); }
    bool getDirection() { return directionParam->get(); }
    
    CircularBuffer& getCircularBuffer () { return rewinder.getBuffer(); }
    
    juce::AudioProcessorValueTreeState apvts;
private:
    Rewinder rewinder;
    
    juce::AudioParameterBool* triggerParam;
    juce::AudioParameterBool* directionParam;
    
    juce::AudioParameterFloat* startSpeedParam;
    juce::AudioParameterFloat* durationParam;
    juce::AudioParameterFloat* bufferLengthParam;
    juce::AudioParameterFloat* powerParam;
    
    std::atomic<double> bpm { 120.0 };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PullUpMachineAudioProcessor)
};
