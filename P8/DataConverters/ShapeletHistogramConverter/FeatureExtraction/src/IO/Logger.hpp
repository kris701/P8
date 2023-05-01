#ifndef FEATUREEXTRACTION_LOGGER_H
#define FEATUREEXTRACTION_LOGGER_H

#include <string>
#include <chrono>
#include <unordered_map>
#include <optional>
#include <iostream>
#include "misc/Constants.hpp"

class Logger {
public:
    static uint Begin(const std::string &desc);
    static int64_t End(uint id);
    static void Info(const std::string &desc);

private:
    struct LogPoint {
        const std::optional<uint> parent;
        const std::string desc;
        bool finished = false;
        // Initial time
        std::chrono::steady_clock::time_point iTime;
        // End time
        std::chrono::steady_clock::time_point eTime;
        int64_t time = 0;
        explicit LogPoint(const std::string &desc) : desc(desc), finished(true) {}
        LogPoint(const std::string &desc, std::chrono::steady_clock::time_point iTime) : desc(desc), iTime(iTime) {}
        LogPoint(const std::string &desc, std::chrono::steady_clock::time_point iTime, uint parent) :
            desc(desc), iTime(iTime), parent(parent) {}
    };
    static inline uint currentId = 0;
    static inline uint currentIndent = 0;
    static inline std::vector<LogPoint> logPoints;
};

uint Logger::Begin(const std::string &desc) {
    const uint id = currentId++;

    logPoints.emplace_back(desc, std::chrono::steady_clock::now());

    if (logPoints.size() > 1 && !logPoints.at(logPoints.size() - 2).finished)
        std::cout << std::endl << std::flush;
    uint indent = currentIndent++;
    for (uint i = 0; i < indent; i++)
        std::cout << '\t';

    std::cout << desc << "..." << std::flush;

    return id;
}

int64_t Logger::End(uint id) {
    LogPoint *logPoint = &logPoints.at(id);
    if (logPoint->finished)
        throw std::logic_error("Cannot finish logpoint " + std::to_string(id) + " as it is already finished.");
    else
        logPoint->finished = true;

    logPoint->eTime = std::chrono::steady_clock::now();
    logPoint->time = std::chrono::duration_cast<std::chrono::milliseconds>(logPoint->eTime - logPoint->iTime).count();

    currentIndent--;

    if (id == logPoints.size() - 1)
        std::cout << "Done";
    else {
        for (uint i = 0; i < currentIndent; i++)
            std::cout << '\t';
        std::cout << "Finished " << logPoint->desc;
    }

    std::cout << " (" << std::to_string(logPoint->time) << "ms)" << std::endl << std::flush;

    return logPoint->time;
}

void Logger::Info(const std::string &desc) {
    currentId++;

    logPoints.emplace_back(desc);

    if (logPoints.size() > 1 && !logPoints.at(logPoints.size() - 2).finished)
        std::cout << std::endl << std::flush;
    for (uint i = 0; i < currentIndent; i++)
        std::cout << '\t';

    std::cout << desc << std::endl;
}


#endif //FEATUREEXTRACTION_LOGGER_H
