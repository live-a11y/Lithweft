/*
  ==============================================================================

    BufferPool.h
    Created: 20 Nov 2023 4:59:45pm
    Author:  刘昊

  ==============================================================================
*/

#pragma once
#include "BlockInterface.h"
class BufferPool {
public:
    // Add or accumulate an audio signal for a block's inlet
    void accumulateSignal(BlockInterface* block, size_t inletIndex, const AudioSignal& signal) {
        auto& accumulatedSignal = pool[std::make_pair(block, inletIndex)];
        if (accumulatedSignal.size() < signal.size()) {
            accumulatedSignal.resize(signal.size(), 0.0);
        }
        std::transform(accumulatedSignal.begin(), accumulatedSignal.end(), signal.begin(),
                       accumulatedSignal.begin(), std::plus<double>());
    }

    // Get the accumulated signal for a block's inlet
    AudioSignal getAccumulatedSignal(BlockInterface* block, size_t inletIndex) const {
        auto key = std::make_pair(block, inletIndex);
        auto it = pool.find(key);
        return (it != pool.end()) ? it->second : AudioSignal();
    }

    // Method to perform an operation on each buffer in the pool
     template <typename Func>
     void forEachBuffer(Func func) {
         for (auto& [key, buffer] : pool) {
             func(key, buffer);
         }
     }
    // Clear all buffers in the pool
    void clearAllBuffers() {
        for (auto& entry : pool) {
            entry.second.clear();
        }
    }

private:
    std::map<std::pair<BlockInterface*, size_t>, AudioSignal> pool; // Store accumulated signals
};
