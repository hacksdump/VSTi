#include "PluginEditor.h"

static void setupRotarySlider (juce::Slider& slider, juce::Label& label,
                               const juce::String& text, juce::Component& parent)
{
    slider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 18);
    parent.addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
    parent.addAndMakeVisible (label);
}

SineSynthAudioProcessorEditor::SineSynthAudioProcessorEditor (SineSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setupRotarySlider (attackSlider,  attackLabel,  "Attack",  *this);
    setupRotarySlider (decaySlider,   decayLabel,   "Decay",   *this);
    setupRotarySlider (sustainSlider, sustainLabel, "Sustain", *this);
    setupRotarySlider (releaseSlider, releaseLabel, "Release", *this);

    attackAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "attack",  attackSlider);
    decayAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "decay",   decaySlider);
    sustainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "sustain", sustainSlider);
    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "release", releaseSlider);

    setSize (400, 300);
}

SineSynthAudioProcessorEditor::~SineSynthAudioProcessorEditor() {}

void SineSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    g.drawText ("Sine Synth", getLocalBounds().removeFromTop (40), juce::Justification::centred);
}

void SineSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (10).withTrimmedTop (50);
    int knobWidth = area.getWidth() / 4;

    attackSlider .setBounds (area.removeFromLeft (knobWidth));
    decaySlider  .setBounds (area.removeFromLeft (knobWidth));
    sustainSlider.setBounds (area.removeFromLeft (knobWidth));
    releaseSlider.setBounds (area);
}
