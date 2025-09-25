#pragma once

#include <JuceHeader.h>

class FrequencyScreen : public juce::Component {
    public:
        FrequencyScreen(const juce::String& name, juce::AudioProcessorValueTreeState& state, const juce::String& paramID);
        void resized() override;
    
    private:
        juce::Label value;
        std::unique_ptr<juce::ParameterAttachment> attachment;
};