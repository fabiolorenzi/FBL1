#include "Knob.h"

void KnobLookAndFeel::drawRotarySlider(juce::Graphics& graphics, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(4);

    // ====== Knob background (outer ring) ======
    graphics.setColour(juce::Colours::darkgrey.darker(0.8f));
    graphics.fillEllipse(bounds);

    graphics.setColour(juce::Colours::black);
    graphics.drawEllipse(bounds, 1.5f);

    // ====== Knob fill (main knob face) ======
    auto knobBounds = bounds.reduced(6);
    juce::ColourGradient gradient(knobColour.brighter(0.2f), knobBounds.getCentreX(), knobBounds.getY(), knobColour.darker(0.6f), knobBounds.getCentreX(), knobBounds.getBottom(), true);

    graphics.setGradientFill(gradient);
    graphics.fillEllipse(knobBounds);

    graphics.setColour(juce::Colours::black.withAlpha(0.6f));
    graphics.drawEllipse(knobBounds, 1.0f);

    // ====== Pointer (indicator line) ======
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    juce::Path p;
    auto pointerLength = knobBounds.getWidth() * 0.4f;
    auto pointerThickness = 2.0f;
    p.addRectangle(-pointerThickness * 0.5f, -knobBounds.getHeight() / 2.5f, pointerThickness, pointerLength);

    graphics.setColour(juce::Colours::whitesmoke);
    graphics.fillPath(p, juce::AffineTransform::rotation(angle).translated(knobBounds.getCentreX(), knobBounds.getCentreY()));
}

Knob::Knob(const juce::String& name, juce::AudioProcessorValueTreeState& state, const juce::String& paramID, float _minVal, float _maxVal, juce::String _unit, juce::Colour knobColour, bool _hasPopup, juce::Component* parent) {
    minVal = _minVal;
    maxVal = _maxVal;
    unit = _unit;
    hasPopup = _hasPopup;

    knobLookAndFeel.knobColour = knobColour;

    label.setText(name, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    knob.setLookAndFeel(&knobLookAndFeel);
    if (hasPopup) {
        knob.setPopupDisplayEnabled(true, false, parent);
    }

    addAndMakeVisible(knob);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, paramID, knob);
}

void Knob::resized() {
    auto bounds = getLocalBounds();

    label.setBounds((bounds.getWidth() - 60) / 2, 25, 60, 14);

    const int knobSize = 60;

    juce::Rectangle<int> knobArea = bounds;
    juce::Rectangle<int> knobBounds(
        (knobArea.getWidth() - knobSize) / 2,
        (knobArea.getHeight() - knobSize) / 2,
        knobSize,
        knobSize
    );

    knob.setBounds(knobBounds);
}

void Knob::paint(juce::Graphics& g) {
    const int numTicks = 6;

    const int knobSize = 60;
    const int labelHeight = 20;
    juce::Rectangle<int> knobArea = getLocalBounds().withTrimmedTop(labelHeight);
    
    juce::Point<float> centre = knob.getBounds().toFloat().getCentre();
    float radius = knobSize / 2.0f + 1.0f;

    float rotaryStartAngle = juce::MathConstants<float>::pi * 1.2f;
    float rotaryEndAngle = juce::MathConstants<float>::pi * 2.8f;

    g.setFont(10.0f);
    g.setColour(juce::Colours::white);

    for (int i = 0; i <= numTicks; ++i) {
        float tempValue = juce::jmap((float)i, 0.0f, (float)numTicks, minVal, maxVal);
        float minValueFormatted = formatValue(minVal);
        float maxValueFormatted = formatValue(maxVal);
        juce::String unitFormatted = formatUnit(tempValue);

        float value = juce::jmap((float)i, 0.0f, (float)numTicks, minValueFormatted, maxValueFormatted);
        float proportion = (float)i / (float)numTicks;
        float tickAngle = rotaryStartAngle + proportion * (rotaryEndAngle - rotaryStartAngle);

        juce::Point<float> tickStart(0.0f, -radius);
        juce::Point<float> tickEnd(0.0f, -radius - 5.0f);

        auto rotation = juce::AffineTransform::rotation(tickAngle);
        tickStart = tickStart.transformedBy(rotation) + centre;
        tickEnd = tickEnd.transformedBy(rotation) + centre;

        g.drawLine({ tickStart, tickEnd }, 1.2f);

        if (i == 0 || i == numTicks) {
            juce::String text = juce::String(value, 0) + unitFormatted;

            juce::Point<float> textPoint(0.0f, -radius - 12.0f);
            textPoint = textPoint.transformedBy(rotation) + centre;

            g.drawFittedText(text, 
                (int)textPoint.getX() - 15, 
                (int)textPoint.getY() - 7, 
                30, 14, 
                juce::Justification::centred, 1);
        }
    }
}

float Knob::formatValue(float value) {
    if (value >= 1000) {
        return value / 1000;
    }
    return value;
}

juce::String Knob::formatUnit(float value) {
    if (value >= 1000) {
        return "k" + unit;
    }
    return unit;
}