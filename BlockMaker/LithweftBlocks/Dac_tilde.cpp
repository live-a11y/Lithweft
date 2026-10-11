// Dac.h
#include "BlockInterface.h"
#include <vector>
#include <algorithm>

class Dac_tilde : public BlockInterface {
public:

    
    virtual void configureAudioParameters(int numSamples, double sampleRate) override{
        this->numSamples = numSamples;
        this->sampleRate =  sampleRate;
        inputs.resize(numChannels); // Resize inputs to match the number of channels
        outputs.resize(numChannels);
        
        for (AudioSignal input : inputs){
            input.resize(numSamples);
            
        }
        
        for (AudioSignal output : outputs){
            output.resize(numSamples);
        }
        
        
    }

//    void init(const BlockData &args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override{
//        this->systemMessageCallback = systemMessageCallback;
//        this->outletCallback = outletCallback;
//        
//        this->numChannels = args.empty() ? 2 : std::stoi(args[0]);
//        
//    }
    
    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback)override {
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;

        // Default value for numChannels
        this->numChannels = 2;

        // Process 'args' based on its type
        if (std::holds_alternative<Number>(args)) {
            // If args is a Number, use it directly
            this->numChannels = static_cast<int>(std::get<Number>(args));
        }
        else if (std::holds_alternative<List>(args)) {
            // If args is a List, extract the first element as a Number
            const List& argsList = std::get<List>(args);
            if (!argsList.empty() && std::holds_alternative<Number>(argsList[0])) {
                this->numChannels = static_cast<int>(std::get<Number>(argsList[0]));
            }
        }
        // If args is empty or of any other type, numChannels remains its default value (2)
    }



    
    void setInput(int index, const BlockData& data) override {
          if (index < inputs.size()) {
              if(std::holds_alternative<AudioSignal>(data)){
                  inputs[index] = std::get<AudioSignal>(data);
              }
          }
      }
    
    size_t getNumberOfInputs() const override {
        return numChannels;
    }

    size_t getNumberOfOutputs() const override {
        return 0; // DAC has no outputs, it's an endpoint
    }

    bool handlesAudioProcessing() const override {
        return true;
    }

    bool handlesAudioOutput() const override {
        return true;
    }

    
    void processAudio() override {
        // Prepare the output buffers
//        prepareOutputBuffers(numSamples);

        // Process the audio signals
        for (size_t i = 0; i < inputs.size(); ++i) {
//            if (std::holds_alternative<AudioSignal>(inputs[i])) {
                outputs[i] = inputs[i]; // Directly copy to outputs
//            }
        }

        // Clear inputs after copying to outputs
        for (auto& input : inputs) {
                input.clear();
        }
    }


    AudioSignal getAudioOutput(size_t outletIndex) const override {
        if (outletIndex < outputs.size()) {
            const auto& output = outputs[outletIndex];
            // Check if the output variant holds an AudioSignal and is not empty
            if (!(output.empty())) {
                // Return the AudioSignal
                return (output);
            }
        }
        return AudioSignal(); // Return empty signal if outletIndex is out of range or output is empty
    }
    
//    AudioSignal getAudioOutput(size_t outletIndex) const override {
//        if (outletIndex < inputs.size()) {
//            const auto& output = inputs[outletIndex];
//            // Check if the output variant holds an AudioSignal and is not empty
//            if (!(output.empty())) {
//                // Return the AudioSignal
//                return (output);
//            }
//        }
//        return AudioSignal(); // Return empty signal if outletIndex is out of range or output is empty
//    }


private:
    int numSamples;
    double sampleRate;
    int numChannels;
    std::vector<AudioSignal> inputs;
    std::vector<AudioSignal> outputs; // Add outputs to store the audio signals

    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;
    
//    void prepareOutputBuffers(int numSamples) {
//        for (auto& output : outputs) {
////            if (std::holds_alternative<AudioSignal>(output)) {
//                output.resize(numSamples);
////            }
//        }
//    }
    
    
};

extern "C" BlockInterface* createBlockInstance() {
//    int numChannels = args.empty() ? 2 : std::stoi(args[0]);
    Dac_tilde* dacBlock = new Dac_tilde();
//    dacBlock->init(args, messageCallback, outletCallback);
    return dacBlock;
}

