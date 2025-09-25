#pragma once

#include <JuceHeader.h>
#include "FrequencyScreen.h"
#include "Knob.h"

class PluginProcessor;

class PluginEditor : public juce::AudioProcessorEditor {
    public:
        PluginEditor(PluginProcessor& pluginProcessor);
        ~PluginEditor() override;
        void paint(juce::Graphics& graphics) override;
        void resized() override;
    
    private:
        PluginProcessor& pluginProcessor;
        Knob crossLKnob;
        Knob crossMLKnob;
        Knob crossMKnob;
        Knob crossMHKnob;
        Knob outputKnob;
        FrequencyScreen lowScreen;
        FrequencyScreen midLowScreen;
        FrequencyScreen midScreen;
        FrequencyScreen midHighScreen;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor);
};