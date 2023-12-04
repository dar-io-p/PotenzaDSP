/*
  ==============================================================================

    Params.h
    Created: 29 Aug 2023 11:04:30am
    Author:  Dario Potenza

  ==============================================================================
*/

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

    enum class PID {
        Threshold,
        Knee,
        InputGain,
        OutputGain,
        Oversampling,
        NumParams,
    };
    static constexpr int NumParams = static_cast<int>(PID::NumParams);

    inline String toName(PID pID)
    {
        switch (pID)
        {
            case PID::Threshold:
                return "Threshold";
            case PID::Knee:
                return "Knee";
            case PID::InputGain:
                return "Input Gain";
            case PID::OutputGain:
                return "OutputGain";
            case PID::Oversampling:
                return "Oversampling";
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
    
    namespace cook
    {
        inline float threshold(float t)
        {
            return juce::Decibels::decibelsToGain(t);
        }
        inline float knee(float k)
        {
            return k / 100.f;
        }
    }

    using ValToStr = std::function<String(float, int)>;
    using StrToVal = std::function<float(const String&)>;

    namespace valToStr
    {
        inline ValToStr db(){
            return [](float value, int maxLen){
                return String(value, 2) + " dB";
            };
        }

        inline ValToStr percent(){
            return [](float value, int maxLen){
                return String(value, 2) + " %";
            };
        }
    }
    
    namespace strToVal
    {
        inline StrToVal db() {
            return [](const String& text){
                return text.removeCharacters(" dB").getFloatValue();
            };
        }

        inline StrToVal percent() {
            return [](const String& text){
                return text.removeCharacters(" %").getFloatValue();
            };
        }
    }

    inline void createParam(UniqueRAPVector& vec, PID pID, float min, float max, float defaultValue, const String& unit, ValToStr vts, StrToVal stv, float midPoint) {
    
    const auto name = toName(pID);
    const auto ID = toID(name);
    
    RangeF range(min, max);
    range.setSkewForCentre(midPoint);
    
    vec.push_back(std::make_unique<APF>
    (
     juce::ParameterID {ID, 1},
     name,
     range,
     defaultValue,
     unit,
     APPCategory::genericParameter,
     vts,
     stv
     ));
    }
    
    inline Layout createParamterLayout()
    {
        UniqueRAPVector params;
        createParam(params, PID::Threshold, -100.f, 0.f, +0.f, "dB", valToStr::db(), strToVal::db(), -12.f);
        createParam(params, PID::Knee, 0.f, 100.f, 5.f, "%", valToStr::percent(), strToVal::percent(), 20.f);
        createParam(params, PID::InputGain, -100.f, 12.f, +0.0f, "dB", valToStr::db(), strToVal::db(), 0.f);
        createParam(params, PID::OutputGain, -100.f, 12.f, +0.0f, "dB", valToStr::db(), strToVal::db(), 0.f);
        params.push_back(std::make_unique<APB>
        (
         juce::ParameterID {toID(PID::Oversampling), 1},
         toName(PID::Oversampling),
         false
        ));

        return {params.begin(), params.end()};
    }
}
