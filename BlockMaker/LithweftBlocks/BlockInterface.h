#pragma once
#include <variant>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <map>

//--------------------
// Use std::monostate for an uninitialized state
using Uninitialized = std::monostate;
// Use double to cover both int and float values
using Number = double;
using Message = std::string;
using AudioSignal = std::vector<double>;

using ListElement = std::variant<Number, Message>;
using List = std::vector<ListElement>;
//using List = std::vector<std::variant<Number, Message>>;
// BlockData can hold an uninitialized state, a number, a string message, a list of strings, or an audio signal

using BlockData = std::variant<Uninitialized, Number, Message,List, AudioSignal>;




//
//class BlockDataUtils {
//public:
//    // Checkers for BlockData types
//    static bool isNumber(const BlockData& data) {
//        return std::holds_alternative<Number>(data);
//    }
//
//    static bool isMessage(const BlockData& data) {
//        return std::holds_alternative<Message>(data);
//    }
//
//    static bool isList(const BlockData& data) {
//        return std::holds_alternative<List>(data);
//    }
//
//    static bool isAudioSignal(const BlockData& data) {
//        return std::holds_alternative<AudioSignal>(data);
//    }
//
//    // Getters for BlockData types
//    static std::optional<Number> getNumber(const BlockData& data) {
//        if (auto val = std::get_if<Number>(&data)) {
//            return *val;
//        }
//        return std::nullopt;
//    }
//
//    static std::optional<Message> getMessage(const BlockData& data) {
//        if (auto val = std::get_if<Message>(&data)) {
//            return *val;
//        }
//        return std::nullopt;
//    }
//
//    static std::optional<List> getList(const BlockData& data) {
//        if (auto val = std::get_if<List>(&data)) {
//            return *val;
//        }
//        return std::nullopt;
//    }
//
//    static std::optional<AudioSignal> getAudioSignal(const BlockData& data) {
//        if (auto val = std::get_if<AudioSignal>(&data)) {
//            return *val;
//        }
//        return std::nullopt;
//    }
//
//    // Add more utility methods as needed
//
//    // Example: Convert BlockData to string (for debugging or display)
//    static std::string toString(const BlockData& data) {
//        return std::visit([](const auto& val) -> std::string {
//            using T = std::decay_t<decltype(val)>;
//            if constexpr (std::is_same_v<T, Number>) {
//                return "Number: " + std::to_string(val);
//            } else if constexpr (std::is_same_v<T, Message>) {
//                return "Message: " + val;
//            } else if constexpr (std::is_same_v<T, List>) {
//                std::string result = "List: ";
//                for (const auto& elem : val) {
//                    result += std::visit([](const auto& innerVal) -> std::string {
//                        using InnerT = std::decay_t<decltype(innerVal)>;
//                        if constexpr (std::is_same_v<InnerT, Number>) {
//                            return std::to_string(innerVal) + " ";
//                        } else if constexpr (std::is_same_v<InnerT, Message>) {
//                            return innerVal + " ";
//                        }
//                        return "";
//                    }, elem);
//                }
//                return result;
//            } else if constexpr (std::is_same_v<T, AudioSignal>) {
//                return "AudioSignal with " + std::to_string(val.size()) + " samples";
//            } else {
//                return "Uninitialized";
//            }
//        }, data);
//    }
//};

//--------------------

//using InitializationArguments = std::vector<std::string>;
//using InitializationArguments = BlockData;
//using SystemMessageCallback = std::function<void(const std::string&, const BlockData&)>;
using SystemMessageCallback = std::function<void(class BlockInterface* sender, const std::string&, const BlockData&)>;

using OutletCallback = std::function<void(class BlockInterface*, size_t, const BlockData&)>;


class BlockInterface {
    
public:
    virtual ~BlockInterface() = default;
    
    // Initialize the block with dynamic arguments
    virtual void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) = 0;
    // Set an input at a specific index
    virtual void setInput(int index, const BlockData& data) = 0;
    
    // New methods to get the count of inputs and outputs
    virtual size_t getNumberOfInputs() const = 0;
    virtual size_t getNumberOfOutputs() const  = 0;
    
    virtual void timerTick()  {}

    
    //for audio block (optional)
    // Method to configure or update audio parameters
    //if it is a block that handles Audio Processing. this method will be called after init()
    virtual void configureAudioParameters(int numSamples, double sampleRate) {
        // Default implementation does nothing
        // Override in subclasses that handle audio processing
    }
    virtual void processAudio() {};

    // Get the audio output for a specific outlet
    virtual AudioSignal getAudioOutput(size_t outletIndex) const {
        return AudioSignal(); // Return an empty signal by default
    }

    virtual bool handlesAudioProcessing() const {return false;}
    virtual bool handlesAudioOutput() const{return false;}
    
    virtual BlockData getInfo(const std::string& query = "") const {
        // Default implementation returns a generic message or handles query
//        if (query.empty()) {
//            return Message("No specific block information provided.");
//        } else {
//            // Handle specific queries based on the parameter
//            // Example: if query == "name", return the block's name
//            // This is just a placeholder implementation, adjust according to your needs
//            if (query == "name") {
//                return Message("Generic Block");
//            }
//            // Add more cases as needed
//        }
        return Uninitialized{};
    }

    
    
private:

    
};

typedef BlockInterface* (*CreateBlockInstanceFunction)();


