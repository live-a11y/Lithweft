#pragma once

#include <JuceHeader.h>
#include "PatchController.h"
#include "AudioConfigureComponent.h"
#include "Timer.h"
#include "PatchDocument.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

// Define the menu item IDs
enum MenuItems
{
    newPatchMenuItemID = 100, openPatchMenuItemID, savePatchMenuItemID, savePatchAsMenuItemID,
    audioOptionsMenuItemID = 1001,
    // ... other menu item IDs
};

class MainComponent  : public juce::AudioAppComponent ,public juce::MenuBarModel,public Timer::Listener, public juce::KeyListener, private juce::Timer

{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    bool keyPressed(const juce::KeyPress&) override;
    bool keyPressed(const juce::KeyPress&, juce::Component*) override;
    void requestClose(std::function<void()> onClose);
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    

    // MenuBarModel overrides
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex (int topLevelMenuIndex, const juce::String& menuName) override;
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;

    void onTimerTick()override;

private:
    //==============================================================================
    // Your private member variables go here...
    void shutdownAudio();

    PatchController patchController;
    PatchDocument patchDocument;
    bool documentOperationPending = false;
    bool handleDocumentShortcut(const juce::KeyPress&);
    void runDocumentCommand(int command);
    void timerCallback() override;
    void updateDocumentTitle();
//    double sampleRate = 44100.0;
    void showAudioConfig();
    
    // Your other member variables and methods...
    juce::AudioDeviceManager deviceManager;
    
    ::Timer timer;

    juce::TextEditor textTestEditor;
//    std::unique_ptr<AudioConfigComponent> audioConfigComponent;
//    juce::MenuBarComponent menuBar;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
