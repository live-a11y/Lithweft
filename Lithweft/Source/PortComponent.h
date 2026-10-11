/*
  ==============================================================================

    BlockComponent.h
    Created: 9 Nov 2023 10:36:08am
    Author: 刘昊

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>

#define PORT_WIDTH 16
#define PORT_HEIGHT 6

class BlockComponent;
//class PortComponent; // Forward declaration
//
//class PortComponentListener {
//public:
//    virtual ~PortComponentListener() = default;
//
//    virtual void portMouseDown(PortComponent* port, const juce::MouseEvent& event) = 0;
//    virtual void portMouseDrag(PortComponent* port, const juce::MouseEvent& event) = 0;
//    virtual void portMouseUp(PortComponent* port, const juce::MouseEvent& event) = 0;
//};



class PortComponent : public juce::Component {
    
public:
    enum class Type {
         Inlet,
         Outlet
     };
    
    


    bool hitTest(int x, int y) override {
        // Define an extended hit test area
        const int hitTestMargin = 10; // Adjust the margin size as needed
        juce::Rectangle<int> extendedBounds = getLocalBounds().expanded(hitTestMargin);

        // Check if the point is within this extended area
        return extendedBounds.contains(x, y);
    }

//    void setListener(PortComponentListener* newListener) {
//        listener = newListener;
//    }
    
    
    // Constructor
    PortComponent(Type portType,size_t index, BlockComponent* owner)
    : portType(portType),ownerBlock(owner), portIndex(index) {
            
    }
    
    void setHighlighted(bool highlight) {
        if (isHighlighted != highlight) {
            isHighlighted = highlight;
            repaint();  // Trigger a repaint to show/hide the highlight
        }
    }

    
    bool isOutlet() const{
        return  portType == Type::Outlet ? true:false;
    }

    bool isInlet() const{
        return  portType == Type::Inlet ? true:false;
    }
    
    void paint(juce::Graphics& g) override {
//        g.setColour(isHighlighted ? juce::Colours::lightgrey:juce::Colours::black);
//        g.fillRect(getLocalBounds());
//        
//        if (isHighlighted) {
////            g.setColour(juce::Colours::yellow); // Example highlight color
//            // Additional drawing code for highlighting
//        }

//        int reductionSize = 2; // The amount by which you want to reduce the rectangle's height
//
//            auto bounds = getLocalBounds();
//            juce::Rectangle<int> reducedBounds;
//
//            if (isInlet()) {
//                // For inlets, reduce from the top
//                reducedBounds = bounds.removeFromTop(bounds.getHeight() - reductionSize);
//            } else if (isOutlet()) {
//                // For outlets, reduce from the bottom
//                reducedBounds = bounds.removeFromBottom(bounds.getHeight() - reductionSize);
//            }
//
//            // Use the adjusted bounds for drawing
//            g.setColour(isHighlighted ? juce::Colours::white : juce::Colours::black);
//            g.fillRect(reducedBounds);
        
        
//        int reductionSize = 1; // The amount by which you want to reduce the rectangle's size on all sides
//
//           auto bounds = getLocalBounds();
//           // Apply reduction to both width and height
//           auto reducedBounds = bounds.reduced(reductionSize);
//
//           // If it's an outlet, you might want to adjust the position of the reduced rectangle
//           // For example, if you want to align the reduced rectangle to the bottom of the original bounds for outlets:
//           if (isInlet()) {
//               reducedBounds.setY(bounds.getBottom() - reducedBounds.getHeight() - reductionSize);
//               reducedBounds.setHeight(reducedBounds.getHeight() - reductionSize);
//           }
//
//           // Similarly, adjust for the inlet if necessary. For example:
//           if (isOutlet()) {
//               reducedBounds.setY(reducedBounds.getY() + reductionSize);
//               reducedBounds.setHeight(reducedBounds.getHeight() - reductionSize);
//           }
//
//           // Drawing the rectangle
//           g.setColour(isHighlighted ? juce::Colours::lightgrey : juce::Colours::black);
//           g.fillRect(reducedBounds);
        
//        
        int heightReductionSize = 2; // The amount by which you want to reduce the rectangle's height
          int widthReductionSize = 2;  // The amount by which you want to reduce the rectangle's width

          auto bounds = getLocalBounds();
          // Apply width reduction to the bounds
          auto reducedWidthBounds = bounds.reduced(widthReductionSize / 2, 0); // Reduce width from both sides

          juce::Rectangle<int> reducedBounds;

          if (isOutlet()) {
              // For inlets, reduce height from the top and adjust Y position
              reducedBounds = reducedWidthBounds.withTrimmedTop(heightReductionSize).withTrimmedBottom(heightReductionSize / 2);
          } else if (isInlet()) {
              // For outlets, reduce height from the bottom and adjust Y position
              reducedBounds = reducedWidthBounds.withTrimmedBottom(heightReductionSize).withTrimmedTop(heightReductionSize / 2);
          }

          // Drawing the rectangle with adjusted bounds
          g.setColour(isHighlighted ? juce::Colours::white : juce::Colours::black);
          g.fillRect(reducedBounds);
//        
        
//        int heightReductionSize = 2; // The amount by which you want to reduce the rectangle's height
//           int widthReductionSize = 2;  // The amount by which you want to reduce the rectangle's width
//
//           auto bounds = getLocalBounds();
//           // Apply width reduction to the bounds
//           auto reducedWidthBounds = bounds.reduced(widthReductionSize / 2, 0); // Reduce width from both sides
//
//           juce::Rectangle<int> reducedBounds;
//
//           if (isOutlet()) {
//               // For outlets, reduce height from the top and adjust Y position
//               reducedBounds = reducedWidthBounds.withTrimmedTop(heightReductionSize).withTrimmedBottom(heightReductionSize / 2);
//           } else if (isInlet()) {
//               // For inlets, reduce height from the bottom and adjust Y position
//               reducedBounds = reducedWidthBounds.withTrimmedBottom(heightReductionSize).withTrimmedTop(heightReductionSize / 2);
//           }
//
//           // Fill color based on highlight state
//           if (isHighlighted) {
//               // Fill the rectangle with white
//               g.setColour(juce::Colours::white);
//               g.fillRect(reducedBounds);
//               // Draw a black border around the rectangle
//               g.setColour(juce::Colours::black);
//               g.drawRect(reducedBounds, 1); // Adjust the thickness as needed
//           } else {
//               // If not highlighted, fill with black (or any other default color)
//               g.setColour(juce::Colours::black);
//               g.fillRect(reducedBounds);
//           }
       
        
    }

//    void mouseDown(const juce::MouseEvent& event) override {
//        if (listener) {
//            listener->portMouseDown(this, event);
//        }
//    }
//
//    void mouseDrag(const juce::MouseEvent& event) override{
//        if (listener) {
//            listener->portMouseDrag(this, event);
//        }
//    }
//    void mouseUp(const juce::MouseEvent& event) override {
//        // Perform any port-specific logic needed on mouse up
//        if (listener) {
//            listener->portMouseUp(this, event);
//        }
//    }
    
    size_t getIndex() const {
        return portIndex;
    }

    Type getType() const {
        return portType;
    }

    BlockComponent* getBlockOwner()
    {
        return ownerBlock;
    }
    

    juce::Rectangle<int> getBoundsRelativeTo(juce::Component* relativeToComponent) {
        if (relativeToComponent == nullptr) {
            return juce::Rectangle<int>(); // Return an empty rectangle if the relative component is null
        }

        // Get the bounds of the PortComponent relative to its owner BlockComponent
        auto portBounds = getBounds();

        // Convert these bounds to the coordinate space of the given component
        auto boundsRelativeTo = ((juce::Component*)ownerBlock)->localAreaToGlobal(portBounds);
        boundsRelativeTo = relativeToComponent->getLocalArea(nullptr, boundsRelativeTo);

        return boundsRelativeTo;
    }

    
    juce::Point<int> getCentreRelativeTo(const juce::Component* relativeToComponent) {
        if (relativeToComponent == nullptr || ownerBlock == nullptr) {
            return {-1, -1}; // Return an invalid point if parameters are null
        }

        // Get the bounds of the PortComponent relative to its owner BlockComponent
        auto portBounds = getBounds();

        // Convert these bounds to the coordinate space of the given component
        auto boundsRelativeTo = ((juce::Component*)ownerBlock)->localAreaToGlobal(portBounds);
        boundsRelativeTo = relativeToComponent->getLocalArea(nullptr, boundsRelativeTo);

        // Calculate and return the centre point
        return boundsRelativeTo.getCentre();
    }


    juce::Point<int> getCentreGlobal() const {
        auto centreLocal = getBounds().getCentre();
//        std::cout<<"getCentreGlobal :"<<localPointToGlobal(centreLocal).toString()<<std::endl;
        return localPointToGlobal(centreLocal);
    }

    
private:
    Type portType; // Type of the port (Inlet/Outlet)
    BlockComponent* ownerBlock; // Pointer to the owning BlockComponent
    size_t portIndex; // Index of this port
//    bool isZoomed = false; // Flag to indicate if the port is zoomed
    bool isHighlighted = false;

//    bool isConnecting = false;
//    PortComponentListener* listener = nullptr;

};
