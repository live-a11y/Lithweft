//#include "BlockSystem.h"
//#include <iostream>
//
//BlockSystem::~BlockSystem() {
//    // Clean up blocks
//    for (auto& block : blocks) {
//        block.reset();  // This will call the destructor of each block if it exists
//    }
//}
//
//void BlockSystem::addBlock(std::unique_ptr<BlockInterface> block) {
//    // The block is assumed to be already initialized when passed to this function.
//    // Here we simply add the block to our vector of blocks.
//    blocks.push_back(std::move(block));
//}
//
//// Method to remove a block from the system
//void BlockSystem::removeBlock(BlockInterface* blockToRemove) {
//       // Remove the block from the vector of blocks
//       blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
//                                   [blockToRemove](const std::unique_ptr<BlockInterface>& block) {
//                                       return block.get() == blockToRemove;
//                                   }), blocks.end());
//
//       // Remove all connections associated with this block
//       connections.erase(blockToRemove);
//       for (auto& [sourceBlock, destInfos] : connections) {
//           destInfos.erase(std::remove_if(destInfos.begin(), destInfos.end(),
//                                          [blockToRemove](const Connection& conn) {
//                                              return conn.destinationBlock == blockToRemove;
//                                          }), destInfos.end());
//       }
//   }
//
//void BlockSystem::connectBlocks(BlockInterface* sourceBlock, size_t outletIndex,
//                                BlockInterface* destinationBlock, size_t inletIndex) {
//    // Store the connection information
//    connections[sourceBlock].emplace_back(Connection{destinationBlock, inletIndex, outletIndex});
//}
//
//
//// Method to disconnect one block's outlet from another block's inlet
//void BlockSystem::disconnectBlocks(BlockInterface* sourceBlock, size_t outletIndex,BlockInterface* destinationBlock, size_t inletIndex) {
//    // If the source block has connections, proceed to remove the specific one
//    if (connections.count(sourceBlock)) {
//        // Use std::remove_if to remove the connection from the vector
//        auto& connVector = connections[sourceBlock];
//        connVector.erase(
//            std::remove_if(
//                connVector.begin(),
//                connVector.end(),
//                [destinationBlock, inletIndex, outletIndex](const Connection& conn) {
//                    return conn.destinationBlock == destinationBlock &&
//                           conn.inletIndex == inletIndex &&
//                           conn.outletIndex == outletIndex;
//                }
//            ),
//            connVector.end()
//        );
//
//        // If there are no more connections for the source block, remove the entry from the map
//        if (connVector.empty()) {
//            connections.erase(sourceBlock);
//        }
//    }
//}
//
//
//void BlockSystem::blockOutputTriggered(BlockInterface* block, size_t outletIndex, const BlockData& value) {
//    std::cout << "block triggered outlet: " << block;
//    std::cout << "   outlet: " << outletIndex << "   value: ";
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
//        } else if constexpr (std::is_same_v<T, AudioSignal>) {
//            std::cout << "AudioSignal: ";
//            for (const auto& sample : val) {
//                std::cout << sample << ' ';
//            }
//            std::cout << std::endl;
//        } else {
//            std::cout << "Unhandled type in BlockData variant." << std::endl;
//        }
//    }, value);
//
//    // Propagate the output value to connected blocks
//    if (connections.find(block) != connections.end()) {
//        for (const auto& connection : connections[block]) {
//            if (connection.outletIndex == outletIndex) {
//                connection.destinationBlock->setInput(connection.inletIndex, value);
//            }
//        }
//    }
//}
//
//
/*
  ==============================================================================

    BlockChain.cpp
    Created: 9 Nov 2023 8:49:44pm
    Author:  刘昊

  ==============================================================================
*/
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

void BlockChain::handleSystemMessage(const std::string& operation, const BlockData& data) {
    // Implementation depends on your system's requirements
    // Example:
    if (operation == "print") {
        // Handle print operation
        // If data is a message, print it
        if (std::holds_alternative<Message>(data)) {
            std::cout << std::get<Message>(data) << std::endl;
        }
    } else if (operation == "registration") {
        // Handle registration operation
        // ... Other logic ...
    }
    // Add more operations as needed
}


void BlockChain::handleBlockOutlet(BlockInterface* block, size_t outletIndex, const BlockData& value) {
    // Handle the outlet event, such as updating connections or passing data
    // For example, update UI components or pass the value to other parts of the system
    
    
    std::cout<<"block triggered outlet: "<< block;
    std::cout<<"   outlet :" <<outletIndex<<"   value: ";


    // Use std::visit with a generic lambda to print the value
    // Use std::visit with a generic lambda to print the value
    std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, Uninitialized>) {
            std::cout << "Uninitialized" << std::endl;
        } else if constexpr (std::is_same_v<T, Number>) {
            std::cout << "Number: " << val << std::endl;
        } else if constexpr (std::is_same_v<T, Message>) {
            std::cout << "Message: " << val << std::endl;
        } else if constexpr (std::is_same_v<T, AudioSignal>) {
            std::cout << "AudioSignal: ";
            for (const auto& sample : val) {
                std::cout << sample << ' ';
            }
            std::cout << std::endl;
        } else {
            std::cout << "Unhandled type in BlockData variant." << std::endl;
        }
    }, value);


    
    if (connections.find(block) != connections.end()) {
        for (const auto& connection : connections[block]) {
            if (connection.outletIndex == outletIndex) {
                // Propagate the output value to the connected block's input
                connection.destinationBlock->setInput(connection.inletIndex, value);
                
            }
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
