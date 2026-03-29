#pragma once
#include <JuceHeader.h>

class SineSynthAudioProcessor : public juce::AudioProcessor
{
public:
    SineSynthAudioProcessor();
    ~SineSynthAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "SineSynth"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    enum class EnvStage { Off, Attack, Decay, Sustain, Release };

    struct Voice
    {
        int noteNumber = -1;
        double phase = 0.0;
        double phaseIncrement = 0.0;
        float velocity = 0.0f;
        float gain = 0.0f;
        float gainStep = 0.0f;
        float targetGain = 0.0f;
        EnvStage stage = EnvStage::Off;
        bool active = false;
    };

    void advanceEnvelope (Voice& v, float attackSamples, float decaySamples,
                          float sustainLevel, float releaseSamples);

    static constexpr int maxVoices = 16;
    static constexpr float minEnvTimeSec = 0.01f; // 10ms floor
    Voice voices[maxVoices];
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SineSynthAudioProcessor)
};
