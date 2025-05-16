#include "SynthAudioSource.h"

SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState)
    : keyboardState(keyState)
{
    // Add some voices
    for (auto i = 0; i < 4; ++i)
        synth.addVoice(new SineWaveVoice());

    // Add the sound
    synth.addSound(new SineWaveSound());
}

void SynthAudioSource::prepareToPlay(int, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthAudioSource::releaseResources() {}

void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    juce::MidiBuffer incomingMidi;
    keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample,
                                      bufferToFill.numSamples, true);

    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi,
                         bufferToFill.startSample, bufferToFill.numSamples);
}

// Voice implementation
void SynthAudioSource::SineWaveVoice::startNote(int midiNoteNumber, float velocity,
                                              juce::SynthesiserSound*, int)
{
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = false;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();
    angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
}

void SynthAudioSource::SineWaveVoice::stopNote(float, bool allowTailOff)
{
    tailOff = allowTailOff;
}

void SynthAudioSource::SineWaveVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                                    int startSample, int numSamples)
{
    while (--numSamples >= 0)
    {
        auto currentSample = (float)(std::sin(currentAngle) * level);

        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample(i, startSample, currentSample);

        currentAngle += angleDelta;
        ++startSample;

        if (tailOff)
        {
            level *= 0.99;

            if (level <= 0.005)
            {
                clearCurrentNote();
                break;
            }
        }
    }
}