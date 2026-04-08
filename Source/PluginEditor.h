/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BiquadLPFAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BiquadLPFAudioProcessorEditor (BiquadLPFAudioProcessor&);
    ~BiquadLPFAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    BiquadLPFAudioProcessor& audioProcessor;
    
    // Sliders
    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    
    // Labels
    juce::Label cutoffLabel;
    juce::Label resonanceLabel;
    
    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadLPFAudioProcessorEditor)
};
