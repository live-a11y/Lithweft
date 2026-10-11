/*
  ==============================================================================

    CustomTextEditor.h
    Created: 15 Nov 2023 8:20:11pm
    Author:  刘昊

  ==============================================================================
*/
#include <JuceHeader.h>

class CustomTextEditor;

class CustomTextEditorListener {
public:
    virtual ~CustomTextEditorListener() = default;
    virtual void textEditorFocusLost(CustomTextEditor* editor)=0;

    virtual void textEditorMouseClicked(CustomTextEditor* editor, const juce::MouseEvent& event){};
    virtual void textEditorTextChanged(CustomTextEditor* editor){} ; // New method

};

class CustomTextEditor : public juce::TextEditor {
public:
    CustomTextEditor() {
        setReadyForEditing(true); // Initialize with editing disabled
        setJustification(juce::Justification::centredLeft);

        
        // Set the background to a brighter color
        setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentWhite);

        // Set the outline to be transparent
        setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentWhite);

        // Set the text color and shadow color
        setColour(juce::TextEditor::textColourId, juce::Colours::black);
//        setColour(juce::TextEditor::shadowColourId, juce::Colours::transparentBlack);

        setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentWhite);

    }

    
    ~CustomTextEditor()
    {
        setReadyForEditing(false);
        
    }
    bool hitTest(int x, int y) override {
        
        return true;  // Always return false to ignore mouse events
    }
    
    void setReadyForEditing(bool ready) {
        std::cout<<"setReadyForEditing : "<<(ready ?"true":"false")<<std::endl;
        isReadyForEditing = ready;
        setMouseCursor(isReadyForEditing ? juce::MouseCursor::IBeamCursor : juce::MouseCursor::NormalCursor);
//        setMouseClickGrabsKeyboardFocus(ready);
        setReadOnly(!ready);
        
    }

    void setListener(CustomTextEditorListener* newListener) {
        listener = newListener;
    }

    void focusLost(FocusChangeType cause) override {
        juce::TextEditor::focusLost(cause);
        setReadyForEditing(false);
        
        if (listener) {
            listener->textEditorFocusLost(this);
        }
        
    }
    
    
    // Method to set the editor in performance mode
       void setPerformanceMode(bool isInPerformanceMode) {
           this->isInPerformanceMode = isInPerformanceMode;
       }

       // Override mouseDown for performance mode interaction
       void mouseDown(const juce::MouseEvent& event) override {
           if (listener && isInPerformanceMode) {
                  listener->textEditorMouseClicked(this, event);
              } else {
                  // Call the base class implementation for default behavior
                  juce::TextEditor::mouseDown(event);
              }
       }

    void changeBackgroundColor(juce::Colour newColor) {
          setColour(juce::TextEditor::backgroundColourId, newColor);
          repaint();
      }

      void resetBackgroundColor() {
          setColour(juce::TextEditor::backgroundColourId, defaultBackgroundColor);
          repaint();
      }

private:
    bool isReadyForEditing = false;
    bool isInPerformanceMode = false;  // Track performance mode state
    CustomTextEditorListener* listener = nullptr;
    juce::Colour defaultBackgroundColor = juce::Colours::transparentWhite;

};
