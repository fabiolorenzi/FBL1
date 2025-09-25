#include "StepKnob.h"

StepKnob::StepKnob(const juce::String& name, juce::AudioProcessorValueTreeState& state, const juce::String& paramID, juce::Component* parent) {
    knobLookAndFeel.knobColour = juce::Colours::grey;

    label.setText(name, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    knob.setLookAndFeel(&knobLookAndFeel);

    knob.setRange(0, 4, 1);
    addAndMakeVisible(knob);
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, paramID, knob);
}

void StepKnob::resized() {
    auto bounds = getLocalBounds();

    label.setBounds(bounds.removeFromTop(10));

    const int knobSize = 120;
    juce::Rectangle<int> knobBounds(
        (bounds.getWidth() - knobSize) / 2,
        (bounds.getHeight() - knobSize) / 2,
        knobSize,
        knobSize
    );

    knob.setBounds(knobBounds);
}

void StepKnob::paint(juce::Graphics& graphics) {
    const int numSteps = 4;

    const int knobSize = 120;
    const int labelHeight = 20;
    juce::Rectangle<int> knobArea = getLocalBounds().withTrimmedTop(labelHeight);

    juce::Point<float> centre = knob.getBounds().toFloat().getCentre();
    float radius = knobSize / 2.0f + 6.0f;

    float rotaryStartAngle = juce::MathConstants<float>::pi * 1.2f;
    float rotaryEndAngle   = juce::MathConstants<float>::pi * 2.8f;

    graphics.setFont(12.0f);
    graphics.setColour(juce::Colours::white);

    for (int i = 0; i <= numSteps; ++i) {
        float proportion = (float)i / (float)numSteps;
        float tickAngle = rotaryStartAngle + proportion * (rotaryEndAngle - rotaryStartAngle);

        juce::Point<float> tickStart(0.0f, -radius);
        juce::Point<float> tickEnd(0.0f, -radius - 6.0f);

        auto rotation = juce::AffineTransform::rotation(tickAngle);
        tickStart = tickStart.transformedBy(rotation) + centre;
        tickEnd = tickEnd.transformedBy(rotation) + centre;

        graphics.drawLine({tickStart, tickEnd}, 1.2f);

        juce::String text = stepLabels[i];
        juce::Point<float> textPoint(0.0f, -radius - 18.0f);
        textPoint = textPoint.transformedBy(rotation) + centre;

        graphics.drawFittedText(text, (int)textPoint.getX() - 12, (int)textPoint.getY() - 7, 24, 14, juce::Justification::centred, 1);
    }
}