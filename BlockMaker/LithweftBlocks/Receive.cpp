//
//  Receive.cpp
//  LithweftBlocks
//
//  Created by 刘昊 on 2024/2/15.
//

// Receive.h
#include "BlockInterface.h"

class Receive : public BlockInterface {
private:
    std::string channelName; // The channel this block is listening to
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

public:
    // Constructor may take additional parameters if necessary
    Receive() {}

    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        // Initialization logic here. Assume channelName is set from args
        if (std::holds_alternative<Message>(args)) {
            channelName = std::get<Message>(args);
        }
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;
    }

//    void setInput(int index, const BlockData& data) override {
//        // Handle incoming data
//        if(index == -1){
//            outletCallback(this,0,data);
//        }
//    }
    
    void setInput(int index, const BlockData& data) override {
        if(index == -1){
            // Check if the data is not an AudioSignal
            if (!std::holds_alternative<AudioSignal>(data)) {
                // Handle non-AudioSignal data
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (!std::is_same_v<T, AudioSignal>) {
                        // Process the data here
//                        std::cout << "Received non-AudioSignal data on special channel (-1)." << std::endl;
                        // Example action: Directly output the data or log it
                        // You might need to specifically handle each type or perform a generic action
                        
                        outletCallback(this,0,data);

                    }
                }, data);
            } else {
                std::cout << "AudioSignal data received is not handled." << std::endl;
            }
        } else {
            // Handle regular input channels
            // Regular input processing logic here
        }
    }
    
    

    size_t getNumberOfInputs() const override { return 0; }
    size_t getNumberOfOutputs() const override { return 1; } // Assuming it might output the received data

    BlockData  getInfo(const std::string& query = "") const override {

        if(query ==  "getReceiveChannelName"){
            
            return Message(channelName);
        }
        return Uninitialized{};

    }

  
};

extern "C" BlockInterface* createBlockInstance() {
    Receive* block = new Receive();
    return block;
}

