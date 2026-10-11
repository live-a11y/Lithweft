/*
  ==============================================================================

    Timer.h
    Created: 11 Feb 2024 10:34:56am
    Author:  刘昊

  ==============================================================================
*/

#pragma once
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

class Timer {
public:
    class Listener {
    public:
        virtual ~Listener() = default; // Ensure a virtual destructor for proper cleanup
        virtual void onTimerTick() = 0; // Pure virtual function to be implemented by derived classes
    };
    
private:
    std::vector<Listener*> listeners; // Store pointers to Listener objects
    std::atomic<bool> running{false};
    std::thread worker;
    std::mutex listenersMutex;
    int intervalMs;

    void run() {
        auto nextTick = std::chrono::steady_clock::now() + std::chrono::milliseconds(intervalMs);
        while (running) {
            std::this_thread::sleep_until(nextTick);
            nextTick += std::chrono::milliseconds(intervalMs);

            std::lock_guard<std::mutex> lock(listenersMutex);
            for (auto* listener : listeners) {
                listener->onTimerTick(); // Polymorphic call to the listener
            }
        }
    }

public:    
    Timer(int interval = 1) : intervalMs(interval) {}

    ~Timer() {
        stop();
    }

    void addListener(Listener* listener) {
        std::lock_guard<std::mutex> lock(listenersMutex);
        listeners.push_back(listener);
    }

    void removeListener(Listener* listener) {
        std::lock_guard<std::mutex> lock(listenersMutex);
        listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
    }

    void start() {
        if (!running) {
            running = true;
            worker = std::thread(&Timer::run, this);
        }
    }

    void stop() {
        if (running) {
            running = false;
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};
