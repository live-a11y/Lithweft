//
//  Wave_Tilde.cpp
//  LithweftBlocks
//
//  Created by 刘昊 on 2024/2/20.
//

#include <stdio.h>
#include "BlockInterface.h"
#include <cmath>

class Wave : public BlockInterface {
private:
    
    std::string channelName;
    int i_startSample = 0; // Use i_ prefix for integer type
      int i_endSample = -1; // Use -1 to indicate "until the end of the wavetable"
    AudioSignal a_startSample; // Dynamic start sample position as audio signal
    AudioSignal a_endSample; // Dynamic end sample position as audio signal
    
    AudioSignal phaseExtra; // Phase increments for each audio frame
    AudioSignal outputSignal; // Output signal buffer
    
    AudioSignal wavetable; // Wavetable data
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

    double sampleRate;
    int numSamples;
    
    static constexpr size_t START_SAMPLE_INLET = 1;
    static constexpr size_t END_SAMPLE_INLET = 2;
    
public:
    Wave() {}

    //step [2]
    void configureAudioParameters(int numSamples, double sampleRate) override {
        this->sampleRate = sampleRate;
        this->numSamples = numSamples;

        // Resize the output buffer to match the new number of samples
        outputSignal.resize(numSamples);
        phaseExtra.resize(numSamples);

        // Fill a_startSample and a_endSample with i_startSample and i_endSample values
        a_startSample.assign(numSamples, i_startSample);
        a_endSample.assign(numSamples, i_endSample == -1?wavetable.size():i_endSample);
        
//        if (i_endSample == -1){
//            std::fill(a_endSample.begin(), a_endSample.end(), wavetable.size());
//        }

    }

    
    //step [0]
    void init(const BlockData& args, SystemMessageCallback smCallback, OutletCallback oCallback) override {
        systemMessageCallback = smCallback;
        outletCallback = oCallback;

        // Reset start and end to default values
        i_startSample = 0;
        i_endSample = -1; // Use -1 as a marker for "use the entire wavetable length"

        if (std::holds_alternative<List>(args)) {
            const auto& listArgs = std::get<List>(args);

            // Check for channel name
            if (!listArgs.empty() && std::holds_alternative<Message>(listArgs[0])) {
                channelName = std::get<Message>(listArgs[0]);
            }

            // Check if there is a second argument for the start sample
            if (listArgs.size() >= 2 && std::holds_alternative<Number>(listArgs[1])) {
                i_startSample =  static_cast<int>(std::get<Number>(listArgs[1]));
            }

            // Check if there is a third argument for the end sample
            if (listArgs.size() >= 3 && std::holds_alternative<Number>(listArgs[2])) {
                i_endSample = std::max(i_startSample, static_cast<int>(std::get<Number>(listArgs[2])));
            }
        } else if (std::holds_alternative<Message>(args)) {
            // If there's only one argument and it's a Message, it's the channel name
            channelName = std::get<Message>(args);
        }

        // Request the corresponding wavetable data if channelName is set
        if (!channelName.empty()) {
            systemMessageCallback(this, "requestSharedBuffer", Message(channelName));
        }
    }




    //step [1]
    void setInput(int index, const BlockData& data) override {
        // Special index for shared buffer (wavetable update)
        //step [1]
        if (index == -1 && std::holds_alternative<AudioSignal>(data)) {
            // Update the wavetable with the received audio signal
            wavetable = std::get<AudioSignal>(data);
        }
        
        else if (index == 0 && std::holds_alternative<AudioSignal>(data)) {
            // Store the phase for use in processAudio
            phaseExtra = std::get<AudioSignal>(data);
        }
        else if (index == START_SAMPLE_INLET) {
            if (std::holds_alternative<Number>(data)) {
                i_startSample = static_cast<int>(std::get<Number>(data));
                // Directly assign i_startSample value to all elements in a_startSample
                std::fill(a_startSample.begin(), a_startSample.end(), i_startSample);
            } else if (std::holds_alternative<AudioSignal>(data)) {
                a_startSample = std::get<AudioSignal>(data);
            }
        } else if (index == END_SAMPLE_INLET) {
            if (std::holds_alternative<Number>(data)) {
                i_endSample = static_cast<int>(std::get<Number>(data));
                // Directly assign i_endSample value to all elements in a_endSample
                std::fill(a_endSample.begin(), a_endSample.end(), i_endSample);
            } else if (std::holds_alternative<AudioSignal>(data)) {
                a_endSample = std::get<AudioSignal>(data);
            }
        }
    }


    void processAudio() override {
        // Ensure phaseExtra array matches numSamples
        if (phaseExtra.size() != numSamples) {
            std::cerr << "Warning: phaseExtra size does not match numSamples. Expected " << numSamples << ", got " << phaseExtra.size() << std::endl;
            return; // Skip processing or adjust as necessary
        }

        // Clear previous output signal
        outputSignal.clear();

        for (size_t i = 0; i < numSamples; ++i) {
            // Ensure start sample does not exceed end sample
            int startSampleIndex = std::clamp(static_cast<int>(a_startSample[i]), 0, static_cast<int>(wavetable.size()));
            int endSampleIndex = std::clamp(static_cast<int>(a_endSample[i]), startSampleIndex, static_cast<int>(wavetable.size()));
            
            // Ensure start is always less than or equal to end
            if (startSampleIndex > endSampleIndex) {
                std::swap(startSampleIndex, endSampleIndex);
            }

            double phase = phaseExtra[i];
            while (phase >= 1.0) phase -= 1.0;
            while (phase < 0.0) phase += 1.0;

            // Map phase to wavetable index within start and end bounds
            size_t index = static_cast<size_t>(phase * (endSampleIndex - startSampleIndex) + startSampleIndex);
            index = std::clamp(index, static_cast<size_t>(startSampleIndex), static_cast<size_t>(endSampleIndex));

            // Store the output signal for this frame
            outputSignal.push_back(wavetable[index]);
        }
    }




       AudioSignal getAudioOutput(size_t outletIndex) const override {
           // Return the processed output signal
           if (outletIndex == 0) {
               return outputSignal;
           }
           return AudioSignal(); // Return an empty signal for any other outlet index
       }

    
    size_t getNumberOfInputs() const override { return 3; }
    size_t getNumberOfOutputs() const override { return 1; }
    
    bool handlesAudioProcessing() const override { return true; }

    // Additional methods as needed
    void loadWavetable(const AudioSignal& newWavetable) {
        wavetable = newWavetable;
    }

    BlockData getInfo(const std::string& query = "") const override {
        // Check if the query asks for the channel name indicating a need for the buffer
        if (query == "needsBuffer") {
            return Message(channelName);
        }

        // Handle other queries or fallback to the default implementation
        return Uninitialized{};
    }

private:
    void adjustStartAndEndSamplesToBounds() {
        int wavetableSize = static_cast<int>(wavetable.size());

        // Adjust i_startSample and i_endSample to ensure they are within bounds
        i_startSample = std::clamp(i_startSample, 0, wavetableSize);
        i_endSample = (i_endSample == -1) ? wavetableSize : std::clamp(i_endSample, i_startSample, wavetableSize);

        // Adjust a_startSample and a_endSample vectors
        for (auto& sample : a_startSample) {
            sample = std::clamp(static_cast<int>(sample), 0, wavetableSize);
        }
        for (auto& sample : a_endSample) {
            sample = std::clamp(static_cast<int>(sample), 0, wavetableSize);
        }
    }
};

extern "C" BlockInterface* createBlockInstance() {
    return new Wave();
}
