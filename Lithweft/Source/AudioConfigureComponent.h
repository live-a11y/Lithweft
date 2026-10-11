/*
  ==============================================================================

    AudioConfigureComponent.h
    Created: 22 Nov 2023 3:44:55pm
    Author:  刘昊

  ==============================================================================
*/


#pragma once

#include <JuceHeader.h>

//==============================================================================
class AudioConfigureComponent : public juce::Component,
public juce::ChangeListener
{
public:
    AudioConfigureComponent(juce::AudioDeviceManager& manager)
        : deviceManager(manager)
    {
        // Initialize AudioDeviceSelectorComponent
        audioSetupComponent.reset(new juce::AudioDeviceSelectorComponent(
            deviceManager,
            0, 256,     // min/max input channels
            0, 256,     // min/max output channels
            true, true, // show MIDI input/output options
            true, false // show audio input/output options
        ));
        addAndMakeVisible(audioSetupComponent.get());

        deviceManager.addChangeListener(this);
        setSize(500, 600);
    }

    ~AudioConfigureComponent() override
    {
        deviceManager.removeChangeListener(this);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(4);
        audioSetupComponent->setBounds(r);
    }

    void changeListenerCallback(juce::ChangeBroadcaster*) override
    {
        // Optional: Implement any actions when the audio settings change
        // For example, logging the new device info or updating other components
    }

private:
    juce::AudioDeviceManager& deviceManager;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSetupComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfigureComponent)
};

