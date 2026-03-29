#include "PluginEditor.h"

SineSynthAudioProcessorEditor::SineSynthAudioProcessorEditor (SineSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (400, 200);
}

SineSynthAudioProcessorEditor::~SineSynthAudioProcessorEditor() {}

void SineSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.drawText ("Sine Synth", getLocalBounds(), juce::Justification::centred);
}

void SineSynthAudioProcessorEditor::resized() {}