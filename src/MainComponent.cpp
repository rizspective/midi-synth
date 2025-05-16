#include "MainComponent.h"

MainComponent::MainComponent()
    : keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
      synthAudioSource(keyboardState)
{
    addAndMakeVisible(keyboardComponent);
    audioSourcePlayer.setSource(&synthAudioSource);

    setSize(600, 200);
    setAudioChannels(0, 2); // No inputs, stereo output
}

MainComponent::~MainComponent()
{
    audioSourcePlayer.setSource(nullptr);
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    synthAudioSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    synthAudioSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    keyboardComponent.setBounds(10, 10, getWidth() - 20, getHeight() - 20);
}