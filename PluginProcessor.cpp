#include "PluginProcessor.h"
#include "PluginEditor.h"

SineSynthAudioProcessor::SineSynthAudioProcessor() {}
SineSynthAudioProcessor::~SineSynthAudioProcessor() {}

void SineSynthAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;
    phase = 0.0;
    phaseIncrement = juce::MathConstants<double>::twoPi * frequency / currentSampleRate;
}

void SineSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.clear();

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float value = std::sin(phase);

        phase += phaseIncrement;
        if (phase >= juce::MathConstants<double>::twoPi)
            phase -= juce::MathConstants<double>::twoPi;

        for (int ch = 0; ch < numChannels; ++ch)
            buffer.setSample(ch, sample, value * 0.2f);
    }
}

bool SineSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

juce::AudioProcessorEditor* SineSynthAudioProcessor::createEditor()
{
    return new SineSynthAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SineSynthAudioProcessor();
}