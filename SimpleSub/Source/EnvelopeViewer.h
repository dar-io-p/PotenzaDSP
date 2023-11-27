#pragma once

#include <JuceHeader.h>

class EnvelopeViewer  : public juce::Component
{
public:
    EnvelopeViewer()
    {
        envelope.prepare({100, 100, 1});
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        auto w = bounds.getWidth();
        auto h = bounds.getHeight();
        juce::Path p;
        envelope.gate(true);
        int sustainCount = 0;
        
        auto pad = 10;
        
        p.startNewSubPath(pad, h - pad);
        for (int x = pad; x < w - pad; x++)
        {
            if (envelope.getState() == SubSynth::Envelope::Sustain)
            {
                sustainCount++;
            }
            if (sustainCount > 100)
                envelope.gate(false);
            auto y = juce::jmap(envelope.getNextValue(), 0.f, 1.f, (float) h - pad, (float) pad);
            p.lineTo(x, y);
        }
        g.setColour(juce::Colours::white);
        g.strokePath(p, juce::PathStrokeType(2.f));
    }

    void resized() override
    {

    }
    
    void update(float a, float d, float s, float r, float a_curve, float dr_curve)
    {
        envelope.update(a, d, s, r);
        envelope.setTargetRatioA(a_curve);
        envelope.setTargetRatioDR(dr_curve);
    }

private:
    SubSynth::Envelope envelope;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeViewer)
};
