/*
  ==============================================================================

    ParameterBox.h
    Created: 1 Dec 2023 6:52:07pm
    Author:  刘昊

  ==============================================================================
*/


#pragma once
#include "BlockComponent.h"
#include <JuceHeader.h>
#include "BlockInterface.h"
//#incoude "CustomTextEditor.h"

class ParameterBox : public BlockComponent , public BlockInterface  {

public:
    ParameterBox() {
        textEditor.setListener(this);
    }
    
    juce::var getPatchState() const override {
        auto* state = new juce::DynamicObject();
        state->setProperty("text", textEditor.getText());
        return juce::var(state);
    }
    juce::Result restorePatchState(const juce::var& state) override {
        if (!state["text"].isString()) return juce::Result::fail("Invalid message-box text.");
        textEditor.setText(state["text"].toString(), false);
        return juce::Result::ok();
    }

    // Initialize the block with dynamic arguments
    void init(const BlockData& args, SystemMessageCallback systemMessageCallback, OutletCallback outletCallback) override {
        
        this->systemMessageCallback = systemMessageCallback;
        this->outletCallback = outletCallback;
        
    }
    
    
    // Set an input at a specific index
    void setInput(int index, const BlockData& data) override{

    }
    
    
 
    // New methods to get the count of inputs and outputs
    size_t getNumberOfInputs() const  override{
        
        return  2 ;
    }
    size_t getNumberOfOutputs() const override {
        
        return  1;
    }
    
    
    void textEditorFocusLost(CustomTextEditor* editor) override{
        
        adjustSizeToText();
    }

    
    void adjustSizeToText() override {
        auto currentText = textEditor.getText();
        juce::Font currentFont = textEditor.getFont();

        // Measure text width and add padding
        int textWidth = currentFont.getStringWidth(currentText) + 20; // Add padding

        // Estimate the height of the text based on the number of lines
        // Split the text by newline characters to count lines
        int lineCount = 1;
        std::istringstream iss(currentText.toStdString());
        std::string line;
        while (std::getline(iss, line)) {
            lineCount++;
        }

        // Calculate the estimated text height
        int textHeight = currentFont.getHeight() * lineCount;

        // Ensure minimum dimensions
        int minWidth = std::max(textWidth, BLOCK_WIDTH); // Minimum width
        int minHeight = std::max(textHeight + 4, BLOCK_HEIGHT); // Minimum height

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

    
    void textEditorMouseClicked(CustomTextEditor* editor, const juce::MouseEvent& event) override {
        if (editor == &textEditor) {
            // Change the background color to indicate a click
            textEditor.changeBackgroundColor(juce::Colours::blue);

            // Reset the background color after a short delay
            juce::Timer::callAfterDelay(200, [safe = juce::Component::SafePointer<ParameterBox>(this)]() {
                if (safe != nullptr) safe->textEditor.resetBackgroundColor();
            });

            
            // Process the input from the text editor
//            auto processedData = processStringInput(textEditor.getText().toStdString());

            // Trigger the outlet callback with the processed data
//            this->outletCallback(this, 0, processedData);
            processInputStringAndSendData();
            
        }
    }

    

    
    
    void setup(size_t num_inlet,size_t num_outlet,juce::Point<int> pos,bool isUIBlock) override{
        
        
        //        std::cout<<"pos for number box:"<<pos.toString()<<std::endl;
        BlockComponent::setup(this->getNumberOfInputs(),this->getNumberOfOutputs(),pos,isUIBlock);
        
        // Configure the Slider component
        // Configure the text editor (similarly to BlockComponent)
        textEditor.setMultiLine(true); // Allow multi-line input
        textEditor.setReturnKeyStartsNewLine(true); // Pressing return starts a new line
        textEditor.setReadOnly(false); // Allow editing
        textEditor.setScrollbarsShown(true); // Show scrollbars if needed
        
        textEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::lightgrey);
        
        textEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentWhite);
        textEditor.setColour(juce::TextEditor::highlightColourId, juce::Colours::transparentWhite);
        textEditor.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        
        
        // Set the text color and shadow color
        textEditor.setColour(juce::TextEditor::textColourId, juce::Colours::white);
        setColour(juce::TextEditor::shadowColourId, juce::Colours::black);

        textEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentWhite);
        
        this->setBounds(pos.x,pos.y,80,30);
//        textEditor.toFront(true);

        this->addAndMakeVisible(textEditor);
        textEditor.toBack();
        
        resized();
        repaint();
        
    }
    
    void updateComponentState() override{
        textEditor.setReadyForEditing(false);
        textEditor.setPerformanceMode((currentMode == ApplicationMode::Performance));
    }
    
    void setEditorReadyForEditing(bool isReady) override{
        textEditor.setReadyForEditing(true);
        grabKeyboardFocus();
    }
    
    
    
    void resized() override {
        textEditor.setBounds(getLocalBounds().reduced(3)); // Add some padding

    }

    void paint(juce::Graphics& g) override {
        BlockComponent::paint(g);
        // Additional painting code if needed
//        g.fillAll(juce::Colours::grey);
        
        
    //    std::cout<<"paint in BlockComponent: "<<this<<std::endl;
//        g.fillAll(juce::Colours::lightgrey);  //Background color

        g.setColour(juce::Colours::transparentBlack);   // Outline color
        
        g.drawRect(getLocalBounds(), 1);     // Draw outline
        
    }


//    BlockData processStringInput(const std::string& input) {
//        std::istringstream iss(input);
//        std::string token;
//        std::vector<ListElement> elements;
////        List elements;
//
//        while (iss >> token) {
//            // Check for quoted string
//            if (token.size() >= 2 && token.front() == '"' && token.back() == '"') {
//                elements.emplace_back(Message(token.substr(1, token.size() - 2)));
////                elements.emplace_back(Message(token.substr(0, token.size() - 1)));
//            }
//            // Try to parse as a number
//            else {
//                try {
//                    double number = std::stod(token);
//                    elements.emplace_back(Number(number));
//                } catch (const std::invalid_argument&) {
//                    elements.emplace_back(Message(token));
//                }
//            }
//        }
//
//        // Determine the type of BlockData to return
//        if (elements.size() == 1) {
//            // If only one element, return it directly
//            return std::visit([](const auto& elem) -> BlockData {
//                using T = std::decay_t<decltype(elem)>;
//                if constexpr (std::is_same_v<T, Number>) {
//                    return Number(elem);
//                } else {
//                    return Message(elem);
//                }
//            }, elements.front());
//        } else if (elements.empty()) {
//            // If no elements, return an empty message
//            return Message("");
//        } else {
//            // If multiple elements, return as a list
//            return List(elements);
//        }
//        
//        // Determine the type of BlockData to return
//
//    }

    void processInputStringAndSendData() {
        auto inputString = textEditor.getText().toStdString();
        std::istringstream iss(inputString);
        std::string segment;

        while (std::getline(iss, segment, ',')) {
            trimString(segment); // Remove leading/trailing whitespace

            if (isQuotedString(segment)) {
                // Quoted string, treat as a Message
//                segment = segment.substr(1, segment.length() - 2); // Remove quotes
                outletCallback(this, 0, Message(segment));
            } else if (isStrictNumber(segment)) {
                // Strict number
                double number = std::stod(segment);
                outletCallback(this, 0, Number(number));
            } else {
                // Split by spaces for a List or treat as a single Message
                processSegment(segment);
            }
        }
    }

    void processSegment(const std::string& segment) {
        if (segment.find(' ') != std::string::npos) {
            // Contains spaces, split further and treat as List
            List list;
            std::istringstream wordStream(segment);
            std::string word;
            while (wordStream >> word) {
                list.push_back(Message(word));
            }
            outletCallback(this, 0, list);
        } else {
            // Single word, treat as Message
            outletCallback(this, 0, Message(segment));
        }
    }

    // Definitions of isQuotedString, isStrictNumber, and trimString remain the same

    bool isStrictNumber(const std::string& str) {
        if (str.empty() || ((str[0] != '-' && str[0] != '+') && !isdigit(str[0]))) return false;

        bool decimalPointSeen = false;
        for (size_t i = 1; i < str.size(); ++i) {
            if (str[i] == '.') {
                if (decimalPointSeen) return false; // More than one decimal point
                decimalPointSeen = true;
            } else if (!isdigit(str[i])) {
                return false; // Non-numeric character
            }
        }
        return true;
    }

    // The trimString and isQuotedString functions remain the same

    void processList(const std::string& segment) {
        std::istringstream termStream(segment);
        std::string word;
        List parsedList;
        bool isList = false;

        while (termStream >> word) {
            parsedList.push_back(Message(word));
            isList = true;
        }

        if (isList) {
            outletCallback(this, 0, parsedList);
        } else {
            // If no words were found, it's a single Message
            outletCallback(this, 0, Message(segment));
        }
    }

    // The trimString, isQuotedString, and isNumber functions remain the same


    void trimString(std::string &str) {
        // Left trim
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));

        // Right trim
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), str.end());
    }

    bool isQuotedString(const std::string& str) {
        return str.size() >= 2 && str.front() == '"' && str.back() == '"';
    }

    bool isNumber(const std::string& str) {
        std::istringstream iss(str);
        double num;
        iss >> num;
        return !(iss.fail() || !iss.eof());
    }

private:
    CustomTextEditor textEditor;
    SystemMessageCallback systemMessageCallback;
    OutletCallback outletCallback;
    
    

};
