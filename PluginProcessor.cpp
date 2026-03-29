#include "PluginProcessor.h"
#include "PluginEditor.h"

SineSynthAudioProcessor::SineSynthAudioProcessor() {}
SineSynthAudioProcessor::~SineSynthAudioProcessor() {}

void SineSynthAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;
    releaseTimeSamples = static_cast<float>(sampleRate * 0.01); // 10ms release
    for (auto& v : voices)
        v = Voice{};
}

void SineSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    int samplePos = 0;

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        int msgPos = metadata.samplePosition;

        // Render audio up to this MIDI event
        for (; samplePos < msgPos; ++samplePos)
        {
            float sample = 0.0f;
            for (auto& v : voices)
            {
                if (!v.active) continue;
                sample += std::sin(v.phase) * v.gain * 0.2f;
                v.phase += v.phaseIncrement;
                if (v.phase >= juce::MathConstants<double>::twoPi)
                    v.phase -= juce::MathConstants<double>::twoPi;
                v.gain += v.gainStep;
                if (v.releasing && v.gain <= 0.0f)
                {
                    v.gain = 0.0f;
                    v.active = false;
                    v.releasing = false;
                }
                else if (!v.releasing && v.gain >= v.velocity)
                {
                    v.gain = v.velocity;
                    v.gainStep = 0.0f;
                }
            }
            for (int ch = 0; ch < numChannels; ++ch)
                buffer.setSample(ch, samplePos, sample);
        }

        if (msg.isNoteOn())
        {
            // Find a free voice (or steal the first one)
            Voice* voice = nullptr;
            for (auto& v : voices)
                if (!v.active) { voice = &v; break; }
            if (!voice)
                voice = &voices[0];

            voice->noteNumber = msg.getNoteNumber();
            voice->velocity = msg.getFloatVelocity();
            voice->phase = 0.0;
            voice->phaseIncrement = juce::MathConstants<double>::twoPi
                * juce::MidiMessage::getMidiNoteInHertz(msg.getNoteNumber())
                / currentSampleRate;
            voice->gain = 0.0f;
            voice->gainStep = voice->velocity / releaseTimeSamples; // ~10ms attack
            voice->active = true;
            voice->releasing = false;
        }
        else if (msg.isNoteOff())
        {
            for (auto& v : voices)
                if (v.active && v.noteNumber == msg.getNoteNumber())
                {
                    v.releasing = true;
                    v.gainStep = -v.gain / releaseTimeSamples; // ~10ms release
                }
        }
    }

    // Render remaining samples after last MIDI event
    for (; samplePos < numSamples; ++samplePos)
    {
        float sample = 0.0f;
        for (auto& v : voices)
        {
            if (!v.active) continue;
            sample += std::sin(v.phase) * v.gain * 0.2f;
            v.phase += v.phaseIncrement;
            if (v.phase >= juce::MathConstants<double>::twoPi)
                v.phase -= juce::MathConstants<double>::twoPi;
            v.gain += v.gainStep;
            if (v.releasing && v.gain <= 0.0f)
            {
                v.gain = 0.0f;
                v.active = false;
                v.releasing = false;
            }
            else if (!v.releasing && v.gain >= v.velocity)
            {
                v.gain = v.velocity;
                v.gainStep = 0.0f;
            }
        }
        for (int ch = 0; ch < numChannels; ++ch)
            buffer.setSample(ch, samplePos, sample);
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
