#include <stdio.h>
#include "BlockInterface.h"
#include <variant>
#include <vector>

class Multiple_tilde : public BlockInterface {
public:
    Multiple_tilde() {

    }
    

    virtual ~Multiple_tilde() {}


    
    
    // Set an input at a specific index
    void setInput(int index, const BlockData& data) override {
        
        if (index < NUM_INPUTS) {
            if(index == 1){
                
                if(std::holds_alternative<Number>(data)){
                    multiple_factor_number = std::get<Number>(data);
                    std::fill(multiple_factor.begin(), multiple_factor.end(), multiple_factor_number);
                }
                
                if(std::holds_alternative<AudioSignal>(data)){
                    multiple_factor = std::get<AudioSignal>(data);
                }
            }
            
            if(index == 0 && std::holds_alternative<AudioSignal>(data)){
                input_0 = std::get<AudioSignal>(data) ;
            }
            
        }
    }

    // New methods to get the count of inputs and outputs
    size_t getNumberOfInputs() const override {
        return NUM_INPUTS;
    }

    size_t getNumberOfOutputs() const override {
        return NUM_OUTPUTS;
    }



    // Initialize the block with dynamic arguments
    
    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override{
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;

        // Default value for inputs[1]
        multiple_factor_number = Number(0);

        // Check the type of 'args' and process accordingly
        if (std::holds_alternative<Number>(args)) {
            // If args is a Number, use it directly for inputs[1]
            multiple_factor_number = std::get<Number>(args);
        }
        else if (std::holds_alternative<List>(args)) {
            // If args is a List, extract the first element as a Number
            const List& argsList = std::get<List>(args);
            if (!argsList.empty() && std::holds_alternative<Number>(argsList[0])) {
                multiple_factor_number = std::get<Number>(argsList[0]);
            }
        }
        // If args is empty or of any other type, inputs[1] remains its default value (0)
    }
    
    
    virtual void configureAudioParameters(int numSamples, double sampleRate) override{
        this->numSamples = numSamples;
        this->sampleRate =  sampleRate;
        input_0.resize(numSamples,0.0); // Default size and initial value for the audio buffer
        multiple_factor.resize(numSamples,multiple_factor_number);
        output_0.resize(numSamples,0.0);
        
    }

    void processAudio() override {

        for (int i = 0; i < numSamples; ++i) {
            output_0[i] = input_0[i] * multiple_factor[i];
        }
    }
    
    AudioSignal getAudioOutput(size_t outletIndex) const override{
        return output_0;
    }

    bool handlesAudioProcessing() const override {
        return true;
    }

private:
    static constexpr size_t NUM_INPUTS = 2;
    static constexpr size_t NUM_OUTPUTS = 1;

    int numSamples;
    double sampleRate;
    
    AudioSignal input_0;      // First input (audio signal)
    AudioSignal multiple_factor;        // Second input (number or audio signal)
    Number multiple_factor_number;        // Second input (number or audio signal)
    AudioSignal output_0;     // Output audio signal

    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;
    
    
};

extern "C" BlockInterface* createBlockInstance() {
    Multiple_tilde* block = new Multiple_tilde();
    std::cout << "Multiple_tilde block created: " << block << std::endl;
//    block->init(args, messageCallback, outletCallback);
    return block;
}

