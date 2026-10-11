/*
  ==============================================================================

    Patch.h
    Created: 9 Nov 2023 10:28:49am
    Author:  刘昊

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "BlockComponent.h"
#include "PortComponent.h"
#include "BlockInterface.h"
#include "BlockChain.h"
#include "LineComponent.h"
class PatchController : public juce::Component,
                        public BlockListener // Assuming BlockListener is an interface you define
{
public:
    // Constructor
    PatchController();

    // Destructor
    virtual ~PatchController();

//    void requestCreateBlock(BlockComponent* emptyBlock, const std::string& blockName, const std::vector<std::string>& args,bool shouldReplace) override;

//    void requestCreateBlock(BlockComponent* emptyBlock, const std::string& blockName, const std::vector<std::string>& args,bool shouldReplace) override;
    void requestCreateBlock(BlockComponent* emptyBlock, const std::string& blockName, const std::string& args,bool shouldReplace) override;

    juce::var getPatchState();
    juce::Result restorePatch(const juce::var& state);
    juce::Result savePatch(const juce::File& file);
    juce::Result loadPatch(const juce::File& file);
    void clearPatch();

    // GUI Component overrides
    void paint(juce::Graphics& g) override;
    void drawConnectionLine(juce::Graphics& g, PortComponent* sourcePort, PortComponent* destinationPort);
    void drawGrid(juce::Graphics& g) ;

    void resized() override;
    bool keyPressed(const juce::KeyPress&) override;
    void parentHierarchyChanged() override;
    void toggleMode() ;

    //--------------------------
    void mouseDoubleClick(const juce::MouseEvent& event)override;

    void mouseDown(const juce::MouseEvent& event)override;
    void mouseDrag(const juce::MouseEvent& event)override;
    void mouseUp(const juce::MouseEvent& event)override;
    void mouseMove(const juce::MouseEvent& event) override;
    //----------------------------
    void deselectAllBlockComponents();
    void deleteSelectedComponents();
    void deleteSelectedLines();

    void lineSelectionHandler(LineComponent* line, const juce::ModifierKeys& mods);
    void handleBlockSelection(BlockComponent* blockComponent, const juce::Point<int>& position);
    void deselectAllLines();
    void selectLinesInArea();
    
    void selectComponentsInArea(bool shiftKeyPressed);

    void saveInitialPositions();
    void moveSelectedComponents(const juce::Point<int>& offset) ;
    void selectComponent(BlockComponent* component);
    bool anyOtherComponentSelected(BlockComponent* currentComponent);

    
    void highlightPort(PortComponent* port);
    //---------------------------------------------
    //----------------------------------------
    void associateBlockWithInterface( BlockComponent* blockComponent,BlockInterface* blockInterface);
    BlockInterface* getBlockInterfaceForComponent(BlockComponent* blockComponent);
    
    void createConnection(PortComponent* startPort, PortComponent* endPort);
    void disconnect(PortComponent* startPort, PortComponent* endPort);

    void updateConnectionLines();

    std::vector<BlockComponent*> getAllBlockComponents() ;
    BlockComponent* findComponentForInterface(BlockInterface* blockInterface);
    LineComponent* findLineUnderMouse(const juce::Point<int>& position);
    BlockComponent* findBlockComponentUnderMouse(const juce::Point<int>& position);
    PortComponent* findPortUnderMouse(const juce::Point<int>& position);

    //-------------------------------------------------------
    // Call this from BlockComponent when it's about to be deleted
 
    void requestDeleteBlock(BlockComponent* blockComponent)override;

    std::string resolveBlockName(const std::string& alias) ;

    void timerCallback() ;
    void processAudioStream(std::vector<AudioSignal>& audioBuffer, int numOutputChannels);
    void prepareToProcess(int numSamples, double sampleRate);

    
private:
    
    BlockInterface* loadAndCreateBlockInstanceFromLib(
        const std::string& libBaseName,
        const std::string& path);
    
    std::map<BlockComponent*,BlockInterface* > blockMapping;
    
    void createEmptyBlock();
    std::vector<void*> libraryHandles;
    juce::CriticalSection graphLock;
    BlockComponent* lastCreatedComponent = nullptr;

    bool isAudioSetting = false;

    PortComponent* startPort = nullptr;
    juce::Point<int> currentDragPosition;
    bool isDragging = false;
    
//    juce::Point<int> getGlobalCentre(PortComponent* port) ;
    std::vector<LineComponent*> lineComponents;
    
    std::unique_ptr<BlockChain> blockChain;

    bool checkIftheClassNameIsAUIBlock( const std::string& value) {
        return std::find(uiBlockClassNames.begin(), uiBlockClassNames.end(), value) != uiBlockClassNames.end();
    }
    
    // Method to get all BlockComponents (assuming you have a way to do this)

    ApplicationMode currentMode = ApplicationMode::Editing;
    juce::Colour backgroundColor = juce::Colours::grey;
    BlockComponent* selectedBlockComponent = nullptr;

    
    //=======================
    juce::Point<int> selectionStartPoint;
    juce::Point<int> selectionEndPoint;
    bool isSelecting = false;
    
    bool isDraggingSelectedComponents = false;
    juce::Point<int> dragStartPoint;
    std::map<BlockComponent*, juce::Point<int>> initialPositions;
    

    
    int numSamples = 512;
    double sampleRate = 44100.0;

    
    std::vector<std::string> uiBlockClassNames = {"slider","number","scope~","box","trigger","bufferManager~"};
    std::unordered_map<std::string, std::string> aliasMapping = {
        {"*", "multiple"},
        {"+", "add"},
        {"/","divide"},
        {"-","subtract"},
        {"*~", "multiple_tilde"},
        // Add more aliases here
    };
    
    
    //===========================
    BlockData processStringAsBlockData(const std::string& str);
    List processStringToList(const std::string& str);
    bool isQuotedString(const std::string& str);
    bool isStrictNumber(const std::string& str);
    ListElement processStringAsListElement(const std::string& str);

    
};
