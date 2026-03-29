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

    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label  attackLabel,  decayLabel,  sustainLabel,  releaseLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        attackAttach, decayAttach, sustainAttach, releaseAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SineSynthAudioProcessorEditor)
};
