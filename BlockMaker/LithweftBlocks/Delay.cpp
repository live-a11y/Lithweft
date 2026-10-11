#include <stdio.h>
#include "BlockInterface.h"
#include <chrono>
#include <sstream>

class Delay : public BlockInterface {
private:
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;
    int delayTimeMs; // The delay time in milliseconds
    bool triggerReceived; // Flag to indicate a trigger has been received
    std::chrono::steady_clock::time_point startTime; // Time when the trigger was received
    bool delayInProgress; // Flag to indicate the delay is in progress
    int elapsedTimeMs; // Time elapsed since the last trigger

public:
    Delay() : delayTimeMs(0), triggerReceived(false), delayInProgress(false), elapsedTimeMs(0) {}

    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;

        // Initialize delay time from args if it's a Number
        if(std::holds_alternative<Number>(args)) {
            delayTimeMs = static_cast<int>(std::get<Number>(args));
        } else {
            systemMessageCallback(this,"print", "Delay block requires a numeric initialization argument for delay time in milliseconds.");
        }
    }

    void setInput(int index, const BlockData& data) override {
        if (index == 0 && std::holds_alternative<Message>(data)) {
            Message message = std::get<Message>(data);
            if (message == "Trigger") {
                triggerReceived = true;
                delayInProgress = true;
                startTime = std::chrono::steady_clock::now();
            }
        } else if (index == 1 && std::holds_alternative<Number>(data)) {
            // Update delay time dynamically
            int newDelayTimeMs = static_cast<int>(std::get<Number>(data));
            auto currentTime = std::chrono::steady_clock::now();
            elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

            // If new delay is less than elapsed time and a trigger is pending, skip the pending trigger
            if (newDelayTimeMs < elapsedTimeMs && delayInProgress) {
                delayInProgress = false; // Cancel the pending trigger
                systemMessageCallback(this,"print", "Delay updated. Pending trigger skipped due to elapsed time exceeding new delay.");
            } else {
                delayTimeMs = newDelayTimeMs; // Update the delay time
            }

            systemMessageCallback(this,"print", "Delay time updated to " + std::to_string(delayTimeMs) + " ms.");
        }
    }

    void timerTick() override {
        if (delayInProgress) {
            auto currentTime = std::chrono::steady_clock::now();
            elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

            if (elapsedTimeMs >= delayTimeMs) {
                if (outletCallback) {
                    outletCallback(this, 0, Message("Trigger"));
                }
                delayInProgress = false;
            }
        }
    }

    size_t getNumberOfInputs() const override {
        return 2; // Two inputs: 0 for the trigger message, 1 for setting delay time
    }

    size_t getNumberOfOutputs() const override {
        return 1; // One output that sends the trigger message after the delay
    }

//    BlockData  getInfo(const std::string& query = "") const override {
//        std::ostringstream infoStream;
//        infoStream << "Block: Delay\n"
//                   << "Version: 1.0\n"
//                   << "Developer: LIU HAO\n"
//                   << "Delay Time: " << delayTimeMs << " ms\n";
//        return Message(infoStream.str());
//    }
};

extern "C" BlockInterface* createBlockInstance() {
    return new Delay();
}

