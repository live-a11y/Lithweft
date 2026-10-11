#include "BlockInterface.h"
#include <iostream>
#include <sstream>
#include <functional>
#include <map>
#include <variant>
#include <vector>

#define NUM_INPUTS 1
#define NUM_OUTPUTS 0

//// Define the variant to include all the types you want in the list
//using ListElement = std::variant<Number, Message>;
//// Define the List type as a vector of ListElement variants
//using List = std::vector<ListElement>;

// Visitor for converting a ListElement to a string
struct ListElementVisitor {
    std::string operator()(Number numberValue) const {
        return std::to_string(numberValue);
    }

    std::string operator()(const Message& messageValue) const {
        return messageValue;
    }
    
    // Add cases for other types if necessary
};

//struct ListElementVisitor {
//    std::string operator()(Number numberValue) const {
//        return std::to_string(numberValue);
//    }
//
//    std::string operator()(const Message& messageValue) const {
//        return messageValue;
//    }
//    
//    std::string operator()(const AudioSignal& audioSignalValue) const {
//        // Simplify the representation of an AudioSignal for printing
//        return "AudioSignal[" + std::to_string(audioSignalValue.size()) + " samples]";
//    }
//};


// Visitor for converting BlockData to a string
struct BlockDataToStringVisitor {
    std::string operator()(Uninitialized) const {
        return "Uninitialized";
    }
    
    std::string operator()(Number numberValue) const {
        std::ostringstream stream;
        stream.precision(6); // Set precision if you want to limit decimal places
        stream << numberValue;
        std::cout<<"T Number"<<std::endl;
        return stream.str();
    }

    std::string operator()(const Message& messageValue) const {
        std::cout<<"T Message"<<std::endl;
        return messageValue;
    }

    std::string operator()(const List& listValue) const {
        std::ostringstream oss;
        for (const auto& elem : listValue) {
            // Use std::visit with ListElementVisitor for each element
            oss << std::visit(ListElementVisitor(), elem) << ' ';
        }
        std::cout<<"T List"<<std::endl;
        return oss.str();
    }

    std::string operator()(const AudioSignal& audioSignalValue) const {
        std::ostringstream oss;
        for (const auto& sample : audioSignalValue) {
            oss << sample << ' ';
        }
        std::cout<<"T Audio"<<std::endl;
//        for(int i = 0 ; i < audioSignalValue.size();i+=32){
//                oss << audioSignalValue[i] << ' ';
//        }
        return oss.str();
    }
};

//struct BlockDataToStringVisitor {
//    std::string operator()(Uninitialized) const {
//        return "Uninitialized";
//    }
//    
//    std::string operator()(Number numberValue) const {
//        std::ostringstream stream;
//        stream.precision(6); // Set precision if you want to limit decimal places
//        stream << numberValue;
//        return stream.str();
//    }
//
//    std::string operator()(const Message& messageValue) const {
//        return messageValue;
//    }
//
//    std::string operator()(const List& listValue) const {
//        std::ostringstream oss;
//        for (const auto& elem : listValue) {
//            // Use std::visit with ListElementVisitor for each element
//            oss << std::visit(ListElementVisitor(), elem) << ' ';
//        }
//        return oss.str().substr(0, oss.str().size() - 1); // Remove the last space
//    }
//
//    std::string operator()(const AudioSignal& audioSignalValue) const {
//        // Handling AudioSignal directly, no need to change unless detailed print is desired
//        std::ostringstream oss;
//        oss << "AudioSignal[" << audioSignalValue.size() << " samples]";
//        return oss.str();
//    }
//};


class Print : public BlockInterface {
private:
    std::vector<BlockData> inputs;
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

public:
    Print() {
        inputs.resize(NUM_INPUTS);
    }

    void setInput(int index, const BlockData& data) override {
        if (index < inputs.size()) {
            inputs[index] = data;
            
            // Convert BlockData to string and send it to the message callback
            std::string dataAsString = std::visit(BlockDataToStringVisitor(), data);
            if (systemMessageCallback) {
                systemMessageCallback(this,"print",Message(dataAsString));
            }
        } else {
            if (systemMessageCallback) {
                systemMessageCallback( this,"print",Message("Input index out of range."));
            }
        }
    }
    
//    void setInput(size_t index, const BlockData& data) override {
//        if (index < inputs.size()) {
//            inputs[index] = data;
//            
//            // Convert BlockData to string and send it to the message callback
//            std::string dataAsString = std::visit(BlockDataToStringVisitor(), data);
//            if (systemMessageCallback) {
//                systemMessageCallback("print", Message(dataAsString));
//            }
//        } else {
//            if (systemMessageCallback) {
//                systemMessageCallback("print", Message("Input index out of range."));
//            }
//        }
//    }



    
//    void onTrigger() override {
//        if (messageCallback) {
//            messageCallback("Triggered!");
//        }
//    }

    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override{
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;
    }
    
    size_t getNumberOfInputs() const override {
        return NUM_INPUTS; // Your logic to determine the number of inputs
    }

    size_t getNumberOfOutputs() const override {
        return NUM_OUTPUTS; // Your logic to determine the number of outputs
    }

//    BlockData  getInfo(const std::string& query = "") const override {
//
//        std::ostringstream infoStream;
//        infoStream << "Block: print\n"
//                   << "Version: 1.0\n"
//                   << "Developer: Liu Hao\n"
//                   << "Date Created: 2023-11-05\n"
//                   << "Date Updated: 2023-11-05\n";
//        return Message(infoStream.str());
//    }
    
    
//    void processAudio(AudioSignal& audioSignal, int numSamples, double sampleRate) override{
//
//        
//    }
//    bool handlesAudioProcessing() const override{
//        return  false;
//    }
//    void prepareToProcess(double sampleRate,int numSamples) override{}
};

extern "C" BlockInterface* createBlockInstance() {
    auto* block = new Print();
//    block->init(args, messageCallback, outletCallback);
    return block;
}

