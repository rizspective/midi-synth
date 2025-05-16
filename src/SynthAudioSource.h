#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState);

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

private:
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;

    // Custom synthesiser sound class
    struct SineWaveSound : public juce::SynthesiserSound
    {
        SineWaveSound() = default;

        bool appliesToNote(int) override { return true; }
        bool appliesToChannel(int) override { return true; }
    };

    // Custom voice class
    struct SineWaveVoice : public juce::SynthesiserVoice
    {
        bool canPlaySound(juce::SynthesiserSound* sound) override { return dynamic_cast<SineWaveSound*>(sound) != nullptr; }

        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void pitchWheelMoved(int) override {}
        void controllerMoved(int, int) override {}
        void renderNextBlock(juce::AudioBuffer<float>&, int startSample, int numSamples) override;

    private:
        double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
        bool tailOff = false;
    };
};