/*
  ==============================================================================

    BufferManager_Tilde.h
    Created: 17 Feb 2024 11:07:17pm
    Author:  刘昊

  ==============================================================================
*/

#pragma once
#include "BlockInterface.h"
#include <map>
#include <string>
#include <JuceHeader.h>

// Assuming this shared storage is declared globally or in a suitable location

class BufferManager_Tilde : public BlockComponent, public BlockInterface  {
private:
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

public:
    BufferManager_Tilde() {}

    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;
        // Additional initialization if needed
    }

    void setInput(int index, const BlockData& data) override {
        if (index == 0 && std::holds_alternative<List>(data)) {
            auto commandList = std::get<List>(data);
            if (commandList.size() >= 3 && std::holds_alternative<Message>(commandList[0])) {
                auto command = std::get<Message>(commandList[0]);
                auto filePath = std::get<Message>(commandList[1]);
                auto bufferName = std::get<Message>(commandList[2]);

                if (command == "read") {
                    loadWaveFile(filePath, bufferName);
                } else if (command == "add") {
                    addWaveFile(filePath, bufferName);
                }
            }
        }
    }

    size_t getNumberOfInputs() const override { return 1; }
    size_t getNumberOfOutputs() const override { return 1; } // Output could be used for signaling success/failure



    void setup(size_t num_inlet, size_t num_outlet, juce::Point<int> pos, bool isUIBlock) override {
        BlockComponent::setup(num_inlet, num_outlet, pos, isUIBlock);
        std::cout<<" --- posx "<<pos.x<<" posy"<<pos.y<<std::endl;

//        setBounds(pos.x, pos.y, getWidth(), getHeight()); // Fixed component size
        
    }



//    void paint(juce::Graphics& g) override {
//        BlockComponent::paint(g);
//    }
    
private:
     std::map<std::string, AudioSignal> sharedWavetables;

    void loadWaveFile(const std::string& filePath, const std::string& bufferName) {
        // Implementation to load a wave file and store it in 'sharedWavetables'
        AudioSignal waveData = loadWaveFromFile(filePath); // Assume this function loads the wave file
        sharedWavetables[bufferName] = waveData;
    }

    void addWaveFile(const std::string& filePath, const std::string& bufferName) {
        // Similar to 'loadWaveFile', but might involve additional logic
        AudioSignal waveData = loadWaveFromFile(filePath);
        if (sharedWavetables.find(bufferName) == sharedWavetables.end()) {
            sharedWavetables[bufferName] = waveData;
        } else {
            // Logic to handle adding to an existing buffer, if applicable
        }
    }

    // Placeholder for actual wave file loading function
    AudioSignal loadWaveFromFile(const std::string& filePath) {
        AudioSignal signal;
        // Load the file into 'signal'
        return signal;
    }
};

