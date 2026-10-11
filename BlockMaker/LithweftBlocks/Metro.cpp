//
//  Metro.cpp
//  LithweftBlocks
//
//  Created by 刘昊 on 2023/12/19.
//
#define NUM_INPUTS 2
#define NUM_OUTPUTS 1

#include <stdio.h>
#include "BlockInterface.h"

class Metro : public BlockInterface {
private:
    bool isRunning = false;
    int intervalMs = 1; // Default interval in milliseconds
    int counter = 0;
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

public:
    Metro() {
        // Default initialization
    }

    void setInput(int index, const BlockData& data) override {
        if (index == 0 && std::holds_alternative<Number>(data)) {
            // Start/Stop control
            isRunning = (std::get<Number>(data) != 0);
        } else if (index == 1 && std::holds_alternative<Number>(data)) {
            // Speed control
            intervalMs = static_cast<int>(std::get<Number>(data));
            intervalMs = std::max(1, intervalMs); // Enforce minimum interval of 1 ms
        }
    }

    void timerTick() override {
        if (isRunning) {
            counter++;
            if (counter >= intervalMs) {
                counter = 0;
                // Trigger the outlet with a "Trigger" message
                if (outletCallback) {
                    outletCallback(this, 0, Message("Trigger"));
                }
            }
        }
    }

    // Other necessary methods...
    
    void init(const BlockData& args, SystemMessageCallback SystemMessageCallback, OutletCallback outletCallback) override {
        this->systemMessageCallback = SystemMessageCallback;
        this->outletCallback = outletCallback;

        // Check if args is a single Number
        if (std::holds_alternative<Number>(args)) {
            Number numberArg = std::get<Number>(args);
            intervalMs = static_cast<int>(numberArg);
            intervalMs = std::max(1, intervalMs); // Enforce minimum interval of 1 ms
        }
        // Check if args is a List
        else if (std::holds_alternative<List>(args)) {
            const List& argList = std::get<List>(args);
            if (!argList.empty() && std::holds_alternative<Number>(argList[0])) {
                Number numberArg = std::get<Number>(argList[0]);
                intervalMs = static_cast<int>(numberArg);
                intervalMs = std::max(1, intervalMs); // Enforce minimum interval of 1 ms
            } else {
                systemMessageCallback(this,"print", Message("Invalid or missing interval in List."));
            }
        } else {
            // Handle other cases or error
            systemMessageCallback(this,"print", Message("Unsupported argument type or missing interval."));
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
    BlockData  getInfo(const std::string& query = "") const override {
        std::ostringstream infoStream;
        infoStream << "Block: Metro\n"
                   << "Version: 1.0\n"
                   << "Developer: Liu Hao\n"
                   << "Date Created: 2023-12-19\n"
                   << "Date Updated: 2023-12-19\n";
        return Message(infoStream.str());
    }
    
};

extern "C" BlockInterface* createBlockInstance() {
    Metro* block = new Metro();
    std::cout<<"Metro loaded:"<<block<<std::endl;
//    block->init(args, SystemMessageCallback, outletCallback);
    return block;
}

