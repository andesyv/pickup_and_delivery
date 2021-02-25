#include "feasability.h"
#include <algorithm>
#include <utility>


std::optional<std::runtime_error> checkFeasability(const Problem& problem, const Solution& solution) {
    for (index_t i{0}; i < solution.size(); ++i) {
        const auto& route = solution.at(i);
        
        // Dummy vehicle is always feasable. Skip
        if (problem.vehicles.size() <= i)
            continue;
        
        const auto& vehicle = problem.vehicles.at(i);

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

        auto findPath = [&](const auto& a, const auto& b) {
            auto it = std::find_if(problem.trips.begin(), problem.trips.end(), [&](const Trip& t){ return t.vehicleIndex == i && t.origin == a && t.destination == b; });
            return (it != problem.trips.end()) ? std::optional<Trip>{*it} : std::nullopt;
        };

        // Route timeslots
        currentCalls.clear();
        int time{vehicle.startingTime}; // Time elapsed since start
        int currentNode{vehicle.homeNodeIndex};
        /** Logic:
         * When adding route, first traverse to route node.
         * When removing route, traverse to end node for removed route.
         */
        for (auto callIndex : route) {
            const auto& call = problem.calls.at(callIndex);
            auto search = std::find(currentCalls.begin(), currentCalls.end(), callIndex);
            if (search == currentCalls.end()) {
                currentCalls.push_back(callIndex);
                // Since adding new route, traverse to start of route.
                auto path = findPath(currentNode, call.origin);
                if (!path)
                    return std::runtime_error{"Could not find route path."};
                // Travel time
                time += path.value().time;
                currentNode = path.value().destination;

                // Check that we didn't miss the pickup window.
                if (call.upperTimewindowPickup < time)
                    return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" missed their timeslot for pickup.")};

                // Wait for pickup timewinow
                if (time < call.lowerTimewindowDelivery)
                    time = call.lowerTimewindowDelivery;

                // Take some time to pick up package
                if (!problem.vehicleCalls.contains({i, callIndex}))
                    return std::runtime_error{"Could not find vehicle call combo."};
                auto vehicleCall = problem.vehicleCalls.at({i, callIndex});
                time += vehicleCall.originNodeTime;

            } else {
                auto path = findPath(currentNode, call.destination);
                if (!path)
                    return std::runtime_error{"Could not find route path."};
                time += path.value().time;
                currentNode = path.value().destination;

                // Assume that we deliver the instant we arrive
                // Check that we didn't miss our delivery window
                if (call.upperTimewindowDelivery < time)
                    return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" missed their timeslot for delivery.")};
                
                // Wait for delivery
                if (time < call.lowerTimewindowDelivery)
                    time = call.lowerTimewindowDelivery;

                // Spend some more time delivering package
                if (!problem.vehicleCalls.contains({i, callIndex}))
                    return std::runtime_error{"Could not find vehicle call combo."};
                auto vehicleCall = problem.vehicleCalls.at({i, callIndex});
                time += vehicleCall.destNodeTime;
                
                currentCalls.erase(search);
            }
        }
    }

    return std::nullopt;
}