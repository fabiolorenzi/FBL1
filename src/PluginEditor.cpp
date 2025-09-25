#include "PluginEditor.h"
#include "PluginProcessor.h"

PluginEditor::PluginEditor(PluginProcessor& pluginProcessor)
    : AudioProcessorEditor(&pluginProcessor),
      pluginProcessor(pluginProcessor),
      crossLKnob("Cr L", pluginProcessor.apvts, "CROSSL", 30.0f, 1000.0f, "Hz", juce::Colours::red, false, this),
      crossMLKnob("Cr ML", pluginProcessor.apvts, "CROSSML", 60.0f, 3000.0f, "Hz", juce::Colours::orange, false, this),
      crossMKnob("Cr M", pluginProcessor.apvts, "CROSSM", 150.0f, 8000.0f, "Hz", juce::Colours::turquoise, false, this),
      crossMHKnob("Cr MH", pluginProcessor.apvts, "CROSSMH", 1000.0f, 16000.0f, "Hz", juce::Colours::blue, false, this),
      lowScreen("", pluginProcessor.apvts, "CROSSL"),
      midLowScreen("", pluginProcessor.apvts, "CROSSML"),
      midScreen("", pluginProcessor.apvts, "CROSSM"),
      midHighScreen("", pluginProcessor.apvts, "CROSSMH"),
      outputKnob("Output", pluginProcessor.apvts, "OUTPUT", -100.0f, 6.0f, "dB", juce::Colours::grey, true, this)
{
    setSize(500, 500);

    addAndMakeVisible(crossLKnob);
    addAndMakeVisible(crossMLKnob);
    addAndMakeVisible(crossMKnob);
    addAndMakeVisible(crossMHKnob);
    addAndMakeVisible(lowScreen);
    addAndMakeVisible(midLowScreen);
    addAndMakeVisible(midScreen);
    addAndMakeVisible(midHighScreen);

    // addAndMakeVisible(outputKnob);
}

PluginEditor::~PluginEditor() = default;

void PluginEditor::paint(juce::Graphics& graphics) {
    juce::ColourGradient backgroundGradient(
        juce::Colours::darkgrey.brighter(0.1f),
        getWidth() / 2.0f, 0.0f,
        juce::Colours::darkgrey.darker(0.5f),
        getWidth() / 2.0f, (float)getHeight(),
        false
    );

    graphics.setGradientFill(backgroundGradient);
    graphics.fillAll();

    auto area = getLocalBounds();

    auto topArea = area.removeFromTop(200);
    auto lowArea = area.removeFromTop(300);

    graphics.setColour(juce::Colours::grey);
    graphics.drawRect(topArea, 2);
    graphics.drawRect(lowArea, 2);

    graphics.setColour(juce::Colours::whitesmoke);
    graphics.setFont(juce::Font(16.0f, juce::Font::bold));
    graphics.drawText("FBL1  ", getLocalBounds().removeFromTop(30).removeFromRight(140), juce::Justification::centredRight, false);
}

void PluginEditor::resized() {
    crossLKnob.setBounds(10, 0, 100, 160);
    crossMLKnob.setBounds(120, 0, 100, 160);
    crossMKnob.setBounds(230, 0, 100, 160);
    crossMHKnob.setBounds(340, 0, 100, 160);
    lowScreen.setBounds(85, 125, 60, 35);
    midLowScreen.setBounds(195, 125, 60, 35);
    midScreen.setBounds(305, 125, 60, 35);
    midHighScreen.setBounds(415, 125, 60, 35);
}