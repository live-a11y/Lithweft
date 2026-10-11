/*
  ==============================================================================

    Scope.h
    Created: 18 Nov 2023 10:02:37pm
    Author:  刘昊

  ==============================================================================
*/


#pragma once
#include "BlockInterface.h"
#include <JuceHeader.h>
#include "BlockComponent.h"

#define SCOPE_NUM_INPUTS  1
#define SCOPE_NUM_OUTPUTS  0

class Scope : public BlockComponent, public BlockInterface ,public juce::Timer{
public:
    Scope() {
        
        startTimer(50);
    }

    ~Scope() {}

    // Initialize the block with dynamic arguments
    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;
        // Additional initialization if necessary
    }

    void processAudio() override {
          // Assuming audioSignal is a member variable of type AudioSignal
          updateScope(audioSignal); // Update the scope with new audio data
          // Additional audio processing logic if necessary...
      }

      void setInput(int index, const BlockData& data) override {
          if (index == 0 && std::holds_alternative<AudioSignal>(data)) {
              audioSignal = std::get<AudioSignal>(data); // Store the incoming audio signal
          }
      }

    bool handlesAudioProcessing() const override {
        return true; // Indicates that this block handles audio processing
    }
    
    size_t getNumberOfInputs() const override {
        return SCOPE_NUM_INPUTS;
    }

    size_t getNumberOfOutputs() const override {
        return SCOPE_NUM_OUTPUTS;
    }



    void setup(size_t num_inlet, size_t num_outlet, juce::Point<int> pos, bool isUIBlock) override {
        BlockComponent::setup(SCOPE_NUM_INPUTS, SCOPE_NUM_OUTPUTS, pos, isUIBlock);
        // Configure the Scope component
        // Example: setBounds, addAndMakeVisible, etc.
        setBounds(pos.x - 120/2, pos.y- 120/2, 120, 120);
        
    }


       void timerCallback() override {
           repaint();  // Called regularly at the specified interval
       }

    // GUI Component overrides
    void paint(juce::Graphics& g) override {
        BlockComponent::paint(g);
        drawScope(g);  // Draw the scope waveform
    }

    void resized() override {
        // Handle component resizing if necessary
    }
    void configureAudioParameters(int numSamples, double sampleRate) override {
//        this->sampleRate = sampleRate;
        this->bufferSize = numSamples; // Update bufferSize to match numSamples

        // Resize the audioBuffer to accommodate the new number of samples
        audioBuffer.resize(bufferSize, 0.0f);

        // Reset bufferWriteIndex to avoid out-of-bounds access
        bufferWriteIndex = 0;
    }

protected:
    void updateComponentState() override {
        // Update component state based on the current mode
    }

private:
    
    // Method to update the scope with new audio data
    void updateScope(const AudioSignal& audioSignal) {
        // Ensure the buffer is initialized
        if (audioBuffer.size() != bufferSize) {
            audioBuffer.resize(bufferSize, 0.0f);
        }

        // Copy the incoming audio signal into the buffer
        for (auto sample : audioSignal) {
            audioBuffer[bufferWriteIndex++] = sample;
            if (bufferWriteIndex >= bufferSize) {
                bufferWriteIndex = 0;
            }
        }

        // Additional processing if needed
    }



    void  drawScope(juce::Graphics& g) {
        drawScopeBackgroundWithGrid(g);  // Draw background with grid
        
        const int width = getWidth();
        const int height = getHeight();

        // Set the color for the waveform
        g.setColour(juce::Colours::white);

        if (!audioSignal.empty()) {
            const float xScale = static_cast<float>(width) / static_cast<float>(bufferSize);

            for (size_t i = 0; i < bufferSize - 1; ++i) {
                // Scale and translate the audio sample to fit within the component bounds
                float y1 = (1.0f - audioBuffer[i]) * height / 2.0f;
                float y2 = (1.0f - audioBuffer[i + 1]) * height / 2.0f;

                // Calculate x positions
                float x1 = static_cast<float>(i) * xScale;
                float x2 = static_cast<float>(i + 1) * xScale;

                // Draw a line between the current and next sample
                g.drawLine(x1, y1, x2, y2);
            }
        }
    }


    void drawScopeBackgroundWithGrid(juce::Graphics& g) {
        int width = getWidth();
        int height = getHeight();

        // Set background color
        g.fillAll(juce::Colours::grey); // or any color you prefer

        // Set grid color
        g.setColour(juce::Colours::lightblue);

        // Draw vertical grid lines
        int numVerticalLines = 10; // Adjust as needed
        for (int i = 1; i < numVerticalLines; ++i) {
            float x = static_cast<float>(i) / static_cast<float>(numVerticalLines) * width;
            g.drawLine(x, 0, x, height);
        }

        // Draw horizontal grid lines
        int numHorizontalLines = 10; // Adjust as needed
        for (int i = 1; i < numHorizontalLines; ++i) {
            float y = static_cast<float>(i) / static_cast<float>(numHorizontalLines) * height;
            g.drawLine(0, y, width, y);
        }
    }

    std::vector<BlockData> inputs;
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

    std::vector<float> audioBuffer;
    size_t bufferWriteIndex = 0;
    size_t bufferSize = 128; // Adjust as needed
    
    AudioSignal audioSignal; // Store the incoming audio signal

    // Additional private members (e.g., buffer for audio data, visualization parameters)
};

//extern "C" BlockInterface* createBlockInstance(const InitializationArguments& args, MessageCallback messageCallback, OutletCallback outletCallback) {
//    auto* block = new Scope(args, messageCallback, outletCallback);
//    return block;
//}
