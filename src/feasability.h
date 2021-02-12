#pragma once
#include "problem.h"
#include <optional>
#include <algorithm>

// Returns nothing if feasable, error otherwise
std::optional<std::runtime_error> checkFeasability(const Problem& problem, const std::vector<std::vector<int>>& solution) {
    for (std::size_t i{0}; i < solution.size(); ++i) {
        const auto& route = solution.at(i);
        
        // Check if route is available for vehicle
        if (i < problem.vehicles.size()) {
            const auto& available = problem.vehicles.at(i).availableCalls;
            for (auto call : route)
                if (!std::any_of(available.begin(), available.end(), [&](const auto& v){ return v == call; }))
                    return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" does not have required route ").append(std::to_string(call))};
        }

        
    }

    return std::nullopt;
}