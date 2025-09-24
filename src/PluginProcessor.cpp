#include "PluginProcessor.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
    .withInput("Input", juce::AudioChannelSet::stereo(), true)
    .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ),
    apvts(*this, nullptr, "Parameters", createParameterLayout()) {
}

PluginProcessor::~PluginProcessor() = default;

const juce::String PluginProcessor::getName() const {
    return "FBL1";
}

bool PluginProcessor::acceptsMidi() const {
    return false;
}

bool PluginProcessor::producesMidi() const {
    return false;
}

bool PluginProcessor::isMidiEffect() const {
    return false;
}

double PluginProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int PluginProcessor::getNumPrograms() {
    return 1;
}

int PluginProcessor::getCurrentProgram() {
    return 0;
}

void PluginProcessor::setCurrentProgram(int index) {}

const juce::String PluginProcessor::getProgramName(int) {
    return {};
}

void PluginProcessor::changeProgramName(int index, const juce::String& newName) {}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    juce::dsp::ProcessSpec spec {
        sampleRate,
        (juce::uint32)samplesPerBlock,
        (juce::uint32)getMainBusNumOutputChannels()
    };

    for (int channel = 0; channel < 2; ++channel) {
        hpFilter[channel].prepare(spec);
        lpFilter[channel].prepare(spec);
    }
}

void PluginProcessor::releaseResources() {}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo() && (layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo() 
        || layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled());
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    auto* outputGainParam = apvts.getRawParameterValue("OUTPUT");

    float hpFreq = apvts.getRawParameterValue("HPFILTER")->load();
    float lpFreq = apvts.getRawParameterValue("LPFILTER")->load();
    float outputGain = outputGainParam->load() <= -99.0f ? 0.0f : juce::Decibels::decibelsToGain(outputGainParam->load());

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        *hpFilter[channel].state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), hpFreq, std::sqrt(0.5f));
        *lpFilter[channel].state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), lpFreq, std::sqrt(0.5f));

        juce::dsp::AudioBlock<float> block(buffer);
        auto singleChannelBlock = block.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> context(singleChannelBlock);

        hpFilter[channel].process(context);
        lpFilter[channel].process(context);
    }
}

bool PluginProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor() {
    //return new PluginEditor(*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes) {
    if (data != nullptr && sizeInBytes > 0) {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
        if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType())) {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("INPUT", "Input Gain", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HPFILTER", "Highpass", juce::NormalisableRange<float>(16.0f, 320.0f, 0.5f), 16.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LPFILTER", "Lowpass", juce::NormalisableRange<float>(2500.0f, 20000.0f, 0.5f), 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OUTPUT", "Output Gain", juce::NormalisableRange<float>(-100.0f, 6.0f, 0.1f, 3.0f), 0.0f));

    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new PluginProcessor();
}