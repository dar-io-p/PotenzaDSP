#pragma once

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace param {
    using APVTS = juce::AudioProcessorValueTreeState;
    using Layout = APVTS::ParameterLayout;
    using RAP = juce::RangedAudioParameter;
    using UniqueRAP = std::unique_ptr<RAP>;
    using UniqueRAPVector = std::vector<UniqueRAP>;
    using APP = juce::AudioProcessorParameter;
    using APPCategory = APP::Category;

    using APF = juce::AudioParameterFloat;
    using APB = juce::AudioParameterBool;

    using RangeF = juce::NormalisableRange<float>;
    using String = juce::String;

    enum PID {
        StartSpeed,
        Duration,
        BufferLength,
        Power,
        Trigger,
        Direction,
        Gain,
        NumParams,
    };
    
    static constexpr int NumberParams = static_cast<int>(PID::NumParams);

    inline String toName(PID pID)
    {
        switch (pID)
        {
            case PID::StartSpeed:
                return "Start Speed";
            case PID::Duration:
                return "Duration";
            case PID::BufferLength:
                return "Buffer Length";
            case PID::Power:
                return "Power";
            case PID::Trigger:
                return "Trigger";
            case PID::Direction:
                return "Direction";
            case PID::Gain:
                return "WheelUpGain";
            default:
                return "Unknown";
        }
    }

    inline String toID(const String& name)
    {
        return name.toLowerCase().removeCharacters(" ");
    }

    inline String toID (PID pID) {
        return toID(toName(pID));
    }

    inline void createFloatParam(UniqueRAPVector& vec, PID pID, float min, float max, float defaultValue, bool skew=false, float centreSkew=0.0f) {
    
        const auto name = toName(pID);
        const auto ID = toID(name);
        
        std::function< juce::String(float, int)> stv = [](float value, int maximumStringLength)
        {
            if (fabs(value) < 0.1f)
                return juce::String(value, 3);
            else
                return juce::String(value, 2);
        };
            
        std::function< float(const juce::String)> vts = [](const juce::String &text)
        {
            return text.getFloatValue();
        };
            
        RangeF range(min, max);
        if(skew)
            range.setSkewForCentre(centreSkew);
        vec.push_back(std::make_unique<APF>
        (
             juce::ParameterID {ID, 1},
             name,
             range,
             defaultValue,
             "",
             juce::AudioProcessorParameter::genericParameter,
             stv,
             vts));
    };
    
    inline Layout createParamterLayout()
    {
        UniqueRAPVector params;
        
        createFloatParam(params, PID::StartSpeed, 0, 20, 10);
        createFloatParam(params, PID::Duration, 0.01, 4, 1);
        createFloatParam(params, PID::BufferLength, 1.f/32.f, 4, 1);
        createFloatParam(params, PID::Power, -10, 10, 0);
        
        params.push_back(std::make_unique<APB>
        (
         juce::ParameterID {toID(PID::Trigger), 1},
         toName(PID::Trigger),
         false
        ));
        
        params.push_back(std::make_unique<APB>
        (
         juce::ParameterID {toID(PID::Direction), 1},
         toName(PID::Direction),
         true
        ));
        
        createFloatParam(params, PID::Gain, -60, 0, 0, true, -6);

        return {params.begin(), params.end()};
    }
}
