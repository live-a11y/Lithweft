//
//  Divide.cpp
//  LithweftBlocks
//
//  Created by 刘昊 on 2023/11/14.
//


#include <stdio.h>
#include "BlockInterface.h"

#define NUM_INPUTS 2
#define NUM_OUTPUTS 1

class Divide : public BlockInterface
{
    
private:
    std::vector<BlockData> inputs;
    std::vector<BlockData> outputs;
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;
  
public:
    Divide(){
        inputs.resize(NUM_INPUTS);
        outputs.resize(NUM_OUTPUTS);
        
    }
    ~Divide(){
        
    }

    // Set an input at a specific index
    void setInput(int index, const BlockData& data) override{
        
        if(index < inputs.size()){
            
            if (std::holds_alternative<Number>(data))  {
                inputs[index] = data;
                
                if(index == 0){
                    process();
                    outletCallback(this,0,outputs[0]);
        
                }
            }

            if(std::holds_alternative<Message>(data)){
                if(index == 0 && std::get<Message>(data) == "Trigger"){
                    process();
                    outletCallback(this,0,outputs[0]);
                }
            }
        }
    }


    // New methods to get the count of inputs and outputs
    size_t getNumberOfInputs() const override{
        return  NUM_INPUTS;
        
    }
    virtual size_t getNumberOfOutputs() const override{
        
        return NUM_OUTPUTS;
    }


    
    
    // Initialize the block with dynamic arguments
    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override{
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;

        // Default value for inputs[1]
        inputs[1] = Number(0);

        // Check the type of 'args' and process accordingly
        if (std::holds_alternative<Number>(args)) {
            // If args is a Number, use it directly for inputs[1]
            inputs[1] = std::get<Number>(args);
        }
        else if (std::holds_alternative<List>(args)) {
            // If args is a List, extract the first element as a Number
            const List& argsList = std::get<List>(args);
            if (!argsList.empty() && std::holds_alternative<Number>(argsList[0])) {
                inputs[1] = std::get<Number>(argsList[0]);
            }
        }
        // If args is empty or of any other type, inputs[1] remains its default value (0)
    }
//    bool handlesAudioProcessing() const override{
//        return  false;
//    }
    
//    void processAudio(AudioSignal& audioSignal, int numSamples, double sampleRate) override{}

//    void prepareToProcess(double sampleRate,int numSamples) override{}
    
private:

    // Process the block's inputs to produce outputs
    void process() {
        // Check that both inputs are of type Float
        if (std::holds_alternative<Number>(inputs[0]) && std::holds_alternative<Number>(inputs[1])) {
            
            Number input_1 = std::get<Number>(inputs[0]);
            Number input_2 = std::get<Number>(inputs[1]);
            outputs[0] = input_1 / input_2;
            
        }else {
            systemMessageCallback(this,"print",Message("Invalid or insufficient inputs for processing."));
        }
    }
    
};
extern "C" BlockInterface* createBlockInstance() {
    Divide* block = new Divide();
    std::cout<<"Add loaded:"<<block<<std::endl;
//    block->init(args, messageCallback, outletCallback);
    return block;
}

