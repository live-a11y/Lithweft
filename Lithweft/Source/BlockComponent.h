/*
  ==============================================================================

    BlockComponent.h
    Created: 9 Nov 2023 10:36:08am
    Author:  刘昊

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PortComponent.h"
#include "BlockInterface.h"
#include "CustomTextEditor.h"
#define BLOCK_WIDTH 80
#define BLOCK_HEIGHT 28

class BlockComponent;


#pragma once
#include <JuceHeader.h>

class BlockListener {

public:
    virtual ~BlockListener() = default;

//    virtual void requestCreateBlock(BlockComponent* blockComponent, const std::string& blockName, const std::vector<std::string>& args, bool shouldReplace) = 0;
    virtual void requestCreateBlock(BlockComponent* blockComponent, const std::string& blockName, const std::string& args, bool shouldReplace) = 0;

//    virtual void blockDeleted(BlockComponent* block) = 0;
    virtual void requestDeleteBlock(BlockComponent* blockComponent) = 0;

};

enum class ApplicationMode {
    Editing,
    Performance
};



class BlockComponent : public juce::Component, public   CustomTextEditorListener
{
public:

    BlockComponent();
    ~BlockComponent() override;

    juce::String getCreationText() const { return isUIBlockComponent ? creationText : textEditor.getText(); }
    void setCreationText(const juce::String& text) {
        creationText = text;
        lastText = text.toStdString();
        textEditor.setText(text, false);
    }
    virtual juce::var getPatchState() const { return juce::var(new juce::DynamicObject()); }
    virtual juce::Result restorePatchState(const juce::var&) { return juce::Result::ok(); }

    virtual void setup(size_t num_inlet,size_t num_outlet,juce::Point<int> pos,bool isUIComponent);
    
    void setListener(BlockListener* newListener) {
        listener = newListener;
    }
   
   void notifyCreationStatus(bool isSucssessful);
    // Method to switch between modes
     void switchMode(ApplicationMode newMode);
    
   virtual void setEditorReadyForEditing(bool isReady);
    
    void paint(juce::Graphics& g) override;
//    void mouseDoubleClick(const juce::MouseEvent& event) override;
//    void mouseDown(const juce::MouseEvent& event)override;
//    void mouseDrag(const juce::MouseEvent& event)override;
//    void mouseUp(const juce::MouseEvent& event)override;
    
    void visibilityChanged() override ;
    void parentHierarchyChanged() override ;
//    PortComponent* getPortComponent(PortComponent::Type type, size_t index) const;
    // Method to set the listener

    void layoutPorts();
    std::vector<PortComponent*> getPorts() const;
    PortComponent* getPortComponent(size_t index, PortComponent::Type type) ;
    
    
    void setSelected(bool select) ;
    bool getSelected() const;
    void requestDeletion() ;

    void textEditorFocusLost(CustomTextEditor* editor) override;
//    void textEditorMouseClicked(CustomTextEditor* editor, const juce::MouseEvent& event) override;
//    void textEditorTextChanged(CustomTextEditor* editor)override;
    virtual void adjustSizeToText() ;

    bool getIsUIBlockComponent() ;
    bool hitTest(int x, int y) override;

    // Method to adjust size based on text
    

protected:
   virtual void updateComponentState() ;
    ApplicationMode currentMode = ApplicationMode::Editing;
    
private:
    
    void loadBlockBasedOnInput();
    
    
    CustomTextEditor textEditor;
    
    juce::OwnedArray<PortComponent> inputPorts;
    juce::OwnedArray<PortComponent> outputPorts;
    // ... other members ...
    BlockListener* listener = nullptr; // Initialize to nullptr
    std::string lastText;
    juce::String creationText;
    
    bool isSelected = false;
//    bool isEditModeEnabled = false;
    bool isUIBlockComponent = false;
    
    
};
