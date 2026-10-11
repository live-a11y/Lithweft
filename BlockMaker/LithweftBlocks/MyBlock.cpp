
#include "BlockInterface.h"
#include <variant>


#define NUM_INPUTS 2
#define NUM_OUTPUTS 2


class MyBlock : public BlockInterface {
private:
    std::vector<BlockData> inputs;
    std::vector<BlockData> outputs;
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

    Number arg_0_factor = 0; // Default factor

public:
    MyBlock() {
        inputs.resize(NUM_INPUTS);
        outputs.resize(NUM_OUTPUTS);
    }
    ~MyBlock()
    {
//        std::cout<<"My Block deconstructed "<<std::endl;
    }

    void setInput(int index, const BlockData& data) override {
        if (index < inputs.size()) {
            
            //check the input is wether or not Integer
//            if (std::holds_alternative<Number>(data))  {
            if (std::holds_alternative<Number>(data))  {
                inputs[index] = data;

                if (index == 0) {
                    // Automatically call the process method to update outputs
                    process();
                    triggerOutput();
                }
            }
            
            
            if(index == 0){

                if(std::holds_alternative<Message>(data)){
                        Message msg = std::get<Message>(data);
                        if(msg == "Trigger"){
                            process();
                            triggerOutput();
                        }
                        if(msg == "getInfo"){
                            systemMessageCallback(this,"print",getInfo());
                        }
                    }
            }

//                List list = std::get<List>(data);
            if (std::holds_alternative<List>(data)) {
                const List& list = std::get<List>(data); // Use a reference to avoid copying
                if (list.size() == 2) {
                    
                    if (std::holds_alternative<Message>(list[0])) {
                        std::cout << "The first element of the list is Message: " << std::get<Message>(list[0]) << std::endl;
                        if (std::get<Message>(list[0]) == "factor"){
                            if (std::holds_alternative<Number>(list[1])) {
                                std::cout << "The second element of the list is Number: " << std::get<Number>(list[1]) << std::endl; //
                                Number factor = std::get<Number>(list[1]);
                                inputs[1] = factor;
                                
                            }
                        }
                        
                    }

                }
            }

            
            
            
        } else {
            systemMessageCallback(this,"print",Message("Input index out of range."));
        }

    }


   
    // Implement the new methods
    size_t getNumberOfInputs()  const override {
//           std::cout<<"getNumberOfInputs():"<<NUM_INPUTS<<std::endl;
           return NUM_INPUTS; // Your logic to determine the number of inputs
       }

    size_t getNumberOfOutputs() const override {
//           std::cout<<"getNumberOfOutputs()"<<NUM_OUTPUTS<<std::endl;
           return NUM_OUTPUTS; // Your logic to determine the number of outputs
       }

//    BlockData  getInfo(const std::string& query = "") const override {
//           std::ostringstream infoStream;
//           infoStream << "Block: myBlock\n"
//                      << "Version: 1.0\n"
//                      << "Developer: Liu Hao\n"
//                      << "Date Created: 2023-11-05\n"
//                      << "Date Updated: 2023-11-05\n";
//           return Message(infoStream.str());
//       }
    
    
    void init(const BlockData& args, SystemMessageCallback SystemMessageCallback, OutletCallback outletCallback) override{
        this->systemMessageCallback = SystemMessageCallback;
        this->outletCallback = outletCallback;

        
        if(std::holds_alternative<Number>(args)){
            // Process Number argument directly
                   Number numberArg = std::get<Number>(args);
                   // Handle the number argument (e.g., set as input or factor)
                   inputs[1] = numberArg; // Assuming the number is meant for input[1]
        } // Check if args is a List
        else if (std::holds_alternative<List>(args)) {
            const List& argList = std::get<List>(args);
            // Expecting the List to contain "factor" followed by a Number
            if (argList.size() == 2 && std::holds_alternative<Message>(argList[0]) && std::get<Message>(argList[0]) == "factor" && std::holds_alternative<Number>(argList[1])) {
                Number factor = std::get<Number>(argList[1]);
                // Process the factor
                inputs[1] = factor; // Set the factor as the second input
            } else {
                // Handle error or unexpected format
                systemMessageCallback(this,"print", Message("Invalid argument format for List."));
            }
        } else {
            // Handle other cases or error
            systemMessageCallback(this,"print", Message("Unsupported argument type."));
        }
        
 
        
    }
    

private:
    
    void process()  {
        
        // Check that both inputs are of type Float
        if (std::holds_alternative<Number>(inputs[0]) && std::holds_alternative<Number>(inputs[1])) {

                Number inputFloat_1 = std::get<Number>(inputs[0]);
                Number inputFloat_2 = std::get<Number>(inputs[1]);

                outputs[0] = inputFloat_1 * inputFloat_2; // Multiply the inputs
                outputs[1] = inputFloat_1 + inputFloat_2; // Add the inputs

            
        } else {
            systemMessageCallback(this,"print",Message( "Invalid or insufficient inputs for processing."));
        }
    }
    
    void triggerOutput()
    {
        
        if (outletCallback) {
            outletCallback(this, 1, outputs[1]);
            outletCallback(this, 0, outputs[0]);

        }
    }
    
};

extern "C" BlockInterface* createBlockInstance() {
    MyBlock* block = new MyBlock();
    std::cout<<"myBlock loaded:"<<block<<std::endl;
//    block->init(args, SystemMessageCallback, outletCallback);
    return block;
}

