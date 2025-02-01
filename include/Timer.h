#pragma once

#include "Headers.h"

class Timer {
public:
    Timer() : start_time(0), paused_time(0), running(false), paused(false) {}


    void start() {
        if (!running) {
            start_time = std::clock();  // Start from the current time
            running = true;
            paused = false;
            paused_time = 0;  // Reset paused time
        }
    }

    void stop() {
        running = false;
        paused = false;
    }

    void Pause() {
        if (running && !paused) {
            paused_time = std::clock() - start_time;  // Capture how long it's been running
            paused = true;
        }
    }

    void Resume() {
        if (running && paused) {
            start_time = std::clock() - paused_time;  // Adjust start time to account for the paused duration
            paused = false;
        }
    }

    bool isStopped() const {
        return !running;
    }

    bool isRunning() const {
        return running && !paused;
    }

    bool IsPaused() const {
        return paused;
    }

    bool HasValidData() const {
        return (start_time > 0);
    }

    void reset() {
        start_time = std::clock();
        running = true;
        paused = false;
        paused_time = 0;
    }

    double getElapsedTime() const {
        if (!running) {
            return 0;
        }
        if (paused) {
            return (paused_time / static_cast<double>(CLOCKS_PER_SEC)) * 1000;
        }
        return ((std::clock() - start_time) / static_cast<double>(CLOCKS_PER_SEC)) * 1000;
    }

    bool hasElapsed(double milliseconds) const {
        if (!running || paused) {
            return false;
        }
        double elapsed_time = getElapsedTime();
        return elapsed_time >= milliseconds;
    }

private:
    std::clock_t start_time;  // Start time of the timer
    std::clock_t paused_time; // Time the timer was paused
    bool running;             // Whether the timer is running
    bool paused;              // Whether the timer is paused
};
