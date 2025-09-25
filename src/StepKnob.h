#pragma once

#include <JuceHeader.h>
#include "Knob.h"

class StepKnob : public juce::Component {
    public:
        StepKnob(const juce::String& name, juce::AudioProcessorValueTreeState& state, const juce::String& paramID, juce::Component* parent);
        void resized() override;
        void paint(juce::Graphics& graphics) override;

    private:
        juce::Label label;
        juce::Slider knob;
        std::array<juce::String, 5> stepLabels {"L", "ML", "M", "MH", "H"};
        KnobLookAndFeel knobLookAndFeel;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};