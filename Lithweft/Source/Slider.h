/*
  ==============================================================================

    Slider.h
    Created: 9 Nov 2023 5:42:41pm
    Author:  刘昊

  ==============================================================================
*/

#pragma once
#include "BlockInterface.h"
#include <cmath>
#include <JuceHeader.h>
#include "BlockComponent.h"

#define NUM_INPUTS  1
#define NUM_OUTPUTS  1

class Slider : public BlockComponent , public BlockInterface , public juce::Slider::Listener
{
    
    public :
    
    Slider()
    {
        slider.addListener(this); // Register this class as a listener to the slider
    }
    
    
    ~Slider(){
        
        
    }
    
    juce::var getPatchState() const override {
        auto* state = new juce::DynamicObject();
        state->setProperty("value", slider.getValue());
        return juce::var(state);
    }
    juce::Result restorePatchState(const juce::var& state) override {
        auto value = state["value"];
        if (! (value.isDouble() || value.isInt() || value.isInt64()) || !std::isfinite(static_cast<double>(value)))
            return juce::Result::fail("Invalid slider value.");
        auto number = static_cast<double>(value);
        if (number < slider.getMinimum() || number > slider.getMaximum())
            return juce::Result::fail("Slider value is out of range.");
        slider.setValue(number, juce::dontSendNotification);
        return juce::Result::ok();
    }

    // Set an input at a specific index
    void setInput(int index, const BlockData& data) override{
        
        if(index == 0 && std::holds_alternative<Number>(data))  {
            slider.setValue(std::get<Number>(data));
            outletCallback((BlockInterface*)this, 0, slider.getValue());
        }
    }
    
    
    
    // New methods to get the count of inputs and outputs
    size_t getNumberOfInputs() const  override{
        
        return  NUM_INPUTS ;
    }
    size_t getNumberOfOutputs() const override {
        
        return  NUM_OUTPUTS;
    }
    

    
    
    void setup(size_t num_inlet,size_t num_outlet,juce::Point<int> pos,bool isUIBlock) override{
        
        //        std::cout<<"pos for number box:"<<pos.toString()<<std::endl;
        BlockComponent::setup(num_inlet,num_outlet,pos,isUIBlock);
        
        // Configure the Slider component
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        slider.setRange(0.0, 100.0, 0.1); // Set range min, max, interval
        slider.setValue(50.0,juce::dontSendNotification); // Set default value
        slider.setColour(juce::Slider::thumbColourId, juce::Colours::red);
        // Set the color for the track
        slider.setColour(juce::Slider::trackColourId, juce::Colours::black);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::yellow);
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::green);
        
        slider.setBounds(10,PORT_HEIGHT,20,100);
        setBounds(pos.x, pos.y, slider.getWidth()+20, slider.getHeight()+PORT_HEIGHT*2);
        addAndMakeVisible(slider);
        
    }
    
    // This method is called when the slider's value changes
    void sliderValueChanged(juce::Slider* slider) override {
        if (slider == &this->slider) {
            //            DBG("Slider value changed: " << slider->getValue());
            outletCallback((BlockInterface*)this, 0, slider->getValue());

        }
    }
    
    
    
    // GUI Component overrides
    void paint(juce::Graphics& g) override{
        BlockComponent::paint(g);
        
    }
    void resized() override{
        
        
    }
    
    // Initialize the block with dynamic arguments
    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;
        
    }
    
    
protected:
    void updateComponentState() override{
//        BlockComponent::updateComponentState();
        if (currentMode == ApplicationMode::Editing){
//            slider.setEnabled(false);
        }
        
        if(currentMode == ApplicationMode::Performance){
//            slider.setEnabled(true);

        }
    };
    
    
private:
    juce::Slider slider;
//    std::vector<BlockData> inputs;
//    std::vector<BlockData> outputs;
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

    
};

