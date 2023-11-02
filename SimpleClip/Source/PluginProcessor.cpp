#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleClipAudioProcessor::SimpleClipAudioProcessor()
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
    params(),
    clipper(8192, 1.f, 0.05f),
    oversampling(2, 2, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple)
#endif
{
    for (int i = 0; i < param::NumParams; i++)
    {
        auto pID = static_cast<param::PID>(i);
        params.push_back(apvts.getParameter(param::toID(pID)));
        apvts.addParameterListener(param::toID(pID), this);
    }
    
    inputGain = dynamic_cast<juce::AudioParameterFloat*>(params[static_cast<int>(param::PID::InputGain)]);
    
    outputGain = dynamic_cast<juce::AudioParameterFloat*>(params[static_cast<int>(param::PID::OutputGain)]);
}

SimpleClipAudioProcessor::~SimpleClipAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleClipAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleClipAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleClipAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleClipAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleClipAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleClipAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleClipAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleClipAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleClipAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleClipAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleClipAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.numChannels = getTotalNumInputChannels();
    oversampling.reset();
    oversampling.initProcessing(samplesPerBlock);

    const auto oS = static_cast<bool>(params[static_cast<int>(param::PID::Oversampling)]->getValue());
    if (oS)
        setLatencySamples(oversampling.getLatencyInSamples());
}

void SimpleClipAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleClipAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
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

void SimpleClipAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    const auto gainin = inputGain->get();
    
    buffer.applyGain(juce::Decibels::decibelsToGain(gainin));
    
    float inRMS = 0.f;
    for (auto c = 0; c < totalNumInputChannels; c++)
    {
        float rms = buffer.getRMSLevel(c, 0, buffer.getNumSamples());
        if (rms > inRMS)
            inRMS = rms;
    }
    
    inRMSLevel.store(inRMS);
    
    inPeakLevel.store(buffer.getMagnitude(0, buffer.getNumSamples()));
    
    //data collector for an oscilloscope
    scopeDataCollector.process(buffer.getReadPointer(0), (size_t) buffer.getNumSamples());
    
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::AudioBlock<float> oversampled;
    const auto oS = static_cast<bool>(params[static_cast<int>(param::PID::Oversampling)]->getValue());

    if (oS)
    {
        oversampled = oversampling.processSamplesUp(block);
        clipper.process(oversampled);
        oversampling.processSamplesDown(block);
    }
    else{
        clipper.process(block);
    }
    
    const auto gainout = outputGain->get();
    
    buffer.applyGain(juce::Decibels::decibelsToGain(gainout));
    
    float outRMS = 0.f;
    for (auto c = 0; c < totalNumOutputChannels; c++)
    {
        float rms = buffer.getRMSLevel(c, 0, buffer.getNumSamples());
        if (rms > outRMS)
            outRMS = rms;
    }
    
    outRMSLevel.store(outRMS);
    
    outPeakLevel.store(buffer.getMagnitude(0, buffer.getNumSamples()));
}

//==============================================================================
bool SimpleClipAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleClipAudioProcessor::createEditor()
{
    return new SimpleClipAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleClipAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SimpleClipAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new SimpleClipAudioProcessor();
}

void SimpleClipAudioProcessor::parameterChanged (const juce::String &id, float newValue)
{
    if (id==param::toID(param::PID::Threshold))
    {
        auto val = param::cook::threshold(newValue);
        clipper.thresholdChanged(val);
    }
    else if (id== param::toID(param::PID::Knee)){
        auto val = param::cook::knee(newValue);
        clipper.kneeChanged(val);
    }
    else if(id == param::toID(param::PID::Oversampling))
    {
        if (static_cast<bool>(newValue))
            setLatencySamples(oversampling.getLatencyInSamples());
        else
            setLatencySamples(0);
    }
}

MultiLevelMeter::RMS_Peak_Info SimpleClipAudioProcessor::getInRMSPeakLevels()
{
    float inRMSLevelValue = inRMSLevel.load();
    float inPeakLevelValue = inPeakLevel.load();
    
    MultiLevelMeter::RMS_Peak_Info info {
        juce::Decibels::gainToDecibels(inRMSLevelValue),
        inRMSLevelValue,
        juce::Decibels::gainToDecibels(inPeakLevelValue),
        inPeakLevelValue,
    };
    return info;
}

MultiLevelMeter::RMS_Peak_Info SimpleClipAudioProcessor::getOutRMSPeakLevels()
{
    float outRMSLevelValue = outRMSLevel.load();
    float outPeakLevelValue = outPeakLevel.load();
    
    MultiLevelMeter::RMS_Peak_Info info {
        juce::Decibels::gainToDecibels(outRMSLevelValue),
        outRMSLevelValue,
        juce::Decibels::gainToDecibels(outPeakLevelValue),
        outPeakLevelValue,
    };
    return info;
}

