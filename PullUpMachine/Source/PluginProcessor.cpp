#include "PluginProcessor.h"
#include "PluginEditor.h"
/*
 TODO:
 Parameters:
    trigger,
    start speed,
    spinback duration,
    buffer length,

 Reset buffer index when press play.
 */
PullUpMachineAudioProcessor::PullUpMachineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts(*this, nullptr, "Parameters", param::createParamterLayout()),
    rewinder()
#endif
{
    auto floatHelper = [&apvts = this->apvts] (auto& param, const auto PID)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(param::toID(PID)));
        jassert(param!=nullptr);
    };
    
    floatHelper(startSpeedParam, param::PID::StartSpeed);
    floatHelper(durationParam, param::PID::Duration);
    floatHelper(bufferLengthParam, param::PID::BufferLength);
    floatHelper(powerParam, param::PID::Power);
    floatHelper(gainParam, param::PID::Gain);
    
    triggerParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(param::toID(param::PID::Trigger)));
    
    directionParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(param::toID(param::PID::Direction)));
    
    apvts.addParameterListener(param::toID(param::PID::Trigger), this);
    apvts.addParameterListener(param::toID(param::PID::Direction), this);
}

PullUpMachineAudioProcessor::~PullUpMachineAudioProcessor()
{
}

//==============================================================================
const juce::String PullUpMachineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PullUpMachineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PullUpMachineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PullUpMachineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PullUpMachineAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PullUpMachineAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PullUpMachineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PullUpMachineAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PullUpMachineAudioProcessor::getProgramName (int index)
{
    return {};
}

void PullUpMachineAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PullUpMachineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rewinder.prepare(sampleRate);
}

void PullUpMachineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PullUpMachineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PullUpMachineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if (auto bpmFromHost = *getPlayHead()->getPosition()->getBpm())
        bpm.store(bpmFromHost);
    
    float gain = juce::Decibels::decibelsToGain(gainParam->get());
    rewinder.updateParams(bpm, startSpeedParam->get(), durationParam->get(), powerParam->get(), bufferLengthParam->get(), gain);
    
    rewinder.process(buffer, triggerParam->get());
}

//==============================================================================
bool PullUpMachineAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PullUpMachineAudioProcessor::createEditor()
{
    return new PullUpMachineAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void PullUpMachineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PullUpMachineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PullUpMachineAudioProcessor();
}

void PullUpMachineAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == param::toID(param::PID::Trigger))
    {
        bool val = static_cast<bool> (newValue);
        if (val == false)
        {
            rewinder.reset();
        }
    }else if (parameterID == param::toID(param::PID::Direction))
    {
        bool val = static_cast<bool> (newValue);
        //backwards = true
        SpeedEnvelope::Direction dir = val ? SpeedEnvelope::Direction::backwards : SpeedEnvelope::Direction::forwards;
        rewinder.changeDirection(dir);
    }
}
