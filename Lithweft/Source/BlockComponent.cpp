/*
  ==============================================================================

    BlockComponent.cpp
    Created: 9 Nov 2023 10:36:08am
    Author:  刘昊

  ==============================================================================
*/

#include "BlockComponent.h"

BlockComponent::BlockComponent(){
//    textEditor.setListener(this);
    textEditor.setListener(this); // Register BlockComponent as a listener
    addAndMakeVisible(textEditor);
    
//    textEditor.onTextChange = [this] { /* Handle text change event */ };
//    textEditor.onReturnKey = [this] {
////        loadBlockBasedOnInput();
//    };
//    
//    textEditor.onEscapeKey=[this]{
////        loadBlockBasedOnInput();
//    };
//    
//    textEditor.onFocusLost=[this]{
//        loadBlockBasedOnInput();
//    };

    
}


BlockComponent::~BlockComponent(){

    removeChildComponent(&textEditor);
    
}

std::vector<std::string> splitStringByWhitespace(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> results(
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>()
    );
    return results;
}



void BlockComponent::textEditorFocusLost(CustomTextEditor* editor){
    // Handle focus lost event
    std::cout<<"textEditorFocusLost"<<std::endl;
    adjustSizeToText();
    loadBlockBasedOnInput();
    
}



void BlockComponent::adjustSizeToText() {
    if(!isUIBlockComponent){
        auto currentText = textEditor.getText();
        juce::Font currentFont = textEditor.getFont();
        
        // Measure text width and add padding
        int textWidth = currentFont.getStringWidth(currentText) + 20; // Add padding
        int textHeight = currentFont.getHeight() + 4; // Height with padding
        
        // Ensure minimum dimensions
        int minWidth = fmax(textWidth, 80); // Minimum width
        int minHeight = fmax(textHeight, BLOCK_HEIGHT);
        
        // Resize the component to fit text
        setSize(minWidth, minHeight);
        
        // Adjust the bounds of the text editor within the block
        const int padding = 2;
        auto bounds = getLocalBounds().reduced(padding);
        textEditor.setBounds(bounds);
        
        // Call resized and repaint to reflect the changes
        resized();
        repaint();
    }
}



//void BlockComponent::loadBlockBasedOnInput() {
//    auto currentText = textEditor.getText().toStdString();
//
//    if (currentText != lastText) {
//        auto parts = splitStringByWhitespace(currentText);
//        if (!parts.empty()) {
//            const auto& blockName = parts[0];
//            std::vector<std::string> args(parts.begin() + 1, parts.end());
//
//            bool shouldReplace = !lastText.empty(); // True if there was a previous block
//
//            if (listener) {
//                listener->requestCreateBlock(this, blockName, args, shouldReplace);
//            }
//        }
////    }
//}

void BlockComponent::loadBlockBasedOnInput() {
    auto currentText = textEditor.getText().toStdString();

    if (currentText != lastText) {
        // Find the first space to separate the block name from the arguments
        size_t firstSpace = currentText.find(' ');
        std::string blockName;
        std::string args;

        if (firstSpace != std::string::npos) {
            // If there's a space, separate the block name and the rest as args
            blockName = currentText.substr(0, firstSpace);
            args = currentText.substr(firstSpace + 1); // +1 to skip the space itself
        } else {
            // If there's no space, the entire text is the block name
            blockName = currentText;
            // args will remain an empty string
        }

        bool shouldReplace = !lastText.empty(); // True if there was a previous block

        lastText = currentText; // A UI replacement may delete this component in the callback.
        if (listener) {
            // Since the listener expects a vector<string> for args, and we now have a single string,
            // we need to wrap it into a vector.
            // This makes a vector with a single element, which is the entire args string.
//            std::vector<std::string> argsVector = {args};
            listener->requestCreateBlock(this, blockName, args, shouldReplace);
        }

    }
}






void BlockComponent::paint(juce::Graphics& g){
    
//    g.fillAll(juce::Colours::lightgrey); // Background color
//    g.setColour(juce::Colours::black);   // Outline color
//    g.drawRect(getLocalBounds(), 1);     // Draw outline
//    
//    
//    
//    // Position the text editor within the block with padding
//    const int padding = 2;
//    auto bounds = getLocalBounds().reduced(padding);
//    textEditor.setBounds(bounds);
//
//
//    textEditor.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
    
    // Define the corner size for rounding
       float cornerSize = 8.0f; // Adjust the corner radius as needed
       
       // Fill background with rounded corners
       g.setColour(juce::Colours::lightgrey); // Background color
       g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerSize);
       
       // Draw outline with rounded corners
       g.setColour(juce::Colours::black); // Outline color
       g.drawRoundedRectangle(getLocalBounds().toFloat(), cornerSize, 2.0f); // Stroke width
       
       // Position the text editor within the block with padding
       const int padding = 2;
       auto bounds = getLocalBounds().reduced(padding);
       textEditor.setBounds(bounds);

       // Additional painting for ports, selection, etc., goes here...
       
       if (isSelected) {
           g.setColour(juce::Colours::blue); // Change color to indicate selection
           g.drawRoundedRectangle(getLocalBounds().toFloat(), cornerSize, 3.0f); // Draw a thicker border for selection
       }
    
    
    layoutPorts();

//    
//    if (isSelected) {
//        g.setColour(juce::Colours::blue); // Change color to indicate selection
//        g.drawRect(getLocalBounds(), 2); // Draw a thicker border
//    }
    
    
}


void BlockComponent::layoutPorts() {
    // Determine the size of each port
     int portWidth = PORT_WIDTH; // Example width for each port
     int portHeight = PORT_HEIGHT; // Example height for each port

     // Calculate the horizontal spacing between ports
     int inputSpacing = (getWidth() - (inputPorts.size() * portWidth)) / (inputPorts.size() + 1);
     int outputSpacing = (getWidth() - (outputPorts.size() * portWidth)) / (outputPorts.size() + 1);

     // Layout input ports along the top
     for (int i = 0; i < inputPorts.size(); ++i) {
         PortComponent* port = inputPorts.getUnchecked(i);
         int xPosition = inputSpacing + (i * (portWidth + inputSpacing));
         port->setBounds(xPosition, 0, portWidth, portHeight);
         
     }

     // Layout output ports along the bottom
     for (int i = 0; i < outputPorts.size(); ++i) {
         PortComponent* port = outputPorts.getUnchecked(i);
         int xPosition = outputSpacing + (i * (portWidth + outputSpacing));
         port->setBounds(xPosition, getHeight() - portHeight, portWidth, portHeight);
     }

}

bool BlockComponent::getIsUIBlockComponent(){
    
    return  isUIBlockComponent;
}

void BlockComponent::setup(size_t num_inlet,size_t num_outlet,juce::Point<int> pos,bool isUIComponent){
    
    setBounds(pos.x, pos.y ,BLOCK_WIDTH , BLOCK_HEIGHT);

    
//    std::cout<<"setup block at :"<<this<<std::endl;

    // Clear existing ports
    inputPorts.clear();
    outputPorts.clear();

    // Assuming blockInstance is the BlockInterface that owns these ports
    // Create input ports
    for (size_t i = 0; i < num_inlet; ++i) {
        auto* port = new PortComponent(PortComponent::Type::Inlet, i,this);
//        port->setListener(portListener);
        addAndMakeVisible(port);
        port->toFront(false);
        inputPorts.add(port);
    }

    // Create output ports
    for (size_t i = 0; i < num_outlet; ++i) {
        auto* port = new PortComponent(PortComponent::Type::Outlet, i,this);
//        port->setListener(portListener);
        addAndMakeVisible(port);
        port->toFront(false);
        outputPorts.add(port);
    }
    
    isUIBlockComponent = isUIComponent;

    if(isUIComponent){
//        this->removeChildComponent(&textEditor);
        textEditor.setVisible(false);
        textEditor.setEnabled(false);
    }
    
    std::cout<<"posx "<<pos.x<<" posy"<<pos.y<<std::endl;
    resized();
    repaint();
    std::cout<<"pos in BlockComponent:"<<pos.toString()<<std::endl;
}


std::vector<PortComponent*> BlockComponent:: getPorts() const {
    std::vector<PortComponent*> allPorts;

    for (auto* inputPort : inputPorts) {
        allPorts.push_back(inputPort);
    }

    for (auto* outputPort : outputPorts) {
        allPorts.push_back(outputPort);
    }

    return allPorts;
}


PortComponent* BlockComponent::getPortComponent(size_t index, PortComponent::Type type) {
    if (type == PortComponent::Type::Inlet) {
        // Check if the index is valid for input ports
        if (index < inputPorts.size()) {
            return inputPorts.getUnchecked(static_cast<int>(index));
        }
    } else if (type == PortComponent::Type::Outlet) {
        // Check if the index is valid for output ports
        if (index < outputPorts.size()) {
            return outputPorts.getUnchecked(static_cast<int>(index));
        }
    }

    return nullptr; // Port not found or index out of bounds
}


void BlockComponent::switchMode(ApplicationMode newMode) {
    currentMode = newMode;
    updateComponentState();
}

void BlockComponent::updateComponentState() {
    textEditor.setReadyForEditing(false);

}

bool BlockComponent::hitTest(int x, int y)  {
    return  (currentMode == ApplicationMode::Performance);
//    return  true;
}

void BlockComponent::setSelected(bool select) {
       isSelected = select;
       repaint(); // Trigger a repaint to update the visual state
   }

   bool BlockComponent::getSelected() const {
       return isSelected;
   }


  // Call this method to delete the block
void BlockComponent::requestDeletion() {
        if (listener) {
            listener->requestDeleteBlock(this);
        }
    }

void BlockComponent::setEditorReadyForEditing(bool isReady){
    
    if(currentMode == ApplicationMode::Editing){
        grabKeyboardFocus();
        textEditor.setReadyForEditing(isReady);
//        std::cout<<"textEditor.setReadyForEditing(isReady); "<<(isReady ?"YES":"NO")<<std::endl;

    }
}


//====================================================================

void BlockComponent::visibilityChanged()  {
    
}

void BlockComponent::parentHierarchyChanged()  {
    if (isShowing()) {
        grabKeyboardFocus();
    }
}

void BlockComponent::notifyCreationStatus(bool isSucssessful){
    
    std::cout<<"create block isSucssessful : "<<(isSucssessful ?"YES":"NO")<<std::endl;
//    if(isSucssessful){
//        adjustSizeToText(); //Adjust size as the user types
//    }

}


//void BlockComponent::mouseDoubleClick(const juce::MouseEvent& event) {
//    // Disable direct editing on double click
//    // Editing is now controlled by PatchController
//    
//}



//void BlockComponent::mouseDown(const juce::MouseEvent& event){
//    std::cout<<"mouse down inside component :"<<std::endl;
//
//    if(getSelected()){
//        textEditor.setReadyForEditing(true);
//    }
//    
//    setSelected(true);
//    
//}
//
//
//void BlockComponent::mouseDrag(const juce::MouseEvent& event){
//    
//}
//void BlockComponent::mouseUp(const juce::MouseEvent& event){
//
//    
//}
