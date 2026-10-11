/*
  ==============================================================================

    Trigger.h
    Created: 16 Feb 2024 9:48:01pm
    Author:  刘昊

  ==============================================================================
*/


// Trigger.h
#pragma once

#include "BlockInterface.h"
#include <JuceHeader.h>
#include "BlockComponent.h"
//class Trigger : public BlockComponent, public BlockInterface {
//public:
//    Trigger() {
//        triggerButton.setButtonText("");
//        triggerButton.onClick = [this] { this->buttonClicked(); };
//        triggerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
//        triggerButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white); // Highlight color
//
//        addAndMakeVisible(triggerButton);
//    }
//
//    void setInput(size_t index, const BlockData& data) override {
//        if (index == 0 && std::holds_alternative<Message>(data) && std::get<Message>(data) == "Trigger") {
//            buttonClicked(); // Programmatically "press" the button
//        }
//    }
//
//    size_t getNumberOfInputs() const override { return 1; }
//    size_t getNumberOfOutputs() const override { return 1; }
//
//    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
//        this->systemMessageCallback = systemMessageCallback;
//        this->outletCallback = outletCallback;
//    }
//
//    
//    
////    void setup(size_t num_inlet, size_t num_outlet, juce::Point<int> pos, bool isUIBlock) override {
////        BlockComponent::setup(num_inlet, num_outlet, pos, isUIBlock);
////        // Adjust the button size and position relative to the BlockComponent
////        triggerButton.setBounds(10, PORT_HEIGHT + 10, getWidth() - 20, getHeight() - (PORT_HEIGHT * 2 + 20));
////        setBounds(pos.x, pos.y, 30, 20+PORT_HEIGHT);
////    }
//    
//    void setup(size_t num_inlet, size_t num_outlet, juce::Point<int> pos, bool isUIBlock) override {
//        BlockComponent::setup(num_inlet, num_outlet, pos, isUIBlock);
//        
//        // Fixed component size
//        int componentWidth = 30;
//        int componentHeight = 30;
//
//        // Calculate the button size and position within the component
//        int buttonSize = componentWidth - 10; // Subtracting a fixed amount to account for visual margins
//        int buttonX = (componentWidth - buttonSize) / 2; // Horizontally center the button
//        int buttonY = (componentHeight - buttonSize) / 2; // Vertically center the button
//
//        // Set the button bounds
//        triggerButton.setBounds(buttonX, buttonY, buttonSize, buttonSize);
//
//        // Set the BlockComponent bounds
//        setBounds(pos.x, pos.y, componentWidth, componentHeight);
//    }
//
//
//
//
//private:
//    void buttonClicked() {
//        // Trigger the outlet callback with the "Trigger" message
//        if (outletCallback) {
//            outletCallback(this, 0, Message("Trigger"));
//        }
//        animateButtonPress();
//    }
//
//    void animateButtonPress() {
//        
//
//        
//        // Highlight the button to indicate it's been triggered
//        triggerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
//
//        // Use a timer or delay to revert the color change, simulating a brief highlight
//        juce::Timer::callAfterDelay(40, [this] {
//            triggerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
//        });
//
//
//    }
//
//    juce::TextButton triggerButton;
//    SystemMessageCallback systemMessageCallback;
//    OutletCallback outletCallback;
//
//};

//#pragma once
//
//#include "BlockInterface.h"
//#include <JuceHeader.h>
//#include "BlockComponent.h"
//
//class Trigger : public BlockComponent, public BlockInterface {
//public:
//    Trigger() {
//        // Initial state
//        isHighlighted = false;
//    }
//
//    void setInput(size_t index, const BlockData& data) override {
//        if (index == 0 && std::holds_alternative<Message>(data) && std::get<Message>(data) == "Trigger") {
//            // Simulate the highlight effect
//            if (outletCallback) {
//                outletCallback(this, 0, Message("Trigger"));
//            }
//            triggerHighlightEffect();
//        }
//    }
//
//    size_t getNumberOfInputs() const override { return 1; }
//    size_t getNumberOfOutputs() const override { return 1; }
//
//    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
//        this->systemMessageCallback = systemMessageCallback;
//        this->outletCallback = outletCallback;
//    }
//
//    void mouseDown(const juce::MouseEvent&) override {
//
//        if (outletCallback) {
//            outletCallback(this, 0, Message("Trigger"));
//        }
//        
//        
//        isHighlighted = true; // Highlight on mouse down
//        repaint();
//    }
//
//    void mouseUp(const juce::MouseEvent&) override {
//        isHighlighted = false; // Remove highlight on mouse up
//
//        repaint();
//    }
//    
//        void setup(size_t num_inlet, size_t num_outlet, juce::Point<int> pos, bool isUIBlock) override {
//            
//            BlockComponent::setup(num_inlet, num_outlet, pos, isUIBlock);
//    
//            // Fixed component size
//            int componentWidth = 30;
//            int componentHeight = 30;
//    
//            // Set the BlockComponent bounds
//            setBounds(pos.x, pos.y, componentWidth, componentHeight);
//        }
//
//    void paint(juce::Graphics& g) override {
//        BlockComponent::paint(g);
//        
//        auto bounds = getLocalBounds().toFloat().reduced(7);
//        auto fillColour = isHighlighted ? juce::Colours::white : juce::Colours::darkgrey;
//        auto outlineColour = juce::Colours::black;
//
//        g.setColour(fillColour);
//        g.fillEllipse(bounds);
//
//        g.setColour(outlineColour);
//        g.drawEllipse(bounds, 1.0f); // Stroke width
//    }
//
//    void resized() override {
//        // This component does not need to do anything in resized,
//        // as its drawing is based on its current bounds.
//    }
//
//private:
//    bool isHighlighted;
//        SystemMessageCallback systemMessageCallback;
//        OutletCallback outletCallback;
//    
//    
//    void triggerHighlightEffect() {
//        isHighlighted = true;
//        repaint();
//
//        juce::Timer::callAfterDelay(100, [this] {
//            isHighlighted = false;
//            repaint();
//        });
//    }
//};
//
//
//
//
//

#pragma once

#include "BlockInterface.h"
#include <JuceHeader.h>
#include "BlockComponent.h"
class Trigger : public BlockComponent, public BlockInterface {
public:
    Trigger() {
        isHighlighted = false;
        highlightCounter = -1; // Initialize the counter to -1 to indicate no highlight effect is active
    }

    void setInput(int index, const BlockData& data) override {
        if (index == 0 ) {
            if (outletCallback) {
                outletCallback(this, 0, Message("Trigger"));
            }
            // Start the highlight effect
            startHighlightEffect();
        }
    }

    size_t getNumberOfInputs() const override { return 1; }
    size_t getNumberOfOutputs() const override { return 1; }

    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;
    }

    void mouseDown(const juce::MouseEvent&) override {
        if (outletCallback) {
            outletCallback(this, 0, Message("Trigger"));
        }
        // Start highlight effect without timer
        isHighlighted = true;
        repaint();
    }

    void mouseUp(const juce::MouseEvent&) override {
        // End highlight effect immediately
        isHighlighted = false;
        highlightCounter = -1; // Reset counter
        repaint();
    }
    
    void setup(size_t num_inlet, size_t num_outlet, juce::Point<int> pos, bool isUIBlock) override {
        BlockComponent::setup(num_inlet, num_outlet, pos, isUIBlock);
        setBounds(pos.x, pos.y, 30, 30); // Fixed component size
    }

    void timerTick() override {
        if (highlightCounter >= 0) {
            highlightCounter--;
            if (highlightCounter == 0) {
                // End of highlight effect
                isHighlighted = false;
                highlightCounter = -1; // Reset counter
                
                juce::MessageManager::callAsync([this]() {
                     repaint();
                 });
                
                
            }
        }
    }

    void paint(juce::Graphics& g) override {
        BlockComponent::paint(g);
        auto bounds = getLocalBounds().toFloat().reduced(7);
        auto fillColour = isHighlighted ? juce::Colours::white : juce::Colours::darkgrey;
        auto outlineColour = juce::Colours::black;

        g.setColour(fillColour);
        g.fillEllipse(bounds);
        g.setColour(outlineColour);
        g.drawEllipse(bounds, 1.0f); // Stroke width
    }

private:
    bool isHighlighted;
    int highlightCounter; // Counter variable for tracking highlight duration
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;

    void startHighlightEffect() {
        isHighlighted = true;
        highlightCounter = 100; // Set for 100ms highlight duration
        
        juce::MessageManager::callAsync([this]() {
             repaint();
         });
        
    }
};
