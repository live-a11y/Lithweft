/*
  ==============================================================================

    BlockChain.h
    Created: 9 Nov 2023 8:49:44pm
    Author:  刘昊

  ==============================================================================
*/


#pragma once

#include "BlockInterface.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include "BufferPool.h"
#include <map>
#include <set>

//#include <JuceHeader.h>

// This structure is used to keep track of connections between blocks
//struct Connection {
//    BlockInterface* sourceBlock;
//    BlockInterface* destinationBlock;
//    size_t inletIndex;
//    size_t outletIndex;
//};

struct Connection {
    BlockInterface* sourceBlock;
    BlockInterface* destinationBlock;
    size_t inletIndex;
    size_t outletIndex;
    // Add an identifier to denote audio connection if needed
    bool isAudioConnection = false;
    
    bool operator==(const Connection& other) const {
        return sourceBlock == other.sourceBlock &&
               destinationBlock == other.destinationBlock &&
               inletIndex == other.inletIndex &&
               outletIndex == other.outletIndex;
    }
};



   
class BlockChain   {

public:

    void onTest();
    
    BlockChain();
    ~BlockChain();
    // New method to handle block creation requests

      // New method to clean up and unload a dynamic library
    void addBlock(BlockInterface* block);
    void removeBlock(BlockInterface* blockToRemove);
    void replaceBlock(BlockInterface* oldBlock, BlockInterface* newBlock);

    void connectBlocks(BlockInterface* sourceBlock, size_t outletIndex,
                       BlockInterface* destinationBlock, size_t inletIndex);
    
    
    void disconnectBlocks(BlockInterface* sourceBlock, size_t outletIndex,
                          BlockInterface* destinationBlock, size_t inletIndex);
    std::vector<Connection> getConnections() const;

    SystemMessageCallback systemMessageCallback = [this](BlockInterface* sender, const std::string& operationSign,const BlockData& data) {
            handleSystemMessage(sender,operationSign,data);
       };

       OutletCallback outletCallback = [this](BlockInterface* block, size_t outletIndex, const BlockData& value) {
           handleBlockOutlet(block, outletIndex, value);
       };
    
    
    void prepareToProcess(int numSamples, double sampleRate);
    void processAudioStream(std::vector<AudioSignal>& audioBuffer, int numOutputChannels);
//    void distributeAudioSignal(BlockInterface* block) ;
    void distributeAudioSignal(BlockInterface* block, BufferPool& bufferPool) ;
    void finalizeBuffers(BufferPool& bufferPool);

    //for test
    // Add a method to find end-of-chain blocks
    std::vector<BlockInterface*> getEndOfChainBlocks() {
        std::vector<BlockInterface*> endBlocks;

        for (const auto& block : blocks) {
            // Check if the block is connected to any other block
            if (connections.find(block) == connections.end() || connections[block].empty()) {
                // If not connected, it's an end-of-chain block
                endBlocks.push_back(block);
            }
        }

        return endBlocks;
    }
    
    
    void tick();
private:

//    AudioSignal processAccumulatedSignal(const std::vector<AudioSignal>& signals);
   void handleBlockOutlet(BlockInterface* block, size_t outletIndex, const BlockData& value);
    
    // You may need to define this new method to handle the system messages
    void handleSystemMessage(BlockInterface* sender,const std::string& operation, const BlockData& data);
        
    std::unordered_map<BlockInterface*, std::vector<Connection>> connections;
    std::vector<BlockInterface*> blocks;
    
    int numSamples;
    double sampleRate;
    
    


};
