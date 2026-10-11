/*
  ==============================================================================

    Patch.cpp
    Created: 9 Nov 2023 10:28:49am
    Author:  刘昊

  ==============================================================================
*/

#include "PatchController.h"
#include "BlockInterface.h"
#include "Slider.h"
#include "NumberBox.h"
#include "Scope.h"
#include "ParameterBox.h"
#include "Trigger.h"
#include "StringToBlockDataConverter.h"
#include "BufferManager_Tilde.h"
// PatchController.cpp
#include <dlfcn.h> // Make sure to include this
#include "../../BlockMaker/LithweftBlocks/BlockLibraryPaths.h"


typedef enum : int {
    dylibBlock,
    uiBlock,
    
} BlockType;
PatchController::PatchController():blockChain(new BlockChain)
{
    // Constructor implementation
    setWantsKeyboardFocus(true); // Allow this component to receive keyboard focus


}

PatchController::~PatchController() {

    clearPatch();
    blockChain.reset();
}

bool PatchController::keyPressed(const juce::KeyPress& key)
{
    if (key.getTextCharacter() == 'n') {
        createEmptyBlock();
        
        return true; // Key was handled
    }
    
    if (key.getTextCharacter() == 't') {
        
        // Remove the old block from blockMapping
        std::cout << "--------Block Mappings:" << std::endl;
        for (const auto& pair : blockMapping) {
            const BlockComponent* blockComponent = pair.first;
            const BlockInterface* blockInterface = pair.second;
            
            std::cout << "BlockComponent: " << blockComponent
            << " associated with BlockInterface: " << blockInterface << std::endl;
            
        }
        
        blockChain->onTest();
        
        repaint();
        resized();
        
        return true; // Key was handled
    }
    
    // Check for Command + E or Ctrl + E for mode switching
    if ((key.getModifiers().isCommandDown() || key.getModifiers().isCtrlDown()) &&
        (key.getKeyCode() == 101 || key.getKeyCode() == 69)) { // ASCII values for 'e' and 'E'
        toggleMode(); // Toggle between Editing and Performance modes
        return true; // Key was handled
    }
    
    if (key.getKeyCode() == juce::KeyPress::backspaceKey) {
        if(currentMode == ApplicationMode::Editing){
            deleteSelectedLines();
            deleteSelectedComponents();
            
            return true; // Indicates that the key press was handled
        }
     }

       // Handle other key presses
        return false; // Key was not handled
    
}

void PatchController::deleteSelectedComponents() {
    // Get all components
    auto allComponents = getAllBlockComponents();

    // Iterate through each component and check if it is selected
    for (auto* component : allComponents) {
        std::cout<<"component :"<<component<<std::endl;
        if (component->getSelected()) {
            // If the component is selected, request its deletion
            std::cout<<"component :"<<component<<" is selected"<<std::endl;

            component->requestDeletion();
        }
    }
}


void PatchController::requestDeleteBlock(BlockComponent* component) {
    const juce::ScopedLock lock(graphLock);
    if (component == nullptr) return;
    auto* instance = getBlockInterfaceForComponent(component);
    const bool isUI = component->getIsUIBlockComponent();
    for (auto* line : lineComponents) delete line;
    lineComponents.clear();
    removeChildComponent(component);
    blockMapping.erase(component);
    if (instance != nullptr) blockChain->removeBlock(instance);
    if (!isUI || instance == nullptr) delete component;
    selectedBlockComponent = nullptr;
    startPort = nullptr;
    initialPositions.clear();
    updateConnectionLines();
    repaint();
}

//void PatchController::mouseDoubleClick(const juce::MouseEvent& event){
//    
//    if(currentMode == ApplicationMode::Editing){
//        createEmptyBlock();
//    }
//    
//}

void PatchController::mouseDoubleClick(const juce::MouseEvent& event) {
    // Try to find a block component under the mouse
    BlockComponent* clickedComponent = findBlockComponentUnderMouse(event.getPosition());

    if (clickedComponent) {
        // If a block component is found, trigger editing for that component
        clickedComponent->setEditorReadyForEditing(true);
    } else {
        // If no component is found, it's a double-click on a blank area
        if (currentMode == ApplicationMode::Editing) {
            createEmptyBlock();
        }
    }
}


    
void PatchController::createEmptyBlock() {
    // Create a BlockComponent and set the BlockSystem as its listener
         auto* blockComponent = new BlockComponent();
         blockComponent->setListener(this); // Assuming BlockComponent has a setListener method
         addAndMakeVisible(blockComponent);
    
        // Set the block position to the mouse position
        auto mousePos = getMouseXYRelative();
        blockComponent->setBounds(mousePos.getX()-BLOCK_WIDTH/2, mousePos.getY()-BLOCK_HEIGHT/2, BLOCK_WIDTH, BLOCK_HEIGHT); // You can set your own size
        
}

//=======================block delegate=======================
void PatchController::requestCreateBlock(BlockComponent* requested, const std::string& blockName,
                                         const std::string& args, bool) {
    const juce::ScopedLock lock(graphLock);
    lastCreatedComponent = nullptr;
    if (requested == nullptr) return;
    const auto position = requested->getPosition();
    const auto expression = juce::String(blockName) + (args.empty() ? juce::String() : " " + juce::String(args));
    const auto actualName = resolveBlockName(blockName);
    std::unique_ptr<BlockInterface> instance;
    if (actualName == "slider") instance.reset(new Slider());
    else if (actualName == "number") instance.reset(new NumberBox());
    else if (actualName == "scope~") instance.reset(new Scope());
    else if (actualName == "box") instance.reset(new ParameterBox());
    else if (actualName == "trigger") instance.reset(new Trigger());
    else if (actualName == "bufferManager~") instance.reset(new BufferManager_Tilde());
    else {
        const auto executable = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
        const auto library = BlockLibraryPaths::find(actualName, executable.getFullPathName().toStdString());
        if (!library.empty()) instance.reset(loadAndCreateBlockInstanceFromLib(actualName, library.parent_path().string() + "/"));
        else std::cerr << "Module not found: " << actualName << ". Build the modules or set LITHWEFT_BLOCKS_DIR." << std::endl;
    }
    if (instance == nullptr) {
        requested->notifyCreationStatus(false);
        return;
    }
    auto* component = dynamic_cast<BlockComponent*>(instance.get());
    std::unique_ptr<BlockComponent> regularComponent;
    const bool isUI = component != nullptr;
    if (!isUI) {
        regularComponent = std::make_unique<BlockComponent>();
        component = regularComponent.get();
    }
    instance->init(StringBlockDataProcessor::processStringInput(args), blockChain->systemMessageCallback, blockChain->outletCallback);
    if (instance->handlesAudioProcessing()) instance->configureAudioParameters(numSamples, sampleRate);
    component->setCreationText(expression);
    component->setup(instance->getNumberOfInputs(), instance->getNumberOfOutputs(), position, isUI);
    if (!isUI) component->adjustSizeToText();
    component->layoutPorts();
    component->switchMode(currentMode);
    component->setListener(this);

    // The chain owns UI blocks as well as runtime-only blocks. Detach the old
    // component before replacing its runtime, which may destroy that component.
    auto* newInstance = instance.get();
    const bool oldIsUI = requested->getIsUIBlockComponent();
    auto* oldInstance = getBlockInterfaceForComponent(requested);
    for (auto* line : lineComponents) delete line;
    lineComponents.clear();
    removeChildComponent(requested);
    blockMapping.erase(requested);
    if (oldInstance != nullptr) blockChain->replaceBlock(oldInstance, instance.release());
    else blockChain->addBlock(instance.release());
    if (!oldIsUI || oldInstance == nullptr) delete requested;
    addAndMakeVisible(component);
    associateBlockWithInterface(component, newInstance);
    lastCreatedComponent = component;
    regularComponent.release();
    updateConnectionLines();
    repaint();
}

void PatchController::parentHierarchyChanged()  {
    if (isShowing()) {
        grabKeyboardFocus();
    }
}

void PatchController::resized() {
    // Implementation for handling resize events
}


BlockInterface* PatchController::loadAndCreateBlockInstanceFromLib(
    const std::string& libBaseName,
    const std::string& path)
{

    // Append the '.dylib' extension to the base name
    std::string fullLibName = libBaseName + ".dylib";

    // Construct the full path to the dynamic library
    std::string fullLibPath = path + fullLibName;
    
    
    // Load the dynamic library
    void* libHandle = dlopen(fullLibPath.c_str(), RTLD_LAZY);
    if (!libHandle) {
        std::cerr << "Failed to open dynamic library: " << dlerror() << std::endl;
        return nullptr;
    }

    // Retrieve the factory function from the library
    auto createInstance = (CreateBlockInstanceFunction)dlsym(libHandle, "createBlockInstance");
    if (!createInstance) {
        std::cerr << "Failed to find factory function 'createBlockInstance': " << dlerror() << std::endl;
        dlclose(libHandle);
        return nullptr;
    }

    // Use the factory function to create an instance of the block
    BlockInterface* blockInstance = createInstance();
    if (!blockInstance) {
        std::cerr << "Failed to create block instance." << std::endl;
        dlclose(libHandle);
        return nullptr;
    }

    // The dynamic library handle (`libHandle`) should be managed to prevent a memory leak.
    // You might want to store `libHandle` somewhere to call `dlclose(libHandle)` when the block is no longer needed.

    // Return the created block instance
    libraryHandles.push_back(libHandle);
    return blockInstance;
}



void PatchController::paint(juce::Graphics& g) {
    g.fillAll(backgroundColor); // Fill the background with the current color

    // Set up text attributes
    g.setColour(juce::Colours::black); // Text color
    g.setFont(juce::Font(16.0f)); // Font size

    // Draw the mode text
    auto modeText = (currentMode == ApplicationMode::Editing) ? "Edit Mode" : "Performance Mode";
    g.drawText(modeText, getLocalBounds(), juce::Justification::bottomLeft, true);

    if(currentMode == (ApplicationMode::Editing)){
        drawGrid(g);
    }

    // Draw temporary connection line if dragging from a port
    if (isDragging && startPort) {
        auto start = startPort->getCentreRelativeTo(this);
        auto end = currentDragPosition; // Current mouse position

        // Calculate control points for the curve
        juce::Point<int> controlPoint1(start.getX(), (start.getY() + end.getY()) / 2);
        juce::Point<int> controlPoint2(end.getX(), (start.getY() + end.getY()) / 2);

        // Create a Path to draw the curve
        juce::Path curvePath;
        curvePath.startNewSubPath(start.getX(), start.getY());
        curvePath.cubicTo(controlPoint1.getX(), controlPoint1.getY(),
                          controlPoint2.getX(), controlPoint2.getY(),
                          end.getX(), end.getY());

        // Set the color for the line
        g.setColour(juce::Colours::black);

        // Draw the curve
        g.strokePath(curvePath, juce::PathStrokeType(2.0f));
    }

    // Draw selection rectangle if selecting
    if (isSelecting) {
        juce::Rectangle<int> selectionArea = juce::Rectangle<int>::leftTopRightBottom(
            selectionStartPoint.getX(), selectionStartPoint.getY(),
            selectionEndPoint.getX(), selectionEndPoint.getY()
        );
        g.setColour(juce::Colours::lightgrey.withAlpha(0.3f));
        g.fillRect(selectionArea);
    }
}


void PatchController::drawGrid(juce::Graphics& g) {
    auto area = getLocalBounds();
    const int gridSize = 50; // Increased grid size for better readability of numbers

    g.setColour(juce::Colours::grey); // Light grey color for the grid lines

    // Drawing grid lines and numbers
    for (int x = 0; x < area.getWidth(); x += gridSize) {
        g.drawLine(x, 0, x, area.getHeight());

        // Draw horizontal numbers
        g.setColour(juce::Colours::black);
        g.drawText(juce::String(x), x + 2, 0, 40, 20, juce::Justification::left);
    }

    for (int y = 0; y < area.getHeight(); y += gridSize) {
        g.drawLine(0, y, area.getWidth(), y);

        // Draw vertical numbers
        if (y != 0) { // Skip drawing at the very top to avoid overlap with horizontal numbers
            g.setColour(juce::Colours::black);
            g.drawText(juce::String(y), 2, y + 2, 40, 20, juce::Justification::left);
        }
    }
}

void PatchController::createConnection(PortComponent* startPort, PortComponent* endPort) {
    const juce::ScopedLock lock(graphLock);
    if (startPort && endPort) {
        // Find the associated BlockComponent for each port
        auto* sourceBlockComponent = dynamic_cast<BlockComponent*>(startPort->getParentComponent());
        auto* destinationBlockComponent = dynamic_cast<BlockComponent*>(endPort->getParentComponent());

        if (sourceBlockComponent && destinationBlockComponent) {
            // Get the associated BlockInterfaces using the mapping
            auto* sourceBlockInterface = getBlockInterfaceForComponent(sourceBlockComponent);
            auto* destinationBlockInterface = getBlockInterfaceForComponent(destinationBlockComponent);

            if (sourceBlockInterface && destinationBlockInterface) {
                // Get port indices
                size_t outletIndex = startPort->getIndex();
                size_t inletIndex = endPort->getIndex();

                // Connect blocks in the blockchain
                blockChain->connectBlocks(sourceBlockInterface, outletIndex, destinationBlockInterface, inletIndex);
                
                updateConnectionLines();

               repaint();  // Up

            }
        }
    }
}


//============================================================================

void PatchController::timerCallback()  {
    const juce::ScopedTryLock lock(graphLock);
    if (!lock.isLocked()) return;
    
    blockChain->tick();  // Call tick method on blockchain instance
}
//=============================================================================


void PatchController::mouseDown(const juce::MouseEvent& event) {
    if (currentMode == ApplicationMode::Editing) {
        bool clickedOnComponent = false;
        bool shiftKeyPressed = event.mods.isShiftDown();  // Check if Shift key is pressed


        PortComponent* portUnderMouse = findPortUnderMouse(event.getPosition());
         if (portUnderMouse && portUnderMouse->isOutlet()) {
             startPort = portUnderMouse;
             isDragging = true;
             currentDragPosition = event.getEventRelativeTo(this).getPosition();
             return; // Exit as port interaction takes precedence
         }

        LineComponent* lineUnderMouse = findLineUnderMouse(event.getPosition());

            if (lineUnderMouse) {
                if (shiftKeyPressed) {
                    lineUnderMouse->setSelected(!lineUnderMouse->getSelected()); // Toggle selection state
                } else if (!lineUnderMouse->getSelected()) {
                    deselectAllBlockComponents();
                    deselectAllLines();
                    lineUnderMouse->setSelected(true);
                }
                clickedOnComponent = true;
                return;
            }
    
        // Check for a click on a BlockComponent
        if (!clickedOnComponent) {
            auto blockComponent = findBlockComponentUnderMouse(event.getPosition());
            if (blockComponent) {
                clickedOnComponent = true;

                if (shiftKeyPressed) {
                    // Toggle selection state
                    blockComponent->setSelected(!blockComponent->getSelected());
                } else {
                    if (!blockComponent->getSelected()) {
                        // If not already selected, select this component and deselect others
                        deselectAllBlockComponents();
                        deselectAllLines();
                        blockComponent->setSelected(true);
                    } else {
                        // If already selected, set the text ready for editing
                        blockComponent->setEditorReadyForEditing(true);
                    }
                }

                handleBlockSelection(blockComponent, event.getPosition());
                isDraggingSelectedComponents = true;
                dragStartPoint = event.getPosition();
                saveInitialPositions();
                return;
            }
        }

        // If clicked on a blank area, handle selecting/deselecting
        if (!clickedOnComponent) {
            if (!shiftKeyPressed) {
                deselectAllBlockComponents();
                deselectAllLines();
            }
            selectionEndPoint = selectionStartPoint = event.getPosition();
            isSelecting = true;
        }
    }
    repaint();
}



bool PatchController::anyOtherComponentSelected(BlockComponent* currentComponent) {
    for (auto* component : getAllBlockComponents()) {
        if (component != currentComponent && component->getSelected()) {
            return true;
        }
    }
    return false;
}


void PatchController::mouseDrag(const juce::MouseEvent& event) {
    
    if (currentMode == ApplicationMode::Editing) {
        
        if (isDragging && startPort) {
            // Handle dragging from a port
            currentDragPosition = event.getPosition();
            //        portMouseDrag(startPort, event);  // Update dragging line
            if (startPort && startPort->isOutlet()) { // Assuming isOutlet() tells whether the port is an outlet
                
                auto startBounds = startPort->getBounds();
                juce::Point<int> startCentre(startBounds.getX() + startBounds.getWidth() / 2,
                                             startBounds.getY() + startBounds.getHeight() / 2);
                
                currentDragPosition = event.getEventRelativeTo(this).getPosition();
                //        std::cout<<"currentDragPosition : "<<currentDragPosition.toString()<<std::endl;
                
            }
            repaint();
        }
        
        if (isDraggingSelectedComponents) {
            auto offset = event.getPosition() - dragStartPoint;
            moveSelectedComponents(offset);
        } else if (isSelecting) {
            selectionEndPoint = event.getPosition();
            repaint();
        }
        
        // Check if the mouse is over any port and highlight it
         auto hoveredPort = findPortUnderMouse(event.getPosition());
         highlightPort(hoveredPort);
    }
}



void PatchController::mouseUp(const juce::MouseEvent& event) {
    if (currentMode == ApplicationMode::Editing) {
        if (isDragging && startPort) {
            // Handle releasing the mouse on a port
            PortComponent* endPort = findPortUnderMouse(event.getPosition());
            if (endPort) {
                //            portMouseUp(startPort, event);  // Finalize the connection
                if (isDragging && startPort) {
                    // Use the current mouse position to find the end port
                    auto mousePosition = event.getEventRelativeTo(this).getPosition();
                    PortComponent* endPort = findPortUnderMouse(mousePosition);

                    if (endPort && endPort->isInlet() && endPort != startPort) {
                        createConnection(startPort, endPort);
                        //                std::cout<<"connected Ports : from :"<<startPort<<" to :"<<endPort<<std::endl;

                    }

                    // Reset the dragging state and the flag
                    isDragging = false;
                    startPort = nullptr;
                }

            }
            isDragging = false;
            startPort = nullptr;
            repaint();
        }
        else if (isDraggingSelectedComponents) {
            isDraggingSelectedComponents = false; // End dragging
            
        } else if (isSelecting) {
            selectComponentsInArea(event.mods.isShiftDown()); // Handle area selection with shift key state
            selectLinesInArea(); // Existing logic for selecting lines
            isSelecting = false;
            repaint();
        }
        repaint();
    }
}

void PatchController::mouseMove(const juce::MouseEvent& event) {
    
    if (currentMode == ApplicationMode::Editing) {
        
        // Find the port under the mouse
        auto* portUnderMouse = findPortUnderMouse(event.getPosition());
        
        // Use the highlightPort method to manage highlighting
        highlightPort(portUnderMouse);
        
        repaint(); // Repaint to update the UI with the new state
    }
}

//=======================================================================================================

void PatchController::deselectAllBlockComponents() {
    // Iterate over all BlockComponents and deselect them
    for (auto* blockComponent : getAllBlockComponents()) {
        blockComponent->setSelected(false);
    }
    // Perform any additional actions needed after deselecting components

}

void PatchController::selectComponentsInArea(bool shiftKeyPressed) {
    juce::Rectangle<int> selectionRect = juce::Rectangle<int>::leftTopRightBottom(
        std::min(selectionStartPoint.getX(), selectionEndPoint.getX()),
        std::min(selectionStartPoint.getY(), selectionEndPoint.getY()),
        std::max(selectionStartPoint.getX(), selectionEndPoint.getX()),
        std::max(selectionStartPoint.getY(), selectionEndPoint.getY())
    );

    for (auto* component : getAllBlockComponents()) {
        if (selectionRect.intersects(component->getBounds())) {
            // If Shift is pressed, add new components to the current selection
            // Otherwise, select only the new components
            if (!shiftKeyPressed) {
                component->setSelected(false);
            }
            component->setSelected(true);
        }
    }

    // Similar logic for line components if needed
    // ...
}



void PatchController::saveInitialPositions() {
    initialPositions.clear();
    for (auto* component : getAllBlockComponents()) {
        if (component->getSelected()) {
            initialPositions[component] = component->getPosition();
        }
    }
}


void PatchController::moveSelectedComponents(const juce::Point<int>& offset) {
    for (auto& pair : initialPositions) {
        auto* component = pair.first;
        auto initialPosition = pair.second;
        component->setTopLeftPosition(initialPosition + offset);
    }
}


//-----------
void PatchController::lineSelectionHandler(LineComponent* line, const juce::ModifierKeys& mods) {
    if (mods.isShiftDown()) {
        line->setSelected(!line->getSelected()); // Toggle selection
    } else {
        deselectAllLines();
        line->setSelected(true); // Select the clicked line
    }
}




void PatchController::handleBlockSelection(BlockComponent* blockComponent, const juce::Point<int>& position) {
    

    
    if (blockComponent->getSelected()) {

        //handle drag things
        isDraggingSelectedComponents = true;
        dragStartPoint = position;
        saveInitialPositions();
    }
    
}

void PatchController::selectComponent(BlockComponent* component) {
    if (component) {
        // if there is a second-click on the component then enable editing
//        std::cout<<"selectComponent :"<<component<<"is selcted :"<<(component->getSelected() ?"true" :"false")<<std::endl;
        
        if(component->getSelected()){
            component->setEditorReadyForEditing(true);
        }
        component->setSelected(true);
        // You may need additional logic here, depending on how you handle selection
    }
}


void PatchController::deselectAllLines() {
    for (auto* line : lineComponents) {
        line->setSelected(false);
    }
    repaint();
}

// Method to delete selected lines
void PatchController::deleteSelectedLines() {
    // Loop through lines and delete the selected ones
    for (std::vector<LineComponent*>::size_type i = lineComponents.size(); i-- > 0; ) {
        if (lineComponents[i]->getSelected()) {
            // Call the disconnect logic for the line
            disconnect(lineComponents[i]->getStartPort(), lineComponents[i]->getEndPort());

            // Remove the line component from the UI
            removeChildComponent(lineComponents[i]);
            delete lineComponents[i];
            lineComponents.erase(lineComponents.begin() + i);
        }
    }

    repaint(); // Repaint to update the UI
}

void PatchController::updateConnectionLines() {
    const juce::ScopedLock lock(graphLock);
    // Clear existing lines
    for (auto* line : lineComponents) {
        removeChildComponent(line);
        delete line;
    }
    lineComponents.clear();

    // Create new lines based on blockchain connections
    auto connections = blockChain->getConnections();
    for (const auto& connection : connections) {
        BlockComponent* sourceBlockComponent = findComponentForInterface(connection.sourceBlock);
        BlockComponent* destinationBlockComponent = findComponentForInterface(connection.destinationBlock);

        if (sourceBlockComponent && destinationBlockComponent) {
            PortComponent* sourcePort = sourceBlockComponent->getPortComponent(connection.outletIndex, PortComponent::Type::Outlet);
            PortComponent* destinationPort = destinationBlockComponent->getPortComponent(connection.inletIndex, PortComponent::Type::Inlet);

            if (sourcePort && destinationPort) {
                auto* line = new LineComponent(sourcePort, destinationPort, this);
                addAndMakeVisible(line);
                line->toBack(); // Ensure the line is drawn behind ports
                lineComponents.push_back(line);
               // Ensure ports are brought to the front
                sourcePort->toFront(false);
                destinationPort->toFront(false);
                
            }
        }
    }
}
// Method to disconnect the line logically
void PatchController::disconnect(PortComponent* startPort, PortComponent* endPort) {
    const juce::ScopedLock lock(graphLock);
    if (!startPort || !endPort) return;

    // Retrieve the blocks associated with the ports
    BlockComponent* startBlockComponent = dynamic_cast<BlockComponent*>(startPort->getBlockOwner());
    BlockComponent* endBlockComponent = dynamic_cast<BlockComponent*>(endPort->getBlockOwner());

    if (!startBlockComponent || !endBlockComponent) return;

    // Find the BlockInterface instances corresponding to the BlockComponent instances
    BlockInterface* sourceBlock = blockMapping[startBlockComponent];
    BlockInterface* destinationBlock = blockMapping[endBlockComponent];

    if (!sourceBlock || !destinationBlock) return;

    // Get the indexes of the ports in their respective blocks
    size_t outletIndex = startPort->getIndex();
    size_t inletIndex = endPort->getIndex();

    // Call the disconnectBlocks method on the blockChain
    blockChain->disconnectBlocks(sourceBlock, outletIndex, destinationBlock, inletIndex);
}



// Method to toggle between Editing and Performance modes
void PatchController::toggleMode() {

    currentMode = (currentMode == ApplicationMode::Editing) ? ApplicationMode::Performance : ApplicationMode::Editing;

    // Update the background color based on the mode
    backgroundColor = (currentMode == ApplicationMode::Editing) ? juce::Colours::grey : juce::Colours::grey;
    repaint();  // Trigger a repaint to update the background

    
    // Update all BlockComponents with the new mode
    for (auto* blockComponent : getAllBlockComponents()) {
        blockComponent->switchMode(currentMode);
        
    }
    
    //
    if(currentMode == ApplicationMode::Performance){
        deselectAllBlockComponents();
        deselectAllLines(); // Deselect all lines
        selectionEndPoint = selectionStartPoint ;
        isSelecting = false;
    }
    
}





//=================================================================================================
void PatchController::highlightPort(PortComponent* port) {
    if (port != nullptr) {
        // Highlight the port
        port->setHighlighted(true);
    } else {
        // Reset highlighting for all ports
        for (auto* block : getAllBlockComponents()) {
            for (auto* p : block->getPorts()) {
                p->setHighlighted(false);
            }
        }
    }
}


void PatchController::selectLinesInArea() {
    juce::Rectangle<int> selectionRect = juce::Rectangle<int>::leftTopRightBottom(
        std::min(selectionStartPoint.getX(), selectionEndPoint.getX()),
        std::min(selectionStartPoint.getY(), selectionEndPoint.getY()),
        std::max(selectionStartPoint.getX(), selectionEndPoint.getX()),
        std::max(selectionStartPoint.getY(), selectionEndPoint.getY())
    );


    for (auto* line : lineComponents) {
        // Transform hit areas from LineComponent to PatchController coordinates
        for (const auto& localHitArea : line->getHitTestAreas()) {

            
            auto globalHitArea = line->localAreaToGlobal(localHitArea);
            
            auto patchControllerHitArea = this->getLocalArea(nullptr, globalHitArea);
            
            if (selectionRect.intersects(patchControllerHitArea.toType<int>())) {
                line->setSelected(true);
                break; // No need to check further if one hit area intersects
            }
        }
    }

    repaint(); // Repaint to update the visual state of the lines
}



LineComponent* PatchController::findLineUnderMouse(const juce::Point<int>& position) {
    for (auto* line : lineComponents) {
        juce::Point<int> localPoint = line->getLocalPoint(this, position);
        if (line->isPointOverLine(localPoint)) {
            return line; // Return the line component if the mouse is over it
        }
    }
    return nullptr; // No line component found under the mouse
}



BlockComponent* PatchController::findBlockComponentUnderMouse(const juce::Point<int>& position) {
    for (auto* block : getAllBlockComponents()) {
        // Get the bounds of the block component relative to the PatchController
        auto blockBounds = block->getBounds();

        if (blockBounds.contains(position)) {
            return block; // Return the block component if the mouse is over it
        }
    }
    return nullptr; // No block component found under the mouse
}


PortComponent* PatchController::findPortUnderMouse(const juce::Point<int>& position) {
    for (const auto& entry : blockMapping) {
        auto* blockComponent = entry.first; // This is a BlockComponent

        if (blockComponent) {
            auto portComponents = blockComponent->getPorts(); // Assuming getPorts() exists in BlockComponent
            for (auto* portComponent : portComponents) {
                // Calculate expanded bounds for hit-testing
                                
                auto expandedBounds = portComponent->getBoundsRelativeTo(this).expanded(0, 5); // Expand as needed
                int yOffset = 0;
                if(portComponent->getType() == PortComponent::Type::Outlet){
                    // Offset the Y position
                    yOffset = 5; // Adjust this value as needed
                }
                if(portComponent->getType() == PortComponent::Type::Inlet){
                    // Offset the Y position
                    yOffset = -5; // Adjust this value as needed
                }
                expandedBounds.setY(expandedBounds.getY() + yOffset);
                
                
                if (expandedBounds.contains(position)) {
                    return portComponent;
                }
            }
        }
    }
    
    return nullptr; // No port found under the mouse
}


//===========================================================================================================================
void PatchController::associateBlockWithInterface(BlockComponent* blockComponent, BlockInterface* blockInterface) {
    blockMapping[blockComponent] = blockInterface;
}

BlockInterface* PatchController::getBlockInterfaceForComponent(BlockComponent* blockComponent) {
    auto it = blockMapping.find(blockComponent);
    if (it != blockMapping.end()) {
        return it->second; // Return the associated BlockInterface
    }
    return nullptr; // BlockComponent not found in the mapping
}


BlockComponent* PatchController::findComponentForInterface(BlockInterface* blockInterface) {
    auto it = std::find_if(blockMapping.begin(), blockMapping.end(),
                           [blockInterface](const auto& pair) {
                               return pair.second == blockInterface;
                           });

    if (it != blockMapping.end()) {
        return it->first; // Return the BlockComponent
    }
    return nullptr; // BlockInterface not found
}



std::vector<BlockComponent*> PatchController::getAllBlockComponents() {
    std::vector<BlockComponent*> blockComponents;

    // Iterate through the child components of PatchController
    for (int i = 0; i < this->getNumChildComponents(); ++i) {
        auto* child = this->getChildComponent(i);
        BlockComponent* blockComponent = dynamic_cast<BlockComponent*>(child);
        if (blockComponent) {
            blockComponents.push_back(blockComponent);
        }
    }

    return blockComponents;
}


std::string PatchController::resolveBlockName(const std::string& alias) {

    auto it = aliasMapping.find(alias);
    return (it != aliasMapping.end()) ? it->second : alias;
}

//========================================================
BlockData PatchController::processStringAsBlockData(const std::string& str) {
    // Process the string as in ParameterBox
    // If it's a quoted string, return as Message
    if (isQuotedString(str)) {
        return Message(str.substr(1, str.length() - 2)); // Remove quotes
    }
    // If it's a number, return as Number
    else if (isStrictNumber(str)) {
        return Number(std::stod(str));
    }
    // If it's a comma-separated list, split and create a List of BlockData
    else {
//        return processStringToList(str);
        return Message(str);

    }
}

ListElement PatchController::processStringAsListElement(const std::string& str) {
    if (isQuotedString(str)) {
        // Remove quotes and return as Message
        return Message(str.substr(1, str.length() - 2));
    }
    else if (isStrictNumber(str)) {
        // Convert to Number and return
        return Number(std::stod(str));
    }
    else {
        // If it's not a quoted string or a number, treat it as a Message
        return Message(str);
//        return processStringToList(str);

    }
}



List PatchController::processStringToList(const std::string& str) {
    std::istringstream iss(str);
    std::string item;
    List list;

    while (iss >> item) {
        if (isStrictNumber(item)) {
            list.push_back(Number(std::stod(item)));
        } else {
            list.push_back(Message(item));
        }
    }

    return list;
}

bool PatchController::isQuotedString(const std::string& str) {
    return str.size() >= 2 && str.front() == '"' && str.back() == '"';
}

bool PatchController::isStrictNumber(const std::string& str) {
    // Implement the logic to check if the string is a strict number
    // Similar to ParameterBox implementation
    // ...
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
//========================================================================
void PatchController::prepareToProcess(int numSamples, double sampleRate) {
    const juce::ScopedLock lock(graphLock);

    this->numSamples = numSamples;
    this->sampleRate = sampleRate;
    std::cout<<"sampleRate : "<<sampleRate<<std::endl;
    isAudioSetting = true;
    blockChain->prepareToProcess(numSamples, sampleRate);
    isAudioSetting = false;
}


void PatchController::processAudioStream(std::vector<AudioSignal>& audioBuffer, int numOutputChannels) {
    const juce::ScopedTryLock lock(graphLock);
    if (!lock.isLocked()) return;
    if(!isAudioSetting){
        blockChain->processAudioStream(audioBuffer,numOutputChannels);
    }

}
