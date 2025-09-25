#include "FrequencyScreen.h"

FrequencyScreen::FrequencyScreen(const juce::String& name, juce::AudioProcessorValueTreeState& state, const juce::String& paramID) {
    value.setJustificationType(juce::Justification::centred);
    value.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    value.setColour(juce::Label::textColourId, juce::Colours::limegreen);
    addAndMakeVisible(value);

    if (auto* param = state.getParameter(paramID)) {
        attachment = std::make_unique<juce::ParameterAttachment>(*param, [this, param](float) {
            value.setText(param->getCurrentValueAsText(), juce::dontSendNotification);
        }, nullptr);

        value.setText(param->getCurrentValueAsText(), juce::dontSendNotification);
    }
}

void FrequencyScreen::resized() {
    value.setBounds(getLocalBounds().reduced(4));
    //value.setBounds(4, 30, 60, 32);
}