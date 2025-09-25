#pragma once

#include <JuceHeader.h>

struct FlatButtonLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        FlatButtonLookAndFeel();
        ~FlatButtonLookAndFeel() override;
        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton, bool isButtonDown) override;
        juce::Path createTextPath(juce::ToggleButton& button, juce::Rectangle<float> bounds);
};

class FlatButton : public juce::Component {
    public:
        FlatButton(const juce::String& name, juce::AudioProcessorValueTreeState& state, const juce::String& paramID);
        void resized() override;
    
    private:
        juce::Label label;
        juce::ToggleButton button;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
        FlatButtonLookAndFeel flatButtonLookAndFeel;
};