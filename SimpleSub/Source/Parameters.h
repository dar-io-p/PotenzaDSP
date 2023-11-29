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
    param_attack = 0,
    param_decay,
    param_sustain,
    param_release,
    
    param_A_curve,
    param_DR_curve,
    
    param_out_gain,
    param_drive,
    
    param_harmonic_1,
    param_harmonic_2,
    param_harmonic_3,
    param_harmonic_4,
    param_harmonic_5,
    
    param_width,
    
    param_distortionIsActive,
    param_distorionType,
    
    param_glide,
    param_glide_active,
    
    NumParams,
};
static constexpr int TotalNumParams = static_cast<int>(NumParams);

inline const std::map<PID, String>& GetParams(){
    static std::map<PID, String> params =
    {
        {param_attack,"Attack"},
        {param_decay,"Decay"},
        {param_sustain,"Sustain"},
        {param_release,"Release"},
        {param_A_curve,"Attack Curve"},
        {param_DR_curve,"Decay Release Curve"},
        {param_out_gain,"Output Gain"},
        {param_drive, "Drive"},
        {param_harmonic_1, "Harmonic 1"},
        {param_harmonic_2, "Harmonic 2"},
        {param_harmonic_3, "Harmonic 3"},
        {param_harmonic_4, "Harmonic 4"},
        {param_harmonic_5, "Harmonic 5"},
        {param_width, "Width"},
        {param_distortionIsActive, "Distortion Active"},
        {param_distorionType, "Distortion Type"},
        {param_glide, "Glide"},
        {param_glide_active, "Glide Enabled"}
    };
    
    return params;
}

enum distortionIndices
{
    CubicAlgo = 0,
    TanhAlgo = 1,
    SinAlgo = 2,
    NumAlgo,
};

const static juce::StringArray distortionChoices ({
    "Cubic",
    "Tanh",
    "Sine",
});

inline const std::function<float(float)> getWaveShaper(distortionIndices index)
{
    switch (index) {
        case CubicAlgo:
            return [] (float x) {
                float sign = (x>0) - (x<0);
                float output = fabs(x) > 1 ? sign : 1.5*(x-(x*x*x)/3);
                return output;
            };
            break;
        case TanhAlgo:
            return [] (float x) {
                return juce::dsp::FastMathApproximations::tanh(juce::jlimit(-5.f, 5.f, x));
            };
        case SinAlgo:
            return [] (float x) {
                return juce::dsp::FastMathApproximations::sin(x);
            };
        case NumAlgo:
            jassertfalse;
            break;
    }
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
        return String(value, 1) + " dB";
    };
}

inline ValToStr percent(){
    return [](float value, int maxLen){
        return String(value, 2) + " %";
    };
    }
    
inline ValToStr generic(){
    return [](float value, int maxLen){
        if (value < 10.f)
            return String(value, 2);
        else if (value < 100.f)
            return String(value, 1);
        else
            return String((int)value);
    };
}

inline ValToStr mS() {
    return [](float value, int maxLen){
        if (value < 10.f)
            return String(value, 2) + " ms";
        else if (value < 100.f)
            return String(value, 1) + " ms";
        else
            return String((int)value) + " ms";
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
    
    createParamFloat(params, param_attack, 0, 2000, 5, valToStr::generic(), strToVal::generic(), 100);
    createParamFloat(params, param_decay, 0, 2000, 100, valToStr::generic(), strToVal::generic(), 100);
    createParamFloat(params, param_sustain, -100, 0, 0, valToStr::generic(), strToVal::generic(), -12);
    createParamFloat(params, param_release, 0, 2000, 5, valToStr::generic(), strToVal::generic(), 100);
    
    createParamFloat(params, param_A_curve, 0, 50, 50, valToStr::generic(), strToVal::generic(), 1);
    createParamFloat(params, param_DR_curve, 0, 50, 50, valToStr::generic(), strToVal::generic(), 1);
    
    createParamFloat(params, param_drive, -24, 24, -12, valToStr::db(), strToVal::db(), 0);
    createParamFloat(params, param_out_gain, -100, 12, -12, valToStr::db(), strToVal::db(), 0);
    
    createParamFloat(params, param_harmonic_1, 0, 1, 1, valToStr::generic(), strToVal::generic(), 0.25);
    createParamFloat(params, param_harmonic_2, 0, 1, 0, valToStr::generic(), strToVal::generic(), 0.25);
    createParamFloat(params, param_harmonic_3, 0, 1, 0, valToStr::generic(), strToVal::generic(), 0.25);
    createParamFloat(params, param_harmonic_4, 0, 1, 0, valToStr::generic(), strToVal::generic(), 0.25);
    createParamFloat(params, param_harmonic_5, 0, 1, 0, valToStr::generic(), strToVal::generic(), 0.25);
    
    createParamFloat(params, param_width, 0, 1, 0, valToStr::generic(), strToVal::generic(), 0.5);
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
                                                                  juce::ParameterID {toID(param_distorionType), 1},
                                                                  toName(param_distorionType),
                                                                  distortionChoices,
                                                                  TanhAlgo));
    createParamBool(params, param_distortionIsActive, false);
    
    
    createParamFloat(params, param_glide, 0.001, 1000, 0.001, valToStr::mS(), strToVal::mS(), 100);
    createParamBool(params, param_glide_active, false);
    
    return {params.begin(), params.end()};
}
}
