#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
:patchController(),patchDocument(patchController),timer(1)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    // Set up the menu bar
    
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
        
        
    }

    addAndMakeVisible(patchController);
    
    addToDesktop(juce::ComponentPeer::windowAppearsOnTaskbar);
    juce::MenuBarModel::setMacMainMenu(this); // Set up the macOS menu bar

    timer.addListener(this);
    timer.start();
    addKeyListener(this);
    startTimer(300);

}


MainComponent::~MainComponent()
{
    stopTimer();
    timer.stop();
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    juce::MenuBarModel::setMacMainMenu(nullptr); // Clear the macOS menu bar
    
}

//==============================================================================
void MainComponent::onTimerTick(){
    
    
    patchController.timerCallback();
}

//
juce::StringArray MainComponent::getMenuBarNames()
{
    return {"File", "Edit", "Options", "Help"};
}

juce::PopupMenu MainComponent::getMenuForIndex(int, const juce::String& menuName) {
    juce::PopupMenu menu;
    if (menuName == "File") {
        menu.addItem(newPatchMenuItemID, juce::String::fromUTF8("New Patch    ⌘N"), !documentOperationPending);
        menu.addItem(openPatchMenuItemID, juce::String::fromUTF8("Open Patch…    ⌘O"), !documentOperationPending);
        menu.addSeparator();
        menu.addItem(savePatchMenuItemID, juce::String::fromUTF8("Save    ⌘S"), !documentOperationPending);
        menu.addItem(savePatchAsMenuItemID, juce::String::fromUTF8("Save As…    ⇧⌘S"), !documentOperationPending);
    } else if (menuName == "Options") {
        menu.addItem(audioOptionsMenuItemID, "Audio Setup");
    }
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int) {
    if (menuItemID == audioOptionsMenuItemID) showAudioConfig();
    else runDocumentCommand(menuItemID);
}

void MainComponent::timerCallback() {
    patchDocument.checkForChanges();
    updateDocumentTitle();
}

void MainComponent::updateDocumentTitle() {
    if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
        if (window->getName() != patchDocument.title()) window->setName(patchDocument.title());
}

bool MainComponent::handleDocumentShortcut(const juce::KeyPress& key) {
    if (!key.getModifiers().isCommandDown() && !key.getModifiers().isCtrlDown()) return false;
    const auto code = juce::CharacterFunctions::toLowerCase(static_cast<juce::juce_wchar>(key.getKeyCode()));
    if (code == 's') runDocumentCommand(key.getModifiers().isShiftDown() ? savePatchAsMenuItemID : savePatchMenuItemID);
    else if (code == 'o') runDocumentCommand(openPatchMenuItemID);
    else if (code == 'n') runDocumentCommand(newPatchMenuItemID);
    else return false;
    return true;
}

void MainComponent::runDocumentCommand(int command) {
    if (documentOperationPending) return;
    if (command < newPatchMenuItemID || command > savePatchAsMenuItemID) return;
    patchController.grabKeyboardFocus();
    patchDocument.checkForChanges();
    documentOperationPending = true;
    const auto safe = juce::Component::SafePointer<MainComponent>(this);
    auto completed = [safe](juce::FileBasedDocument::SaveResult) {
        if (safe != nullptr) { safe->documentOperationPending = false; safe->updateDocumentTitle(); }
    };
    if (command == savePatchMenuItemID) patchDocument.saveAsync(true, true, completed);
    else if (command == savePatchAsMenuItemID) patchDocument.saveAsInteractiveAsync(true, completed);
    else {
        patchDocument.saveIfNeededAndUserAgreesAsync([safe, command](juce::FileBasedDocument::SaveResult result) {
            if (safe == nullptr) return;
            if (result != juce::FileBasedDocument::savedOk) { safe->documentOperationPending = false; return; }
            if (command == newPatchMenuItemID) {
                safe->patchDocument.reset();
                safe->documentOperationPending = false;
                safe->updateDocumentTitle();
            } else {
                safe->patchDocument.loadFromUserSpecifiedFileAsync(true, [safe](juce::Result) {
                    if (safe != nullptr) { safe->documentOperationPending = false; safe->updateDocumentTitle(); }
                });
            }
        });
    }
}

void MainComponent::requestClose(std::function<void()> onClose) {
    if (documentOperationPending) return;
    patchController.grabKeyboardFocus();
    patchDocument.checkForChanges();
    documentOperationPending = true;
    const auto safe = juce::Component::SafePointer<MainComponent>(this);
    patchDocument.saveIfNeededAndUserAgreesAsync([safe, onClose](juce::FileBasedDocument::SaveResult result) {
        if (safe == nullptr) return;
        safe->documentOperationPending = false;
        if (result == juce::FileBasedDocument::savedOk) onClose();
    });
}

void MainComponent::showAudioConfig()
{
    // Create the dialog window with the audio config component
    auto* configComponent = new AudioConfigureComponent(deviceManager);
    configComponent->setSize(340, 460); // Ensure the component size is set correctly

    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(configComponent);
    options.dialogTitle = "Audio Configuration";
    options.dialogBackgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = false; // You can change this if needed

    // Ensure the window size is set to match the component size
    auto* window = options.create();
    window->centreWithSize(configComponent->getWidth(), configComponent->getHeight());
    window->setVisible(true);
    
    resized();

}



bool MainComponent::keyPressed(const juce::KeyPress& key) { return handleDocumentShortcut(key); }
bool MainComponent::keyPressed(const juce::KeyPress& key, juce::Component*) { return handleDocumentShortcut(key); }




//======================================================================================

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    patchController.prepareToProcess(samplesPerBlockExpected, sampleRate);

}


void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    std::vector<AudioSignal> audioBuffer(bufferToFill.buffer->getNumChannels(), AudioSignal(bufferToFill.numSamples));
    patchController.processAudioStream(audioBuffer, bufferToFill.buffer->getNumChannels());

    for (int channel = 0; channel < static_cast<int>(audioBuffer.size()); ++channel) {
        std::copy(audioBuffer[channel].begin(), audioBuffer[channel].end(), bufferToFill.buffer->getWritePointer(channel));
    }
}




void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.fillAll(juce::Colours::darkgrey); // Background color

    // You can add your drawing code here!
    patchController.paint(g);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    
    patchController.setBounds(getLocalBounds());
    
}


void MainComponent::shutdownAudio()
{
    // This will be called when the application is about to close.

    // Shut down the audio device
    deviceManager.closeAudioDevice();

    // Call the base class method to shut down the audio thread
    AudioAppComponent::shutdownAudio();

    // You can also perform any additional cleanup here if needed
}
