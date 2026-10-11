/*
  ==============================================================================

    NumberBox.cpp
    Created: 14 Nov 2023 3:57:29pm
    Author:  刘昊

  ==============================================================================
*/

// NumberBox.h

#pragma once
#include "BlockInterface.h"
#include <cmath>
#include <JuceHeader.h>
#include "BlockComponent.h"

class NumberBox;

// CustomLabel declaration
class CustomNumberLabel : public juce::Label {
public:
    CustomNumberLabel(NumberBox* owner);
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    bool hitTest(int x, int y) override ;
    void setReadyForPerformance(bool status); // Initialize with editing disabled

private:
    NumberBox* ownerBox;
    juce::Point<int> dragStartPoint;
    double dragStartValue;
    bool isAdjustingInteger;
    bool isPerformanceStatus = false;
    
};

// NumberBox declaration
class NumberBox : public BlockComponent, public BlockInterface, public juce::Label::Listener {
public:
    NumberBox();
    void setInput(int index, const BlockData& data) override;
    size_t getNumberOfInputs() const override;
    size_t getNumberOfOutputs() const override;
    void setup(size_t num_inlet, size_t num_outlet, juce::Point<int> pos, bool isUIBlock) override;
    void paint(juce::Graphics& g) override;
    void labelTextChanged(juce::Label* label) override;
    void resized() override;
    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override;

    bool handlesAudioProcessing() const override{return false;}

    juce::var getPatchState() const override {
        auto* state = new juce::DynamicObject();
        state->setProperty("value", currentValue);
        return juce::var(state);
    }
    juce::Result restorePatchState(const juce::var& state) override {
        auto value = state["value"];
        if (! (value.isDouble() || value.isInt() || value.isInt64()) || !std::isfinite(static_cast<double>(value)))
            return juce::Result::fail("Invalid number-box value.");
        currentValue = static_cast<double>(value);
        numberLabel->setText(juce::String(currentValue, 2), juce::dontSendNotification);
        return juce::Result::ok();
    }

    // Custom methods
    double getCurrentValue() const;
    void setCurrentValue(double value);
    bool hitTest(int x, int y) override ;

//    void processAudio(AudioSignal& audioSignal, int numSamples, double sampleRate)override;
        
    
//    void prepareToProcess(double sampleRate,int numSamples) override{}
protected:
    void updateComponentState() override;
    
    
private:
    std::unique_ptr<CustomNumberLabel> numberLabel;
    double currentValue = 0.0;

    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;
};
