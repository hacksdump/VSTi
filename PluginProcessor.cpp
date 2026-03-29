#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout SineSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("attack", 1), "Attack",
        juce::NormalisableRange<float> (0.0f, 5.0f, 0.001f, 0.3f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("decay", 1), "Decay",
        juce::NormalisableRange<float> (0.0f, 5.0f, 0.001f, 0.3f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("sustain", 1), "Sustain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("release", 1), "Release",
        juce::NormalisableRange<float> (0.0f, 5.0f, 0.001f, 0.3f), 0.0f));

    return { params.begin(), params.end() };
}

SineSynthAudioProcessor::SineSynthAudioProcessor()
    : apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

SineSynthAudioProcessor::~SineSynthAudioProcessor() {}

void SineSynthAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;
    for (auto& v : voices)
        v = Voice{};
}

void SineSynthAudioProcessor::advanceEnvelope (Voice& v, float attackSamples, float decaySamples,
                                                float sustainLevel, float releaseSamples)
{
    v.gain += v.gainStep;

    switch (v.stage)
    {
        case EnvStage::Attack:
            if (v.gain >= v.velocity)
            {
                v.gain = v.velocity;
                if (decaySamples > 0.0f && sustainLevel < 1.0f)
                {
                    v.stage = EnvStage::Decay;
                    v.targetGain = v.velocity * sustainLevel;
                    v.gainStep = (v.targetGain - v.gain) / decaySamples;
                }
                else
                {
                    v.stage = EnvStage::Sustain;
                    v.gain = v.velocity * sustainLevel;
                    v.gainStep = 0.0f;
                }
            }
            break;

        case EnvStage::Decay:
            if ((v.gainStep < 0.0f && v.gain <= v.targetGain) ||
                (v.gainStep >= 0.0f && v.gain >= v.targetGain))
            {
                v.gain = v.targetGain;
                v.gainStep = 0.0f;
                v.stage = EnvStage::Sustain;
            }
            break;

        case EnvStage::Sustain:
            v.gainStep = 0.0f;
            break;

        case EnvStage::Release:
            if (v.gain <= 0.0f)
            {
                v.gain = 0.0f;
                v.gainStep = 0.0f;
                v.active = false;
                v.stage = EnvStage::Off;
            }
            break;

        case EnvStage::Off:
            break;
    }
}

void SineSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    float attackTime  = std::max (apvts.getRawParameterValue ("attack")->load(),  minEnvTimeSec);
    float decayTime   = apvts.getRawParameterValue ("decay")->load();
    float sustainLvl  = apvts.getRawParameterValue ("sustain")->load();
    float releaseTime = std::max (apvts.getRawParameterValue ("release")->load(), minEnvTimeSec);

    float attackSamples  = attackTime  * static_cast<float>(currentSampleRate);
    float decaySamples   = decayTime   * static_cast<float>(currentSampleRate);
    float releaseSamples = releaseTime * static_cast<float>(currentSampleRate);

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    int samplePos = 0;

    auto renderSamples = [&](int endPos)
    {
        for (; samplePos < endPos; ++samplePos)
        {
            float sample = 0.0f;
            for (auto& v : voices)
            {
                if (!v.active) continue;
                sample += static_cast<float>(std::sin (v.phase)) * v.gain * 0.2f;
                v.phase += v.phaseIncrement;
                if (v.phase >= juce::MathConstants<double>::twoPi)
                    v.phase -= juce::MathConstants<double>::twoPi;
                advanceEnvelope (v, attackSamples, decaySamples, sustainLvl, releaseSamples);
            }
            for (int ch = 0; ch < numChannels; ++ch)
                buffer.setSample (ch, samplePos, sample);
        }
    };

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        int msgPos = metadata.samplePosition;

        renderSamples (msgPos);

        if (msg.isNoteOn())
        {
            Voice* voice = nullptr;
            for (auto& v : voices)
                if (!v.active) { voice = &v; break; }
            if (!voice)
                voice = &voices[0];

            voice->noteNumber = msg.getNoteNumber();
            voice->velocity = msg.getFloatVelocity();
            voice->phase = 0.0;
            voice->phaseIncrement = juce::MathConstants<double>::twoPi
                * juce::MidiMessage::getMidiNoteInHertz (msg.getNoteNumber())
                / currentSampleRate;
            voice->gain = 0.0f;
            voice->gainStep = voice->velocity / attackSamples;
            voice->stage = EnvStage::Attack;
            voice->active = true;
        }
        else if (msg.isNoteOff())
        {
            for (auto& v : voices)
            {
                if (v.active && v.noteNumber == msg.getNoteNumber() && v.stage != EnvStage::Release)
                {
                    v.stage = EnvStage::Release;
                    v.gainStep = -v.gain / releaseSamples;
                }
            }
        }
    }

    renderSamples (numSamples);
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
