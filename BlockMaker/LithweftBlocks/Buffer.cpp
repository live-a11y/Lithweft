//
//  Buffer_Tilde.cpp
//  LithweftBlocks
//
//  Created by 刘昊 on 2024/2/20.
//

#include <stdio.h>
#include "BlockInterface.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath> // For converting to floating-point
class Buffer : public BlockInterface {
private:
    std::string bufferName;
    AudioSignal localWavetable; // Local storage for wavetable data
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

public:
    Buffer() {}

    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;

        // Initialize bufferName from args if provided
        // Check if args is a Message
           if (std::holds_alternative<Message>(args)) {
               bufferName = std::get<Message>(args);
           }
           // Check if args is a List and take the first element as buffer name if it's a Message
           else if (std::holds_alternative<List>(args)) {
               const List& argsList = std::get<List>(args);
               if (!argsList.empty() && std::holds_alternative<Message>(argsList[0])) {
                   bufferName = std::get<Message>(argsList[0]);
               }
           }
        
        // Audio is loaded explicitly by a `read <path>` message, not from a
        // file on the original developer's desktop.
    }

    void setInput(int index, const BlockData& data) override {
        // Handle list input with "read" command and file path
        if (index == 0 && std::holds_alternative<List>(data)) {
            const List& listData = std::get<List>(data);
            if (listData.size() >= 2 && std::holds_alternative<Message>(listData[0])) {
                std::string command = std::get<Message>(listData[0]);
                if (command == "read" && std::holds_alternative<Message>(listData[1])) {
                    std::string filePath = std::get<Message>(listData[1]);
                    std::cout<<"buffer read input"<<filePath<<std::endl;
                    loadWaveFile(filePath); // Load wave file into localWavetable
                    
                    
                    // Notify the system that the buffer has been updated
                    systemMessageCallback(this, "bufferUpdated", Message(bufferName));

                }
            }
        }
    }

    size_t getNumberOfInputs() const override { return 1; }
    size_t getNumberOfOutputs() const override { return 1; } // Assuming it might output the wavetable or a signal

    BlockData  getInfo(const std::string& query = "") const override {
        // Create a list that includes a message indicating a shared buffer, the buffer name, and the local wavetable

        if (query == "getBufferName") {
              return Message(bufferName);
        }else if(query == "getSharedBuffer"){
            return localWavetable;
        }
        
              // Handle other queries or fallback to the default implementation
        return Uninitialized{};
    }


private:
    struct WAVHeader {
        char riff_header[4]; // Contains "RIFF"
        int wav_size;
        char wave_header[4]; // Contains "WAVE"
        char fmt_header[4];  // Contains "fmt "
        int fmt_chunk_size;
        short audio_format;  // Should be 1 for PCM
        short num_channels;
        int sample_rate;
        int byte_rate;
        short sample_alignment;
        short bit_depth; // Typically 16 for PCM
        char data_header[4];
        int data_bytes;
    };
    
//    void loadWaveFile(const std::string& filePath) {
//        // Implement wave file loading logic here
//        // The loaded data should be stored in localWavetable
//
//        // Sine wave parameters
//          const size_t length = 512; // 1 second of audio
//
//        // Prepare the wavetable
//        localWavetable.clear();
//        localWavetable.reserve(length);
//
//        // Generate the sine wave
//        for (size_t i = 0; i < length; ++i) {
//            double phase = i/(double)length * 2.0 * M_PI;
//            localWavetable.push_back(std::sin(phase));
//        }
//    }
    
    void loadWaveFile(const std::string& rawFilePath) {
        // Trim quotes from the file path if present
        std::string filePath = rawFilePath;
        if (filePath.size() >= 2 && filePath.front() == '\"' && filePath.back() == '\"') {
            filePath = filePath.substr(1, filePath.size() - 2);
        }

        std::ifstream file(filePath, std::ios::binary);
        WAVHeader header;

        if (!file.is_open()) {
            systemMessageCallback(this, "print", "Could not open wave file: " + filePath);
            return; // Exit if file not opened
        }

        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

        // Validate WAV format
        if (std::strncmp(header.riff_header, "RIFF", 4) != 0 ||
            std::strncmp(header.wave_header, "WAVE", 4) != 0 ||
            header.audio_format != 1) {
            systemMessageCallback(this, "print", "Invalid or unsupported WAV file format.");
            return; // Exit if format not supported
        }

        int numSamplesToRead = header.data_bytes / (header.bit_depth / 8);
        std::vector<short> samples(numSamplesToRead);

        file.read(reinterpret_cast<char*>(samples.data()), header.data_bytes);

        // Convert samples to float and normalize
        localWavetable.resize(numSamplesToRead);
        for (int i = 0; i < numSamplesToRead; ++i) {
            localWavetable[i] = samples[i] / static_cast<double>(INT16_MAX);
        }

        // Success message
        systemMessageCallback(this, "print", "Successfully loaded wave file: " + filePath + ", with " + std::to_string(numSamplesToRead) + " samples.");
    }

    
};
extern "C" BlockInterface* createBlockInstance() {
    Buffer* block = new Buffer();
    return block;
}

