#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SineSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SineSynthAudioProcessorEditor (SineSynthAudioProcessor&);
    ~SineSynthAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SineSynthAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SineSynthAudioProcessorEditor)
};