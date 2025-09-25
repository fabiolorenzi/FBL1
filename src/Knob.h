#pragma once

#include <JuceHeader.h>

struct KnobLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        juce::Colour knobColour;

        KnobLookAndFeel() = default;
        ~KnobLookAndFeel() override = default;
        void drawRotarySlider(juce::Graphics& graphics, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
};

class Knob : public juce::Component {
    public:
        Knob(const juce::String& name, juce::AudioProcessorValueTreeState& state, const juce::String& paramID, float _minValue, float _maxValue, juce::String _unit, juce::Colour knobColour, bool _hasPopup, juce::Component* parent);
        void resized() override;
        void paint(juce::Graphics& g) override;
    
    private:
        float minVal;
        float maxVal;
        bool hasPopup;
        juce::String unit;
        juce::Label label;
        juce::Slider knob;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
        KnobLookAndFeel knobLookAndFeel;

        float formatValue(float value);
        juce::String formatUnit(float value);
};