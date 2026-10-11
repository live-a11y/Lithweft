///*
//  ==============================================================================
//
//    AudioConfigureComponent.cpp
//    Created: 22 Nov 2023 3:44:55pm
//    Author:  刘昊
//
//  ==============================================================================
//*/
//
//
//#include "AudioConfigureComponent.h"
//
//
//AudioConfigComponent::AudioConfigComponent(juce::AudioDeviceManager& deviceManager) : deviceManager(deviceManager) {
//    // Initialize UI components
//    deviceManager.createAudioDeviceTypes(availableDeviceTypes);
//
//    addAndMakeVisible(driverLabel);
//    driverLabel.setText("Driver", juce::dontSendNotification);
//
//    addAndMakeVisible(inputDeviceLabel);
//    inputDeviceLabel.setText("Input Device", juce::dontSendNotification);
//
//    addAndMakeVisible(outputDeviceLabel);
//    outputDeviceLabel.setText("Output Device", juce::dontSendNotification);
//
//    addAndMakeVisible(samplingRateLabel);
//    samplingRateLabel.setText("Sample Rate", juce::dontSendNotification);
//    
//    addAndMakeVisible(ioVectorSizeLabel);
//    ioVectorSizeLabel.setText("ioVectorSize", juce::dontSendNotification);
//    
//    addAndMakeVisible(driverSelectBox);
//    addAndMakeVisible(inputDeviceSelectBox);
//    addAndMakeVisible(outputDeviceSelectBox);
//    addAndMakeVisible(samplingRateSelectBox);
//    addAndMakeVisible(ioVectorSizeBox);
//
//    addAndMakeVisible(audioDriverSetupButton);
//    addAndMakeVisible(ioMappingsButton);
//
//    // Register as a listener
//    driverSelectBox.addListener(this);
//    inputDeviceSelectBox.addListener(this);
//    outputDeviceSelectBox.addListener(this);
//    samplingRateSelectBox.addListener(this);
//    ioVectorSizeBox.addListener(this);
//
//    audioDriverSetupButton.addListener(this);
//    ioMappingsButton.addListener(this);
//
//    // Load current settings to populate UI
//    loadCurrentSettings();
//}
//
//AudioConfigComponent::~AudioConfigComponent() {
//    
//    // Remove listeners
//    driverSelectBox.removeListener(this);
//    inputDeviceSelectBox.removeListener(this);
//    outputDeviceSelectBox.removeListener(this);
//    samplingRateSelectBox.removeListener(this);
//    ioVectorSizeBox.removeListener(this);
//
//    audioDriverSetupButton.removeListener(this);
//    ioMappingsButton.removeListener(this);
//}
//
//
//
//
//
//void AudioConfigComponent::paint(juce::Graphics& g) {
//    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // Clear the background
//    // Additional painting code (if needed)
//}
//
//void AudioConfigComponent::resized() {
//    // Position the components
//    auto area = getLocalBounds();
//
//    int componentHeight = 30;
//    int margin_x = 20;
//    int margin_y = 20;
//    
//    int margin_x_second = 60;
//
//    
//    
//    driverLabel.setBounds(margin_x,
//                          margin_y,
//                          getWidth()/2-margin_x_second,
//                          componentHeight);
//    
//    driverSelectBox.setBounds(margin_x+getWidth()/2-margin_x_second,
//                              margin_y,
//                              getWidth()/2-20,
//                              componentHeight);
//
//    
//    inputDeviceLabel.setBounds(margin_x,
//                               componentHeight+40+margin_y ,
//                               getWidth()/2-margin_x_second,
//                               componentHeight);
//    
//    inputDeviceSelectBox.setBounds(margin_x+getWidth()/2-margin_x_second,
//                                   componentHeight+40+margin_y,
//                                   getWidth()/2-20,
//                                   componentHeight);
//
//
//    outputDeviceLabel.setBounds(margin_x,
//                                componentHeight+40+20+40 ,
//                                getWidth()/2-margin_x_second, 
//                                componentHeight);
//    
//    outputDeviceSelectBox.setBounds(margin_x+getWidth()/2-margin_x_second,
//                                    componentHeight+40+margin_y+40 ,
//                                    getWidth()/2-margin_x_second,
//                                    componentHeight);
//      
//    
//    samplingRateLabel.setBounds(margin_x,
//                                componentHeight+40+margin_y+40+40 ,
//                                getWidth()/2-margin_x_second,
//                                componentHeight);
//    samplingRateSelectBox.setBounds(margin_x+getWidth()/2-margin_x_second,
//                                    componentHeight+40+margin_y+40+40 ,
//                                    getWidth()/2-margin_x_second,
//                                    componentHeight);
//    
//    
//    ioVectorSizeLabel.setBounds(margin_x,
//                                componentHeight+40+margin_y+40+40+40 ,
//                                getWidth()/2-margin_x_second,
//                                componentHeight);
//    
//    ioVectorSizeBox.setBounds(margin_x+getWidth()/2-margin_x_second,
//                              componentHeight+40+margin_y+40+40+40 ,
//                              getWidth()/2-margin_x_second,
//                              componentHeight);
//    
//}
//
//void AudioConfigComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
//    // Handle changes in combo boxes
//    if (comboBoxThatHasChanged == &driverSelectBox) {
//        // Change the driver
//    }
//    // ... similar for other combo boxes
//
//    applyChanges();
//}
//
//void AudioConfigComponent::buttonClicked(juce::Button* button) {
//    if (button == &audioDriverSetupButton) {
//        // Open audio driver setup
//    } else if (button == &ioMappingsButton) {
//        // Open I/O mappings
//    }
//}
//
//
//void AudioConfigComponent::updateDeviceList() {
//    auto currentDeviceType = deviceManager.getCurrentAudioDeviceType();
//    
//    driverSelectBox.clear();
//    inputDeviceSelectBox.clear();
//    outputDeviceSelectBox.clear();
//    samplingRateSelectBox.clear();
//    ioVectorSizeBox.clear();
//
//    juce::OwnedArray<juce::AudioIODeviceType> availableDeviceTypes;
//    deviceManager.createAudioDeviceTypes(availableDeviceTypes);
//
//    for (auto& type : availableDeviceTypes) {
//        driverSelectBox.addItem(type->getTypeName(), driverSelectBox.getNumItems() + 1);
//    }
//
//    int selectedTypeIndex = findIndexOfItemWithText(driverSelectBox, currentDeviceType);
//    if (selectedTypeIndex != -1) {
//        driverSelectBox.setSelectedItemIndex(selectedTypeIndex, juce::dontSendNotification);
//        
//        // Update input/output device boxes and sample rates/buffer sizes based on the selected device type
//        updateInputOutputDeviceBoxes();
//        updateSampleRatesAndBufferSizes();
//    }
//}
//
//void AudioConfigComponent::updateInputOutputDeviceBoxes() {
//    auto currentDeviceTypeName = deviceManager.getCurrentAudioDeviceType();
//    for (auto& deviceType : availableDeviceTypes) {
//        if (deviceType->getTypeName() == currentDeviceTypeName) {
//            deviceType->scanForDevices(); // Refresh device list
//            auto device = deviceType->createDevice(deviceType->getDeviceNames()[0], "");
//            if (device) {
//                inputDeviceSelectBox.clear();
//                outputDeviceSelectBox.clear();
//                for (const auto& name : device->getInputChannelNames()) {
//                    inputDeviceSelectBox.addItem(name, inputDeviceSelectBox.getNumItems() + 1);
//                }
//                for (const auto& name : device->getOutputChannelNames()) {
//                    outputDeviceSelectBox.addItem(name, outputDeviceSelectBox.getNumItems() + 1);
//                }
//            }
//            break;
//        }
//    }
//}
//
//void AudioConfigComponent::updateSampleRatesAndBufferSizes() {
//    auto currentDeviceTypeName = deviceManager.getCurrentAudioDeviceType();
//    for (auto& deviceType : availableDeviceTypes) {
//        if (deviceType->getTypeName() == currentDeviceTypeName) {
//            deviceType->scanForDevices(); // Refresh device list
//            auto device = deviceType->createDevice(deviceType->getDeviceNames()[0], "");
//            if (device) {
//                samplingRateSelectBox.clear();
//                ioVectorSizeBox.clear();
//                for (const auto& rate : device->getAvailableSampleRates()) {
//                    samplingRateSelectBox.addItem(juce::String(rate) + " Hz", samplingRateSelectBox.getNumItems() + 1);
//                }
//                for (const auto& size : device->getAvailableBufferSizes()) {
//                    ioVectorSizeBox.addItem(juce::String(size), ioVectorSizeBox.getNumItems() + 1);
//                }
//            }
//            break;
//        }
//    }
//}
//
//// Helper function to find the index of an item by text
//int AudioConfigComponent::findIndexOfItemWithText(const juce::ComboBox& comboBox, const juce::String& text) {
//    for (int i = 0; i < comboBox.getNumItems(); ++i) {
//        if (comboBox.getItemText(i) == text) {
//            return i;
//        }
//    }
//    return -1; // Not found
//}
//
//
//void AudioConfigComponent::updateIOVectorSizeList() {
//    // Depending on the platform and audio device, this might not be applicable or might need a different approach.
//    // You would need to query the current device for supported buffer sizes, and fill in the ioVectorSizeBox accordingly.
//}
//
//void AudioConfigComponent::loadCurrentSettings() {
//    updateDeviceList();
//
//        // Assuming that the sample rates and buffer sizes are already added to their respective ComboBoxes.
//        auto* currentDevice = deviceManager.getCurrentAudioDevice();
//        if (currentDevice) {
//            // Find the current sample rate in the ComboBox and set it as selected.
//            auto currentSampleRate = juce::String(currentDevice->getCurrentSampleRate());
//            for (int i = 0; i < samplingRateSelectBox.getNumItems(); ++i) {
//                if (samplingRateSelectBox.getItemText(i) == currentSampleRate) {
//                    samplingRateSelectBox.setSelectedItemIndex(i);
//                    break;
//                }
//            }
//
//            // Find the current buffer size in the ComboBox and set it as selected.
//            auto currentBufferSize = juce::String(currentDevice->getCurrentBufferSizeSamples());
//            for (int i = 0; i < ioVectorSizeBox.getNumItems(); ++i) {
//                if (ioVectorSizeBox.getItemText(i) == currentBufferSize) {
//                    ioVectorSizeBox.setSelectedItemIndex(i);
//                    break;
//                }
//            }
//
//            // ... Set other settings like input/output device selection
//        }
//}
//
//void AudioConfigComponent::applyChanges() {
//    // This method would take the settings from your combo boxes and apply them to the device manager.
//    // For example:
//    juce::AudioDeviceManager::AudioDeviceSetup setup;
//    deviceManager.getAudioDeviceSetup(setup);
//
//    setup.sampleRate = samplingRateSelectBox.getText().getDoubleValue();
//    setup.bufferSize = ioVectorSizeBox.getText().getIntValue();
//    
//    // ... you would also set input and output devices here ...
//
//    auto error = deviceManager.setAudioDeviceSetup(setup, true);
//       if (error.isNotEmpty()) {
//           juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
//                                                  "Buffer Size Error",
//                                                  "Failed to set buffer size: " + error,
//                                                  "OK");       }
//}
//
//
