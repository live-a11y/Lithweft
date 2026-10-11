/*
  ==============================================================================

    BlockChain.cpp
    Created: 9 Nov 2023 8:49:44pm
    Author:  刘昊

  ==============================================================================
*/


#include "BlockChain.h"
#include <iostream>
#include <dlfcn.h> // Make sure to include this
#include <variant>
#include "StringToBlockDataConverter.h"
BlockChain::BlockChain(){
    
}
BlockChain::~BlockChain() {
    // Clean up blocks
    for (auto& block : blocks) {
        delete block; // Manually delete the block
    }
    blocks.clear();
}
void BlockChain::onTest() {
    
    for (auto& block : blocks) {
//        block.reset();   This will call the destructor of each block if it exists
//        std::cout<<"the block interface inside block chain: "<<block<<std::endl;
    }

    std::cout << "==========get all connections==========" << std::endl;

    
    std::cout << "Current Connections:" << std::endl;

    
    std::vector<Connection> connections = getConnections();

    // Iterate over all connections
    for (const auto& conn : connections) {
        std::cout << "Source Block: " << conn.sourceBlock
                  << " [Outlet " << conn.outletIndex << "] --> "
                  << "Destination Block: " << conn.destinationBlock
                  << " [Inlet " << conn.inletIndex << "]" << std::endl;
    }
    
    std::cout << "========== ==========" << std::endl;
}





void BlockChain::addBlock(BlockInterface* block) {
    blocks.push_back(block);
}


void BlockChain::removeBlock(BlockInterface* blockToRemove) {
    // Loop through the blocks vector to find and remove the block
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i] == blockToRemove) {
//            std::cout<<"blockChain will delete the blockInterface: "<<blocks[i]<<std::endl;
            delete blocks[i]; // Delete the block
            blocks.erase(blocks.begin() + i); // Erase it from the vector
            break; // Exit the loop once the block is found and removed
        }
    }

    // Remove all connections where blockToRemove is the source block
    connections.erase(blockToRemove);

    // Iterate over all other blocks and remove connections where blockToRemove is the destination
    for (auto& [sourceBlock, connVector] : connections) {
        connVector.erase(
            std::remove_if(
                connVector.begin(), connVector.end(),
                [blockToRemove](const Connection& conn) {
                    return conn.destinationBlock == blockToRemove;
                }
            ),
            connVector.end()
        );
    }
}



void BlockChain::replaceBlock(BlockInterface* oldBlock, BlockInterface* newBlock) {
    // Step 1: Replace the block in the blocks vector
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i] == oldBlock) {
            blocks[i] = newBlock; // Replace the old block with the new one
            break; // Exit the loop once the block is found and replaced
        }
    }

    // Step 2: Update connections and remove invalid ones
    for (auto& [sourceBlock, connVector] : connections) {
        auto it = connVector.begin();
        while (it != connVector.end()) {
            bool removeConnection = false;
            Connection& conn = *it;

            // Update the source block of the connection
            if (conn.sourceBlock == oldBlock) {
                conn.sourceBlock = newBlock;
                removeConnection = conn.outletIndex >= newBlock->getNumberOfOutputs();
            }

            // Update the destination block of the connection
            if (conn.destinationBlock == oldBlock) {
                conn.destinationBlock = newBlock;
                removeConnection |= conn.inletIndex >= newBlock->getNumberOfInputs();
            }

            // Remove the connection if it is no longer valid
            if (removeConnection) {
                it = connVector.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Update the connections map for the new block
    if (connections.find(oldBlock) != connections.end()) {
        connections[newBlock] = std::move(connections[oldBlock]);
        connections.erase(oldBlock);
    }

    // Step 4: Cleanup old block
    delete oldBlock;
}




//void BlockChain::connectBlocks(BlockInterface* sourceBlock, size_t outletIndex,
//                                BlockInterface* destinationBlock, size_t inletIndex) {
//    
//    std::cout<<"connectBlocks source:"<<sourceBlock<<" outletIndex : "<<outletIndex<<" destinationBlock:"<<destinationBlock<<"  inletIndex:"<<inletIndex<<std::endl;
//    
//    
//    if (sourceBlock->handlesAudioProcessing() && destinationBlock->handlesAudioProcessing()) {
//            // Create an audio connection
//            Connection conn = {sourceBlock, destinationBlock, inletIndex, outletIndex, true};
//            connections[sourceBlock].push_back(conn);
//        } else {
//            // Handle non-audio connections or throw an error
//            
//            // Validate indices before creating connections
//            if (sourceBlock == nullptr || destinationBlock == nullptr) {
//                std::cerr << "Cannot create connection: source or destination block is null." << std::endl;
//                return;
//            }
//            if (outletIndex >= sourceBlock->getNumberOfOutputs() || inletIndex >= destinationBlock->getNumberOfInputs()) {
//                std::cerr << "Cannot create connection: invalid outlet or inlet index." << std::endl;
//                return;
//            }
//
//            // Store the connection information
//            connections[sourceBlock].emplace_back(Connection{sourceBlock, destinationBlock, inletIndex, outletIndex});
//            std::cout << "Connection created from block " << sourceBlock
//                      << " outlet " << outletIndex << " to block " << destinationBlock
//                      << " inlet " << inletIndex << std::endl;
//        }
//    
//    
//}

void BlockChain::connectBlocks(BlockInterface* sourceBlock, size_t outletIndex,
                               BlockInterface* destinationBlock, size_t inletIndex) {
    
    std::cout << "connectBlocks source:" << sourceBlock << " outletIndex: " << outletIndex
              << " destinationBlock:" << destinationBlock << " inletIndex: " << inletIndex << std::endl;
    
    // Validate the source and destination blocks
    if (sourceBlock == nullptr || destinationBlock == nullptr) {
        std::cerr << "Cannot create connection: source or destination block is null." << std::endl;
        return;
    }

    // Validate the outlet and inlet indices
    if (outletIndex >= sourceBlock->getNumberOfOutputs() || inletIndex >= destinationBlock->getNumberOfInputs()) {
        std::cerr << "Cannot create connection: invalid outlet or inlet index." << std::endl;
        return;
    }

    // Create a new connection object for comparison
    Connection newConnection{sourceBlock, destinationBlock, inletIndex, outletIndex};

    // Check if the connection already exists
    auto& existingConnections = connections[sourceBlock];
    if (std::find(existingConnections.begin(), existingConnections.end(), newConnection) != existingConnections.end()) {
        std::cout << "Connection already exists." << std::endl;
        return;
    }

    // Store the new connection information
    existingConnections.push_back(newConnection);
    std::cout << "Connection created from block " << sourceBlock
              << " outlet " << outletIndex << " to block " << destinationBlock
              << " inlet " << inletIndex << std::endl;

    // Additional logic for handling audio connections, if necessary
    if (sourceBlock->handlesAudioProcessing() && destinationBlock->handlesAudioProcessing()) {
        // Handle the creation of an audio connection
        newConnection.isAudioConnection = true;
        // Additional steps...
    }
}






// Method to disconnect one block's outlet from another block's inlet
void BlockChain::disconnectBlocks(BlockInterface* sourceBlock, size_t outletIndex,BlockInterface* destinationBlock, size_t inletIndex) {
    // If the source block has connections, proceed to remove the specific one
    if (connections.count(sourceBlock)) {
        // Use std::remove_if to remove the connection from the vector
        auto& connVector = connections[sourceBlock];
        connVector.erase(
            std::remove_if(
                connVector.begin(),
                connVector.end(),
                [destinationBlock, inletIndex, outletIndex](const Connection& conn) {
                    return conn.destinationBlock == destinationBlock &&
                           conn.inletIndex == inletIndex &&
                           conn.outletIndex == outletIndex;
                }
            ),
            connVector.end()
        );

        // If there are no more connections for the source block, remove the entry from the map
        if (connVector.empty()) {
            connections.erase(sourceBlock);
        }
    }
}


std::vector<Connection> BlockChain::getConnections() const {
    std::vector<Connection> connectionsList;

    // Iterate over all connections and copy them into the vector
    for (const auto& [sourceBlock, connectionSet] : connections) {
        for (const auto& connection : connectionSet) {
            connectionsList.push_back(connection);
        }
    }

    return connectionsList;
}

void BlockChain::handleSystemMessage(BlockInterface* sender, const std::string& operation, const BlockData& data) {

//void BlockChain::handleSystemMessage(const std::string& operation, const BlockData& data) {

//    if (operation.rfind("send", 0) == 0) {
//        std::string channelName = operation.substr(4); // Extract channel name
//        std::cout<<"send channel name:"<<channelName<<std::endl;
//        
//        for (auto* block : blocks) {
//            // Use the query parameter to request the specific information you need
//            BlockData infoData = block->getInfo("getReceiveChannelName");
//
//            if (std::holds_alternative<Message>(infoData) && std::get<Message>(infoData) == channelName) {
//                // If the channel name matches, handle the data
//                block->setInput(-1, data); // Use the special channel for data reception
// 
//            }
//        }
//
//    } 
    
    if (operation.rfind("send", 0) == 0) {
            // Use the sender to directly obtain the channel name it's sending on
            BlockData senderChannelData = sender->getInfo("getSendChannelName");
            std::string channelName;
            
            // Extract the channel name from the sender's information
            if (std::holds_alternative<Message>(senderChannelData)) {
                channelName = std::get<Message>(senderChannelData);
                std::cout << "send channel name from sender: " << channelName << std::endl;
            } else {
                // Handle error or unexpected data type
                std::cerr << "Error: Sender did not provide a valid channel name." << std::endl;
                return; // Exit the function if no valid channel name is provided
            }

            // Find and notify all blocks subscribed to this channel
            for (auto* block : blocks) {
                BlockData blockChannelData = block->getInfo("getReceiveChannelName");
//                StringBlockDataProcessor::printBlockData(blockChannelData);
                if (std::holds_alternative<Message>(blockChannelData) && std::get<Message>(blockChannelData) == channelName) {
                    // If the channel name matches, handle the data
                    block->setInput(-1, data); // Use the special channel for data reception
                }
            }
        }
    else if (operation == "print") {
        // Handle print operation
        if (std::holds_alternative<Message>(data)) {
            std::cout << std::get<Message>(data) << std::endl;
        }
    }else if (operation == "requestSharedBuffer") {
        std::string requestedBufferName = std::get_if<Message>(&data) ? std::get<Message>(data) : "";
        AudioSignal sharedBufferData;

        // Step 1: Identify all blocks with the shared buffer and retrieve the shared buffer data
        bool bufferFound = false;
        for (auto* block : blocks) {
            BlockData bufferNameData = block->getInfo("getBufferName");

            if (std::holds_alternative<Message>(bufferNameData) && std::get<Message>(bufferNameData) == requestedBufferName) {
                if (!bufferFound) { // Retrieve the shared buffer data once
                    BlockData sharedBufferDataVar = block->getInfo("getSharedBuffer");
                    if (std::holds_alternative<AudioSignal>(sharedBufferDataVar)) {
                        sharedBufferData = std::get<AudioSignal>(sharedBufferDataVar);
                        
                        
                        std::cout << "----------------sharedBufferData has been found in any buffer block : ";
                        for (size_t i = 0; i < sharedBufferData.size() && i < 10; ++i) {
                            std::cout << sharedBufferData[i] << " ";
                        }
                        std::cout << std::endl;

                        bufferFound = true;
                    }
                }
            }
        }

        // Step 2: If the shared buffer is found, set it directly to the sender
            if (bufferFound) {
                // Assuming the sender is requesting the shared buffer
                sender->setInput(-1, sharedBufferData);
                std::cout << "Shared buffer '" << requestedBufferName << "' set to sender.\n";
            } else {
                // Log or handle the case where the shared buffer is not found
                std::cout << "Requested shared buffer '" << requestedBufferName << "' not found.\n";
            }
    }else if (operation == "bufferUpdated") {
        std::string updatedBufferName = std::get_if<Message>(&data) ? std::get<Message>(data) : "";
        
        // Find the buffer block that sent the update
        AudioSignal updatedBufferData;
        for (auto* block : blocks) {
            BlockData bufferNameData = block->getInfo("getBufferName");
            if (std::holds_alternative<Message>(bufferNameData) && std::get<Message>(bufferNameData) == updatedBufferName) {
                BlockData bufferDataVar = block->getInfo("getSharedBuffer");
                if (std::holds_alternative<AudioSignal>(bufferDataVar)) {
                    updatedBufferData = std::get<AudioSignal>(bufferDataVar);
                    break; // Found the updated buffer data
                }
            }
        }
        
        // Distribute the updated buffer data to all relevant blocks
        for (auto* block : blocks) {
            BlockData needsBufferData = block->getInfo("needsBuffer");
            if (std::holds_alternative<Message>(needsBufferData) && std::get<Message>(needsBufferData) == updatedBufferName) {
                block->setInput(-1, updatedBufferData);
            }
        }
    }



}



void BlockChain::handleBlockOutlet(BlockInterface* block, size_t outletIndex, const BlockData& value) {
    // Handle the outlet event, such as updating connections or passing data
    // For example, update UI components or pass the value to other parts of the system
    
    
//    std::cout<<"block triggered outlet: "<< block;
//    std::cout<<"   outlet :" <<outletIndex<<"   value: ";
//    
//    
//    // Use std::visit with a generic lambda to print the value
//    std::visit([](const auto& val) {
//        using T = std::decay_t<decltype(val)>;
//        if constexpr (std::is_same_v<T, Uninitialized>) {
//            std::cout << "Uninitialized" << std::endl;
//        } else if constexpr (std::is_same_v<T, Number>) {
//            std::cout << "Number: " << val << std::endl;
//        } else if constexpr (std::is_same_v<T, Message>) {
//            std::cout << "Message: " << val << std::endl;
//        } else if constexpr (std::is_same_v<T, List>) {
//            std::cout << "List: ";
//            for (const auto& elem : val) {
//                // Use std::visit with a visitor for ListElement
//                std::visit([](const auto& innerVal) {
//                    using InnerT = std::decay_t<decltype(innerVal)>;
//                    if constexpr (std::is_same_v<InnerT, Number>) {
//                        std::cout << "Number: " << innerVal << ' ';
//                    } else if constexpr (std::is_same_v<InnerT, Message>) {
//                        std::cout << "Message: " << innerVal << ' ';
//                    }
//                    // Add more cases if there are more types in ListElement
//                }, elem);
//            }
//            std::cout << std::endl;
//        } else if constexpr (std::is_same_v<T, AudioSignal>) {
//            std::cout << "AudioSignal: ";
////            AudioSignal a =  std::get<AudioSignal>(val);
//            for(int i = 0 ; i < val.size();i++){
//                if( i % 64 == 0){
//                    std::cout << val[i] << ' ';
//                }
//            }
//            std::cout << std::endl;
//        } else {
//            std::cout << "Unhandled type in BlockData variant." << std::endl;
//        }
//    }, value);

    
    if (connections.find(block) != connections.end()) {
        for (const auto& connection : connections[block]) {
            if (connection.outletIndex == outletIndex) {
                // Propagate the output value to the connected block's input
                connection.destinationBlock->setInput(connection.inletIndex, value);
                
            }
        }
    }
    
}

void BlockChain::tick() {
   for (auto* block : blocks) {
       if (block) {
           block->timerTick();  // Call timerTick on each block
       }
   }
}




void BlockChain::prepareToProcess(int numSamples, double sampleRate){
    // Notify each block about the new sample rate and number of samples
    this->numSamples = numSamples;
    this->sampleRate = sampleRate ;
    
    
    for (auto* block : blocks) {
        if (block->handlesAudioProcessing()) {
            block->configureAudioParameters(numSamples, sampleRate);
//            std::cout<<"block at:"<<block<<std::endl;
//            std::cout<<"change *****numSamples size :"<<numSamples<<std::endl;
            
        }
    }
    
}


void BlockChain::processAudioStream(std::vector<AudioSignal>& audioBuffer, int numOutputChannels) {
    BufferPool bufferPool;

    // Process each block
    for (auto* block : blocks) {
        if (block->handlesAudioProcessing()) {
            block->processAudio();

            // Special handling for audio output blocks (like DAC)
            if (block->handlesAudioOutput()) {
                for (size_t channel = 0; channel < static_cast<size_t>(numOutputChannels); ++channel) {
                    AudioSignal channelSignal = block->getAudioOutput(channel);
                    if (!channelSignal.empty()) {
                        
//                        std::cout<<"numSamples size :"<<numSamples<<std::endl;
//                        std::cout<<"channelSignal size :"<<channelSignal.size()<<std::endl;
                        // Ensure the audio buffer size matches the number of samples
                        if(this->numSamples == channelSignal.size()){
                            audioBuffer[channel].resize(this->numSamples);
                            std::copy(channelSignal.begin(), channelSignal.end(), audioBuffer[channel].begin());
                        }
                        
                    }
                }
            } else {
                // Distribute the audio signal from the block to its connected blocks
                distributeAudioSignal(block, bufferPool); // Pass the block and buffer pool
                }
        }
    }
    
    
    // Finalize buffers in BufferPool and pass them to block inlets
    finalizeBuffers(bufferPool);
    
    // Filling any unused output channels with silence
     for (size_t channel = 0; channel < static_cast<size_t>(numOutputChannels); ++channel) {
         if (audioBuffer[channel].size() < static_cast<size_t>(this->numSamples)) {
             audioBuffer[channel].assign(this->numSamples, 0.0); // Fill with silence if no signal
         }
     }
    
}


void BlockChain::distributeAudioSignal(BlockInterface* block, BufferPool& bufferPool) {
    if (connections.find(block) != connections.end()) {
        for (const auto& connection : connections[block]) {
            if (connection.destinationBlock->handlesAudioProcessing()) {
                // Get the audio signal from the specific outlet of the block
                AudioSignal outputSignal = connection.sourceBlock->getAudioOutput(connection.outletIndex);

                // Accumulate the signal in the buffer pool for the respective inlet
                if (!outputSignal.empty()) {
                    bufferPool.accumulateSignal(connection.destinationBlock, connection.inletIndex, outputSignal);

                }
            }
            
        }

        // Set the accumulated signals to the respective inlets of the destination block
        for (size_t inletIndex = 0; inletIndex < block->getNumberOfInputs(); ++inletIndex) {
            AudioSignal accumulatedSignal = bufferPool.getAccumulatedSignal(block, inletIndex);
            if (!accumulatedSignal.empty()) {
                block->setInput(inletIndex, accumulatedSignal);
            }
        }
    }
}


void BlockChain::finalizeBuffers(BufferPool& bufferPool) {
    // Iterate over all buffers and set the accumulated signal to the respective block inlet
    bufferPool.forEachBuffer([](const auto& key, auto& buffer) {
        key.first->setInput(key.second, buffer);
    });
    // Clear the buffer pool for the next audio processing cycle
    bufferPool.clearAllBuffers();
    
}
