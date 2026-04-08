#include "PluginProcessor.h"
#include "PluginEditor.h"

BiquadLPFAudioProcessor::BiquadLPFAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

BiquadLPFAudioProcessor::~BiquadLPFAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout BiquadLPFAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "cutoff", "Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
        1000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "resonance", "Resonance",
        juce::NormalisableRange<float>(0.5f, 10.0f, 0.01f),
        0.707f));

    return layout;
}

const juce::String BiquadLPFAudioProcessor::getName() const { return JucePlugin_Name; }
bool BiquadLPFAudioProcessor::acceptsMidi() const { return false; }
bool BiquadLPFAudioProcessor::producesMidi() const { return false; }
bool BiquadLPFAudioProcessor::isMidiEffect() const { return false; }
double BiquadLPFAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int BiquadLPFAudioProcessor::getNumPrograms() { return 1; }
int BiquadLPFAudioProcessor::getCurrentProgram() { return 0; }
void BiquadLPFAudioProcessor::setCurrentProgram (int index) { juce::ignoreUnused (index); }
const juce::String BiquadLPFAudioProcessor::getProgramName (int index) { juce::ignoreUnused (index); return {}; }
void BiquadLPFAudioProcessor::changeProgramName (int index, const juce::String& newName) { juce::ignoreUnused (index, newName); }

void BiquadLPFAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    currentSampleRate = sampleRate;

    xPrev1L = xPrev1R = 0.0f;
    xPrev2L = xPrev2R = 0.0f;
    yPrev1L = yPrev1R = 0.0f;
    yPrev2L = yPrev2R = 0.0f;
}

void BiquadLPFAudioProcessor::releaseResources() {}

bool BiquadLPFAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

void BiquadLPFAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float cutoffFrequency = apvts.getRawParameterValue("cutoff")->load();
    float qValue          = apvts.getRawParameterValue("resonance")->load();

    const float pi    = 3.14159265359f;
    const float w_0   = 2.0f * pi * (cutoffFrequency / static_cast<float>(currentSampleRate));
    const float alpha = std::sin(w_0) / (2.0f * qValue);

    const float b_0 = (1.0f - std::cos(w_0)) / 2.0f;
    const float b_1 =  1.0f - std::cos(w_0);
    const float b_2 = (1.0f - std::cos(w_0)) / 2.0f;
    const float a_0 =  1.0f + alpha;
    const float a_1 = -2.0f * std::cos(w_0);
    const float a_2 =  1.0f - alpha;

    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float xL = channelDataL[sample];
        float xR = channelDataR[sample];

        float yL = (b_0/a_0 * xL) + (b_1/a_0 * xPrev1L) + (b_2/a_0 * xPrev2L)
                 - (a_1/a_0 * yPrev1L) - (a_2/a_0 * yPrev2L);
        float yR = (b_0/a_0 * xR) + (b_1/a_0 * xPrev1R) + (b_2/a_0 * xPrev2R)
                 - (a_1/a_0 * yPrev1R) - (a_2/a_0 * yPrev2R);

        xPrev2L = xPrev1L;  xPrev2R = xPrev1R;
        xPrev1L = xL;       xPrev1R = xR;
        yPrev2L = yPrev1L;  yPrev2R = yPrev1R;
        yPrev1L = yL;       yPrev1R = yR;

        channelDataL[sample] = yL;
        channelDataR[sample] = yR;
    }
}

bool BiquadLPFAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* BiquadLPFAudioProcessor::createEditor()
{
    return new BiquadLPFAudioProcessorEditor(*this);
}

void BiquadLPFAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void BiquadLPFAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BiquadLPFAudioProcessor();
}
