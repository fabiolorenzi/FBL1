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

    float crossL = apvts.getRawParameterValue("CROSSL")->load();
    float crossML = apvts.getRawParameterValue("CROSSML")->load();
    float crossM = apvts.getRawParameterValue("CROSSM")->load();
    float crossMH = apvts.getRawParameterValue("CROSSMH")->load();
    int bandSelector = apvts.getRawParameterValue("BANDSELECTOR")->load();
    bool activated = apvts.getRawParameterValue("ACTIVATE")->load();

    float outputGain = outputGainParam->load() <= -99.0f ? 0.0f : juce::Decibels::decibelsToGain(outputGainParam->load());

    
    float hpFreq;
    float lpFreq;

    if (bandSelector == 0) {
        hpFreq = 0.0f;
        lpFreq = crossL;
    } else if (bandSelector == 1) {
        hpFreq = crossL;
        lpFreq = crossML;
    } else if (bandSelector == 2) {
        hpFreq = crossML;
        lpFreq = crossM;
    } else if (bandSelector == 3) {
        hpFreq = crossM;
        lpFreq = crossMH;
    } else {
        hpFreq = crossMH;
        lpFreq = 22000.0f;
    }

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float* channelData = buffer.getWritePointer(channel);

        juce::dsp::AudioBlock<float> block(buffer);
        auto singleChannelBlock = block.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> context(singleChannelBlock);

        if (activated) {
            if (bandSelector == 0) {
                *lpFilter[channel].state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), lpFreq, std::sqrt(0.5f));
                lpFilter[channel].process(context);
            } else if (bandSelector == 4) {
                *hpFilter[channel].state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), hpFreq, std::sqrt(0.5f));
                hpFilter[channel].process(context);
            } else {
                *hpFilter[channel].state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), hpFreq, std::sqrt(0.5f));
                *lpFilter[channel].state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), lpFreq, std::sqrt(0.5f));

                hpFilter[channel].process(context);
                lpFilter[channel].process(context);
            }
        }

        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            channelData[i] *= outputGain;
        }
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

    layout.add(std::make_unique<juce::AudioParameterFloat>("CROSSL", "Cross L", juce::NormalisableRange<float>(30.0f, 1000.0f, 1.0f, 0.5f), 80.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CROSSML", "Cross ML", juce::NormalisableRange<float>(60.0f, 3000.0f, 1.0f, 0.5f), 550.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CROSSM", "Cross M", juce::NormalisableRange<float>(150.0f, 8000.0f, 1.0f, 0.5f), 4000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CROSSMH", "Cross MH", juce::NormalisableRange<float>(1000.0f, 16000.0f, 1.0f, 0.5f), 11000.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>("BANDSELECTOR", "Band Selector", 0, 4, 0));
    layout.add(std::make_unique<juce::AudioParameterBool>("ACTIVATE", "Activate", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OUTPUT", "Output Gain", juce::NormalisableRange<float>(-100.0f, 6.0f, 0.1f, 3.0f), 0.0f));

    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new PluginProcessor();
}