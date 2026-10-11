//
//  Osc.cpp
//  LithweftBlocks
//
//  Created by 刘昊 on 2023/11/17.
//

#include <stdio.h>
#include "BlockInterface.h"
// Oscillator class
class Osc_tilde : public BlockInterface {
public:



    virtual ~Osc_tilde() override {}
    
    virtual void configureAudioParameters(int numSamples, double sampleRate) override{
        
//        std::cout<<"in configureAudioParameters:"<<std::endl;
//        std::cout<<"numSamples:"<<numSamples<<std::endl;
//        std::cout<<"sampleRate:"<<sampleRate<<std::endl;
//        std::cout<<"frequency size:"<<frequency.size()<<std::endl;
//        std::cout<<"phaseOffset size:"<<phaseOffset.size()<<std::endl;
        
        this->numSamples = numSamples;
        this->sampleRate =  sampleRate;
        
        
            audioOutput_outlet_0.resize(numSamples,0.0);
            audioOutput_outlet_1.resize(numSamples,0.0);
            
            frequency.resize(numSamples,frequencyNumber);
            phaseOffset.resize(numSamples,phaseOffsetNumber);

    }
    
    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback)override {
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;

        // Resetting values to default
        frequencyNumber = 0;
        phaseOffsetNumber = 0;

        // Handling args based on its type
        if (std::holds_alternative<Number>(args)) {
            // If args is a Number, use it for frequencyNumber
            frequencyNumber = std::get<Number>(args);
        }
        else if (std::holds_alternative<List>(args)) {
            // If args is a List, use the first element for frequencyNumber and the second for phaseOffsetNumber
            const List& argsList = std::get<List>(args);
            if (argsList.size() >= 2) {
                frequencyNumber = getNumberFromListElement(argsList[0]);
                phaseOffsetNumber = getNumberFromListElement(argsList[1]);
            }
        }
        // If args is empty or any other type, frequencyNumber and phaseOffsetNumber remain zero
    }

    Number getNumberFromListElement(const ListElement& element) {
        if (std::holds_alternative<Number>(element)) {
            return std::get<Number>(element);
        }
        else if (std::holds_alternative<Message>(element)) {
            try {
                return std::stod(std::get<Message>(element));
            } catch (...) {
                // Handle conversion error
            }
        }
        return 0; // Default return if not a number or conversion fails
    }


    
    void setInput(int index, const BlockData& data) override {
         if (index < 2) {
    
             
             if(index == 0){
                 if(std::holds_alternative<Number>(data)){ // for pheas getting number type
                     frequencyNumber = std::get<Number>(data);
                     std::fill(frequency.begin(), frequency.end(), frequencyNumber);
                     
                 }
                 
                 if(std::holds_alternative<AudioSignal>(data)){ // for pheas getting AudioSignal type
                     frequency = std::get<AudioSignal>(data);
                 }
                 
             }

             if(index == 1){
                 if(std::holds_alternative<Number>(data)){ // for pheas getting number type
                     phaseOffsetNumber = std::get<Number>(data);
                     std::fill(phaseOffset.begin(), phaseOffset.end(), phaseOffsetNumber);
                 }
                 
                 if(std::holds_alternative<AudioSignal>(data)){ // for pheas getting AudioSignal type
                     phaseOffset = std::get<AudioSignal>(data);

                 }
                 
             }
             

         }
     }


    
//    void processAudio() override {
//        // Update sample rate and prepare for processing
//
//        int numSamples = this->numSamples;
//        
//        // Process audio samples based on the type of frequency and phaseOffset
//        std::visit([this, numSamples](const auto& freqVal) {
//            using T = std::decay_t<decltype(freqVal)>;
//            if constexpr (std::is_same_v<T, Number>) {
//                // Frequency is a single Number, process accordingly
//
//     double phaseInc = 2 * M_PI * freqVal / sampleRate;
//                for (int i = 0; i < numSamples; ++i) {
//                    audioOutput_outlet_0[i] = std::sin(currentPhase + getCurrentPhaseOffset(i));
//                    audioOutput_outlet_1[i] = audioOutput_outlet_0[i]*200+100;
//                    currentPhase += phaseInc;
//                    wrapPhase();
//                }
//            } else if constexpr (std::is_same_v<T, AudioSignal>) {
//                // Frequency is an AudioSignal (vector), process each sample
//                for (int i = 0; i < numSamples; ++i) {
//                    double phaseInc = 2 * M_PI * freqVal[i] / sampleRate;
//                    audioOutput_outlet_0[i] = std::sin(currentPhase + getCurrentPhaseOffset(i));
//                    audioOutput_outlet_1[i] = audioOutput_outlet_0[i]*200+100;
//                    currentPhase += phaseInc;
//                    wrapPhase();
//                }
//            }
//        }, frequency);
//        
////        outletCallback(this,1,Number(audioOutput[0]));
//    }
    
//    void processAudio() override {
//        
////        std::cout<<"in process AUDIO"<<std::endl;
////        std::cout<<"numSamples:"<<numSamples<<std::endl;
////        std::cout<<"sampleRate:"<<sampleRate<<std::endl;
////        std::cout<<"frequency size:"<<frequency.size()<<std::endl;
////        std::cout<<"phaseOffset size:"<<phaseOffset.size()<<std::endl;
//        
//        for (int i = 0; i < numSamples; ++i) {
//            double phaseInc = 2 * M_PI * frequency[i] / sampleRate;
//            audioOutput_outlet_0[i] = std::sin(currentPhase + phaseOffset[i]);
//            audioOutput_outlet_1[i] = (currentPhase);
//            currentPhase += phaseInc;
//
//            if (currentPhase > 2 * M_PI) {
//                currentPhase -= 2 * M_PI;
//            }
//        }
//    }
    
    
    void processAudio() override {
        for (int i = 0; i < numSamples; ++i) {
            double phaseInc = 2 * M_PI * frequency[i] / sampleRate;
            audioOutput_outlet_0[i] = std::sin(currentPhase + phaseOffset[i]);
            
            // Normalize currentPhase to [0, 1] and store in audioOutput_outlet_1
            audioOutput_outlet_1[i] = currentPhase / (2 * M_PI);

            currentPhase += phaseInc;
            
            // Ensure currentPhase wraps around correctly within [0, 2 * PI]
            while (currentPhase >= 2 * M_PI) currentPhase -= 2 * M_PI;
            while (currentPhase < 0) currentPhase += 2 * M_PI;
        }
    }



    AudioSignal getAudioOutput(size_t outletIndex) const override {
        switch (outletIndex) {
            case 0:
                return audioOutput_outlet_0; // Return an empty signal by default
                break;
                
            case 1:
                return audioOutput_outlet_1; // Return an empty signal by default
                break;
                
            default:
                break;
        }
        
        return  AudioSignal();
    }
    
    bool handlesAudioProcessing() const override {
        return true;
    }

    size_t getNumberOfInputs() const override {
        return 2; // Frequency and phase inputs
    }

    size_t getNumberOfOutputs() const override {
        return 2; // One output (audio signal)
    }

private:
    int numSamples;
    double sampleRate;

    AudioSignal frequency;
    AudioSignal phaseOffset; // Changed to BlockData type
    
    Number frequencyNumber;
    Number phaseOffsetNumber; // Changed to BlockData type
    
    double currentPhase = 0.0;
    double phaseIncrement;
    
    AudioSignal audioOutput_outlet_0; // Output audio signal
    AudioSignal audioOutput_outlet_1; // Output audio signal
    
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;
    
    

};
// Factory function
extern "C" BlockInterface* createBlockInstance() {
    Osc_tilde* block = new Osc_tilde();
    std::cout << "Oscillator block created: " << block << std::endl;
//    block->init(args, messageCallback, outletCallback);
    return block;
}
