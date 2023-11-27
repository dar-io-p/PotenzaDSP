#pragma once
#include <JuceHeader.h>

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
    p_LFODepth,
    p_LFORate,
    p_LFO1Type,
    p_LFO2Type,
    p_LFOBypass,
    p_LFOBlend,
    p_oscFreq,
    p_oscShape,
    p_outGain,
    p_active,
    NumParams,
};

static constexpr int TotalNumParams = static_cast<int>(NumParams);

inline const std::map<PID, String>& GetParams(){
    static std::map<PID, String> params =
    {
        {p_LFODepth,"LFOdepth"},
        {p_LFORate,"LFORate"},
        {p_LFO1Type,"LFO1type"},
        {p_LFO2Type,"LFO2type"},
        {p_LFOBypass,"LFOBypass"},
        {p_LFOBlend, "LFOBlend"},
        {p_oscFreq,"BaseFrequency"},
        {p_oscShape,"OscFreq"},
        {p_outGain,"OutGain"},
        {p_active,"Activate"}
    };
    
    return params;
}
namespace LFO1
{
enum Type
{
    Square=0,
    Triangle=1,
    Sine=2
};
const static juce::StringArray choices ({
    "Square",
    "Triangle",
    "Sine"
});

}
namespace LFO2
{
enum Type
{
    Square=0,
    Triangle=1,
    Sine=2
};

const static juce::StringArray choices ({
    "SawUp",
    "SawDown",
    "Square"
});
}


inline String toName(PID pID)
{
    return GetParams().at(pID);
}

inline String toID(const String& name)
{
    return name.toLowerCase().removeCharacters(" ");
}

inline String toID (PID pID) {
    return toID(toName(pID));
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
    
inline ValToStr generic(){
    return [](float value, int maxLen){
        return String(value, 2);
    };
}

inline ValToStr mS() {
    return [](float value, int maxLen){
        return String(value, 2) + " mS";
    };
}

inline ValToStr Hz() {
    return [](float value, int maxLen){
        return String(value, 2) + " Hz";
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

inline StrToVal generic() {
    return [](const String& text){
        return text.getFloatValue();
    };
}

inline StrToVal mS() {
    return [](const String& text){
        return text.removeCharacters(" ms").getFloatValue();
    };
}

inline StrToVal Hz() {
    return [](const String& text){
        return text.removeCharacters(" Hz").getFloatValue();
    };
}
}

inline void createParamFloat(UniqueRAPVector& vec, PID pID, float min, float max, float defaultValue, ValToStr vts, StrToVal stv, float midPoint) {

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
     "",
     APPCategory::genericParameter,
     vts,
     stv
     ));
};

inline void createParamBool(UniqueRAPVector& vec, PID pID, bool defaultVal)
{
    const auto name = toName(pID);
    const auto ID = toID(name);
    
    vec.push_back(std::make_unique<APB>
    (
     juce::ParameterID {ID, 1},
     name,
     false
    ));
}

inline Layout createParamterLayout()
{
    UniqueRAPVector params;
//    p_oscShape,
//    p_active,
    
    createParamFloat(params, p_LFODepth, 0.0f, 1.0f, 0.5f, valToStr::generic(), strToVal::generic(), 0.5f);
    createParamFloat(params, p_LFORate, 0.05f, 10.f, .5f, valToStr::Hz(), strToVal::Hz(), 3.f);
    
    createParamBool(params, p_LFOBypass, false);
    
    createParamFloat(params, p_oscFreq, 40.f, 8000.f, 200.f, valToStr::Hz(), strToVal::Hz(), 440.f);
    createParamFloat(params, p_outGain, -100.f, 0.f, -12.f, valToStr::db(), strToVal::db(), -12.f);
    
    createParamBool(params, p_active, false);
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{toID(p_LFO1Type), 1},
                                                                  toName(p_LFO1Type),
                                                                  LFO1::choices,
                                                                  LFO1::Sine));
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{toID(p_LFO2Type), 1},
                                                                  toName(p_LFO2Type),
                                                                  LFO2::choices,
                                                                  LFO2::Square));
    
    createParamFloat(params, p_LFOBlend, 0.0f, 1.0f, 0.0f, valToStr::generic(), strToVal::generic(), 0.5f);
    
    
    return {params.begin(), params.end()};
}
}
