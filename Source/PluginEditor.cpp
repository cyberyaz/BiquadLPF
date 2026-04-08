/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BiquadLPFAudioProcessorEditor::BiquadLPFAudioProcessorEditor (BiquadLPFAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Setup Cutoff Slider
    cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    cutoffSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(cutoffSlider);
    
    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffLabel.setJustificationType(juce::Justification::centred);
    cutoffLabel.attachToComponent(&cutoffSlider, false);
    addAndMakeVisible(cutoffLabel);
    
    // Setup Resonance Slider
    resonanceSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(resonanceSlider);
    
    resonanceLabel.setText("Resonance", juce::dontSendNotification);
    resonanceLabel.setJustificationType(juce::Justification::centred);
    resonanceLabel.attachToComponent(&resonanceSlider, false);
    addAndMakeVisible(resonanceLabel);
    
    // Attach sliders to parameters
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "cutoff", cutoffSlider);
    
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "resonance", resonanceSlider);
    
    setSize (400, 300);
}

BiquadLPFAudioProcessorEditor::~BiquadLPFAudioProcessorEditor()
{
}

//==============================================================================
void BiquadLPFAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawFittedText ("Biquad LPF", getLocalBounds().removeFromTop(50),
                      juce::Justification::centred, 1);
}

void BiquadLPFAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    // Skip title area
    area.removeFromTop(50);
    
    // Add some padding
    area.reduce(20, 20);
    
    // Split into two columns for the sliders
    auto leftArea = area.removeFromLeft(area.getWidth() / 2);
    auto rightArea = area;
    
    // Position cutoff slider (left)
    cutoffSlider.setBounds(leftArea.reduced(20));
    
    // Position resonance slider (right)
    resonanceSlider.setBounds(rightArea.reduced(20));
}
