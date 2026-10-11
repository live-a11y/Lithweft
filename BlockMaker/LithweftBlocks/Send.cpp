//
//  Send.h
//  LithweftBlocks
//
//  Created by 刘昊 on 2024/2/11.
//
#include "BlockInterface.h"
class Send : public BlockInterface {
public:
    Send() = default;
    virtual ~Send() = default;

    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        // Check if args is of type Message
        if (auto val = std::get_if<Message>(&args)) {
            channelName = *val;
            std::cout << "Initialized with channel name: " << channelName << std::endl;
        } else {
            // Args is not a Message type, output debug information
            std::cout << "Initialization error: args is not a Message type." << std::endl;

            // Additional debug info based on the actual type of args
            if (std::holds_alternative<Number>(args)) {
                std::cout << "Args is a Number: " << std::get<Number>(args) << std::endl;
            } else if (std::holds_alternative<List>(args)) {
                std::cout << "Args is a List with size: " << std::get<List>(args).size() << std::endl;
            } else if (std::holds_alternative<AudioSignal>(args)) {
                std::cout << "Args is an AudioSignal with sample count: " << std::get<AudioSignal>(args).size() << std::endl;
            } else if (std::holds_alternative<Uninitialized>(args)) {
                std::cout << "Args is Uninitialized." << std::endl;
            }
        }

        this->systemMessageCallback = systemMessageCallback;
        // Additional initialization logic as needed
    }


    // Handle incoming data to send
//    void setInput(size_t index, const BlockData& data) override {
//        if (index == 0) { // Assume first inlet is for data
//            // Use the systemMessageCallback to send the data with the channelName as the identifier
//            std::string prefixedChannelName = "send" + channelName;
//
//            if (systemMessageCallback) {
//                systemMessageCallback(channelName, data);
//            }
//        }
//    }
    
    // Handle incoming data to send
    void setInput(int index, const BlockData& data) override {
        if (index == 0) { // Assuming the first inlet is for the data to send
            // Concatenate "send" with the channel name to form the operation name
            std::string operationName = "send";

            // Use the systemMessageCallback to send the data
            if (systemMessageCallback) {
                systemMessageCallback(this,operationName, data);
            }
        }
    }


    BlockData  getInfo(const std::string& query = "") const override {
        // Create a list that includes a message indicating a shared buffer, the buffer name, and the local wavetable

        if (query == "getSendChannelName") {
              return Message(channelName);
        }
        
              // Handle other queries or fallback to the default implementation
        return Uninitialized{};
    }


    size_t getNumberOfInputs() const override { return 1; }
    size_t getNumberOfOutputs() const override { return 0; }
    
    // Other methods...

private:
    std::string channelName;
    SystemMessageCallback systemMessageCallback;
};

extern "C" BlockInterface* createBlockInstance() {
    Send* block = new Send();
    return block;
}


