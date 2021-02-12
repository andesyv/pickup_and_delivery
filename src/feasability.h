#pragma once
#include "problem.h"
#include <optional>
#include <algorithm>

// Returns nothing if feasable, error otherwise
std::optional<std::runtime_error> checkFeasability(const Problem& problem, const std::vector<std::vector<int>>& solution) {
    for (std::size_t i{0}; i < solution.size(); ++i) {
        const auto& route = solution.at(i);
        const auto& vehicle = problem.vehicles.at(i);
        
        // Dummy vehicle is always feasable. Skip
        if (problem.vehicles.size() <= i)
            continue;

        // Check if route is available for vehicle
        const auto& available = vehicle.availableCalls;
        for (auto call : route)
            if (!std::any_of(available.begin(), available.end(), [&](const auto& v){ return v == call; }))
                return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" does not have required route ").append(std::to_string(call))};

        // Vehicle capacity
        int capacity{0};
        std::vector<int> currentCalls;
        for (auto callIndex : route) {
            const auto& call = problem.calls.at(callIndex);
            auto search = std::find(currentCalls.begin(), currentCalls.end(), callIndex);
            if (search == currentCalls.end()) {
                currentCalls.push_back(callIndex);
                capacity += call.size;
            } else {
                currentCalls.erase(search);
                capacity -= call.size;
            }

            // Exceeded capacity, exit.
            if (vehicle.capacity < capacity)
                return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" exceeds capacity.")};
            
            if (capacity < 0)
                capacity = 0;
        }
    }

    return std::nullopt;
}