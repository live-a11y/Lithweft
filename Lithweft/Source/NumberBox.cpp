/*
  ==============================================================================

    NumberBox.cpp
    Created: 14 Nov 2023 9:48:14pm
    Author:  刘昊

  ==============================================================================
*/

#include "NumberBox.h"

// CustomNumberLabel implementation
CustomNumberLabel::CustomNumberLabel(NumberBox* owner) : ownerBox(owner) {}

void CustomNumberLabel::mouseDown(const juce::MouseEvent& event) {
    dragStartValue = ownerBox->getCurrentValue();
    dragStartPoint = event.getPosition();
    isAdjustingInteger = dragStartPoint.x < getWidth() / 2;
}

void CustomNumberLabel::mouseDrag(const juce::MouseEvent& event) {
    auto distanceY = dragStartPoint.y - event.y;
    double adjustment = distanceY * (isAdjustingInteger ? 1.0 : 0.01);
    double newValue = dragStartValue + adjustment;
    ownerBox->setCurrentValue(newValue);
}

bool CustomNumberLabel::hitTest(int x, int y)  {
    return isPerformanceStatus;  // Always return false to ignore mouse events
}

void CustomNumberLabel::setReadyForPerformance(bool status){
    isPerformanceStatus = status;
}


// NumberBox implementation
NumberBox::NumberBox()
    : numberLabel(std::make_unique<CustomNumberLabel>(this)) {
    
    // Add additional settings for numberLabel
    numberLabel->setJustificationType(juce::Justification::centred); // Center text normally
    numberLabel->addListener(this); // Register as a listener
    numberLabel->setEditable(true, true, false); // Make label editable on single-click
//    numberLabel->setText(juce::String(currentValue), juce::dontSendNotification);
    numberLabel->setText(juce::String(0), juce::dontSendNotification);

    addAndMakeVisible(numberLabel.get());
}


void NumberBox::setInput(int index, const BlockData& data) {
    if (index == 0 && std::holds_alternative<Number>(data)) {
        currentValue = std::get<Number>(data);
        setCurrentValue(currentValue);
    } else if (index == 0 && std::holds_alternative<Message>(data)) {
        if (std::get<Message>(data) == "Trigger") {
            outletCallback(this, 0, currentValue);
        }
    }
}





size_t NumberBox::getNumberOfInputs() const {
    return 1;
}

size_t NumberBox::getNumberOfOutputs() const {
    return 1;
}



void NumberBox::setup(size_t num_inlet, size_t num_outlet, juce::Point<int> pos, bool isUIBlock) {
    BlockComponent::setup(num_inlet, num_outlet, pos, isUIBlock);

    numberLabel->setBounds(0, PORT_HEIGHT, 60, 20);
    numberLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    numberLabel->setJustificationType(juce::Justification::centred);
    numberLabel->setEditable(true, false, false);
    
    setBounds(pos.x, pos.y, numberLabel->getWidth(), numberLabel->getHeight() + PORT_HEIGHT * 2);
}

void NumberBox::paint(juce::Graphics& g) {
    BlockComponent::paint(g); // Call the base class paint method
    
//    g.setColour(juce::Colours::grey); // Set the color for the frame

//    g.fillRect(2,2,getWidth()-4,getHeight()-4);
    // Draw a black frame around the component
    
    g.fillAll(juce::Colours::transparentBlack); // Background color
    g.setColour(juce::Colours::black);   // Outline color
    g.drawRect(getLocalBounds(), 1);     // Draw outline
    
    g.setColour(juce::Colours::grey);   // Outline color
    g.fillRect(2, 2, getWidth()-4, getHeight()-4);
//    g.setColour(juce::Colours::black); // Set the color for the frame
//    g.drawRect(1,1,getWidth()-2,getHeight()-2, 1); // Draw a rectangle with a 1-pixel thickness
}

void NumberBox::labelTextChanged(juce::Label* label) {
    if (label == numberLabel.get()) {
        currentValue = numberLabel->getText().getFloatValue();
        outletCallback(this, 0, currentValue);
    }
}

void NumberBox::updateComponentState() {
        numberLabel->setReadyForPerformance((currentMode == ApplicationMode::Performance));
}

bool NumberBox::hitTest(int x, int y)  {
    return  (currentMode == ApplicationMode::Performance);
}

    
void NumberBox::resized() {
    // Implement resizing logic, if needed
}

void NumberBox::init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) {
    this->systemMessageCallback = systemMessageCallback;
    this->outletCallback = outletCallback;
}


double NumberBox::getCurrentValue() const {
    return currentValue;
}

void NumberBox::setCurrentValue(double value) {
    currentValue = value;
    numberLabel->setText(juce::String(currentValue, 2), juce::dontSendNotification);
    outletCallback(this, 0, currentValue);
}
