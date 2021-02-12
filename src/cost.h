#pragma once
#include "problem.h"
#include <optional>
#include <unordered_set>

/// Basically the same code as in the feasability check. (TODO: Refactor)
Result<int, std::runtime_error> getCost(const Problem &problem, const std::vector<std::vector<int>> &solution) {
    int totalCost{0};
    for (std::size_t i{0}; i < solution.size(); ++i) {
        const auto &route = solution.at(i);

        auto findPath = [&](const auto &a, const auto &b) {
            auto it = std::find_if(problem.trips.begin(), problem.trips.end(), [&](const Trip &t) { return t.vehicleIndex == i && t.origin == a && t.destination == b; });
            return (it != problem.trips.end()) ? std::optional<Trip>{*it} : std::nullopt;
        };


        auto findVehicleCallCombo = [&](const auto& c) {
            auto it = std::find_if(problem.vehicleCalls.begin(), problem.vehicleCalls.end(), [&](const VehicleCall& t){ return t.vehicleIndex == i && t.callIndex == c; });
            return (it != problem.vehicleCalls.end()) ? std::optional<VehicleCall>{*it} : std::nullopt;
        };

        std::vector<int> currentCalls;
        if (i < problem.vehicles.size()) {
            const auto &vehicle = problem.vehicles.at(i);
            int currentNode{vehicle.homeNodeIndex};
            /** Logic:
             * When adding route, first traverse to route node.
             * When removing route, traverse to end node for removed route.
             */
            for (auto callIndex : route)
            {
                const auto &call = problem.calls.at(callIndex);
                auto search = std::find(currentCalls.begin(), currentCalls.end(), callIndex);
                if (search == currentCalls.end())
                {
                    // Origin
                    currentCalls.push_back(callIndex);
                    // Since adding new route, traverse to start of route.
                    auto path = findPath(currentNode, call.origin);
                    if (!path)
                        return std::runtime_error{"Could not find route path."};
                    totalCost += path.value().cost;

                    // It costs some moneys to pick up package
                    auto vehicleCall = findVehicleCallCombo(callIndex);
                    if (!vehicleCall)
                        return std::runtime_error{"Could not find vehicle call combo."};
                    totalCost += vehicleCall.value().originNodeCosts;

                    currentNode = path.value().destination;
                }
                else
                {
                    // Destination
                    auto path = findPath(currentNode, call.destination);
                    if (!path)
                        return std::runtime_error{"Could not find route path."};
                    totalCost += path.value().cost;
                    currentNode = path.value().destination;

                    // It costs some moneys to deliver package
                    auto vehicleCall = findVehicleCallCombo(callIndex);
                    if (!vehicleCall)
                        return std::runtime_error{"Could not find vehicle call combo."};
                    totalCost += vehicleCall.value().destNodeCosts;

                    currentCalls.erase(search);
                }
            }
        } else {
            for (auto callIndex : std::unordered_set<int>{route.begin(), route.end()}) {
                const auto& call = problem.calls.at(callIndex);
                totalCost += call.costOfNotTransporting;
            }
        }
    }

    return totalCost;
}