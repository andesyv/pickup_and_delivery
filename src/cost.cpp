#include "cost.h"
#include <optional>
#include <unordered_set>
#include <utility>
#include <algorithm>

Result<int, std::runtime_error> getCost(const Problem &problem, const Solution& solution) {
    int totalCost{0};
    for (index_t i{0}; i < solution.size(); ++i) {
        const auto &route{solution[i]};

        const auto findPath = [&](const auto &a, const auto &b) {
            auto it = std::find_if(problem.trips.begin(), problem.trips.end(), [&](const Trip &t) { return t.vehicleIndex == i && t.origin == a && t.destination == b; });
            return (it != problem.trips.end()) ? std::optional<Trip>{*it} : std::nullopt;
        };

        std::vector<int> currentCalls;
        currentCalls.reserve(problem.calls.size());
        if (i < problem.vehicles.size()) {
            const auto &vehicle{problem.vehicles[i]};
            int currentNode{vehicle.homeNodeIndex};
            /** Logic:
             * When adding route, first traverse to route node.
             * When removing route, traverse to end node for removed route.
             */
            for (auto callIndex : route)
            {
                const auto &call{problem.calls[callIndex]};
                const auto search{std::find(currentCalls.begin(), currentCalls.end(), callIndex)};
                if (search == currentCalls.end())
                {
                    // Origin
                    currentCalls.push_back(callIndex);
                    // Since adding new route, traverse to start of route.
                    const auto pOpt = findPath(currentNode, call.origin);
                    if (!pOpt)
                        return std::runtime_error{"Could not find route path."};
                    const auto path{pOpt.value()};
                    totalCost += path.cost;

                    // It costs some moneys to pick up package
                    if (!problem.vehicleCalls.contains({i, callIndex}))
                        return std::runtime_error{"Could not find vehicle call combo."};
                    const auto vehicleCall = problem.vehicleCalls.at({i, callIndex});
                    totalCost += vehicleCall.originNodeCosts;

                    currentNode = path.destination;
                }
                else
                {
                    // Destination
                    const auto pOpt = findPath(currentNode, call.destination);
                    if (!pOpt)
                        return std::runtime_error{"Could not find route path."};
                    const auto path{pOpt.value()};
                    totalCost += path.cost;
                    currentNode = path.destination;

                    // It costs some moneys to deliver package
                    if (!problem.vehicleCalls.contains({i, callIndex}))
                        return std::runtime_error{"Could not find vehicle call combo."};
                    const auto vehicleCall = problem.vehicleCalls.at({i, callIndex});
                    totalCost += vehicleCall.destNodeCosts;

                    currentCalls.erase(search);
                }
            }
        } else {
            for (auto callIndex : std::unordered_set<int>{route.begin(), route.end()}) {
                const auto& call{problem.calls[callIndex]};
                totalCost += call.costOfNotTransporting;
            }
        }
    }

    return totalCost;
}

Result<int, std::runtime_error> getCost(const Problem &problem, const SolutionComp& solution) {
    int totalCost{0};
    std::size_t routeStart{0}, routeSize{0};
    for (index_t i{0}; i < problem.vehicles.size() + 1; ++i, routeStart += routeSize+1) {
        const auto begin = solution.begin() + routeStart;
        const auto end = std::find(begin, solution.end(), -1);
        routeSize = end - begin;
        const array_view route{solution.data() + routeStart, routeSize};
        
        // Empty route = feasible route
        if (routeSize == 0)
            continue;


        const auto findPath = [&](const auto &a, const auto &b) {
            auto it = std::find_if(problem.trips.begin(), problem.trips.end(), [&](const Trip &t) { return t.vehicleIndex == i && t.origin == a && t.destination == b; });
            return (it != problem.trips.end()) ? std::optional<Trip>{*it} : std::nullopt;
        };

        std::vector<int> currentCalls;
        currentCalls.reserve(problem.calls.size());
        if (i < problem.vehicles.size()) {
            const auto &vehicle{problem.vehicles[i]};
            int currentNode{vehicle.homeNodeIndex};
            /** Logic:
             * When adding route, first traverse to route node.
             * When removing route, traverse to end node for removed route.
             */
            for (auto callIndex : route)
            {
                const auto &call{problem.calls[callIndex]};
                const auto search{std::find(currentCalls.begin(), currentCalls.end(), callIndex)};
                if (search == currentCalls.end())
                {
                    // Origin
                    currentCalls.push_back(callIndex);
                    // Since adding new route, traverse to start of route.
                    const auto pOpt = findPath(currentNode, call.origin);
                    if (!pOpt)
                        return std::runtime_error{"Could not find route path."};
                    const auto path{pOpt.value()};
                    totalCost += path.cost;

                    // It costs some moneys to pick up package
                    if (!problem.vehicleCalls.contains({i, callIndex}))
                        return std::runtime_error{"Could not find vehicle call combo."};
                    const auto vehicleCall = problem.vehicleCalls.at({i, callIndex});
                    totalCost += vehicleCall.originNodeCosts;

                    currentNode = path.destination;
                }
                else
                {
                    // Destination
                    const auto pOpt = findPath(currentNode, call.destination);
                    if (!pOpt)
                        return std::runtime_error{"Could not find route path."};
                    const auto path{pOpt.value()};
                    totalCost += path.cost;
                    currentNode = path.destination;

                    // It costs some moneys to deliver package
                    if (!problem.vehicleCalls.contains({i, callIndex}))
                        return std::runtime_error{"Could not find vehicle call combo."};
                    const auto vehicleCall = problem.vehicleCalls.at({i, callIndex});
                    totalCost += vehicleCall.destNodeCosts;

                    currentCalls.erase(search);
                }
            }
        } else {
            for (auto callIndex : std::unordered_set<int>{route.begin(), route.end()}) {
                const auto& call{problem.calls[callIndex]};
                totalCost += call.costOfNotTransporting;
            }
        }
    }

    return totalCost;
}

#ifndef NDEBUG
static constexpr bool DEBUG = true;
#else
static constexpr bool DEBUG = false;
#endif

FeasibilityCostRet getFeasibleCost(const Problem& p, SolutionCached& s) {
    int totalCost{0};
    for (index_t i{0}; i < s.size(); ++i) {
        // If it was not changed but don't have a cost either, it's infeasible
        if (s[i].infeasible()) {
            if constexpr (DEBUG)
                return std::runtime_error{"Solution previously declared infeasible."};
            else
                return std::nullopt;
        // Skip the whole thing if it hasn't been updated
        } else if (s[i].feasible()) {
            totalCost += *s[i].cost;
            continue;
        }
        const auto &route{s[i].calls};
        int vehicleCost{0};

        const auto findPath = [&](const auto &a, const auto &b) {
            auto it = std::find_if(p.trips.begin(), p.trips.end(), [&](const Trip &t) { return t.vehicleIndex == i && t.origin == a && t.destination == b; });
            return (it != p.trips.end()) ? std::optional<Trip>{*it} : std::nullopt;
        };
        
        // Only check feasibility if not dummy vehicle. Dummy vehicle is always feasible.
        const auto bDummy = p.vehicles.size() <= i;
        if (!bDummy) {

            const auto& vehicle{p.vehicles[i]};

            // Check if route is available for vehicle
            const auto& available{vehicle.availableCalls};
            for (auto it{route.begin()}; it != route.end(); ++it)
                if (!std::any_of(available.begin(), available.end(), [&](const auto& v){ return v == *it; }))
                    if constexpr (DEBUG)
                        return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" does not have required route ").append(std::to_string(*it))};
                    else
                        return std::nullopt;


            std::vector<bool> currentCalls;
            currentCalls.resize(p.calls.size(), false);
            int capacity{0}; // Current capacity of vehicle (starts at 0)
            int time{vehicle.startingTime}; // Current time (starts at time elapsed since start)
            int currentNode{vehicle.homeNodeIndex};

            /** Logic:
             * When adding route, first traverse to route node.
             * When removing route, traverse to end node for removed route.
             */
            for (auto callIndex : route)
            {
                const auto &call{p.calls[callIndex]};
                if (!currentCalls[callIndex])
                {
                    // Origin
                    currentCalls[callIndex] = true;
                    // Since adding new route, traverse to start of route.
                    const auto pOpt = findPath(currentNode, call.origin);
                    if (!pOpt)
                        if constexpr (DEBUG)
                            return std::runtime_error{"Could not find route path."};
                        else
                            return std::nullopt;
                    const auto path{pOpt.value()};
                    vehicleCost += path.cost;
                    capacity += call.size;
                    time += path.time;
                    
                    // If exceeding capacity, exit.
                    if (vehicle.capacity < capacity)
                        if constexpr (DEBUG)
                            return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" exceeds capacity.")};
                        else
                            return std::nullopt;
                        

                    // Check that we didn't miss the pickup window.
                    if (call.upperTimewindowPickup < time)
                        if constexpr (DEBUG)
                            return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" missed their timeslot for pickup.")};
                        else
                            return std::nullopt;
                    
                    currentNode = path.destination;

                    // It costs some moneys to pick up package
                    if (!p.vehicleCalls.contains({i, callIndex}))
                        if constexpr (DEBUG)
                            return std::runtime_error{"Could not find vehicle call combo."};
                        else
                            return std::nullopt;
                    const auto vehicleCall = p.vehicleCalls.at({i, callIndex});

                    vehicleCost += vehicleCall.originNodeCosts;
                    time += vehicleCall.originNodeTime; // Take some time to pick up package

                    // Wait for pickup timewinow
                    if (time < call.lowerTimewindowDelivery)
                        time = call.lowerTimewindowDelivery;
                }
                else
                {
                    // Destination
                    const auto pOpt = findPath(currentNode, call.destination);
                    if (!pOpt)
                        if constexpr (DEBUG)
                            return std::runtime_error{"Could not find route path."};
                        else
                            return std::nullopt;
                    const auto path{pOpt.value()};

                    vehicleCost += path.cost;
                    time += path.time;

                    currentNode = path.destination;

                    // Assume that we deliver the instant we arrive
                    // Check that we didn't miss our delivery window
                    if (call.upperTimewindowDelivery < time)
                        if constexpr (DEBUG)
                            return std::runtime_error{std::string{"Vehicle "}.append(std::to_string(i)).append(" missed their timeslot for delivery.")};
                        else
                            return std::nullopt;

                    // Wait for delivery
                    if (time < call.lowerTimewindowDelivery)
                        time = call.lowerTimewindowDelivery;


                    if (!p.vehicleCalls.contains({i, callIndex}))
                        if constexpr (DEBUG)
                            return std::runtime_error{"Could not find vehicle call combo."};
                        else
                            return std::nullopt;
                    const auto vehicleCall = p.vehicleCalls.at({i, callIndex});
                    // It costs some moneys to deliver package
                    vehicleCost += vehicleCall.destNodeCosts;
                    // Spend some more time delivering package
                    time += vehicleCall.destNodeTime;
                    capacity -= call.size;
                    if (capacity < 0)
                        capacity = 0;

                    currentCalls[callIndex] = false;
                }
            }
        } else {
            for (auto callIndex : std::unordered_set<int>{route.begin(), route.end()}) {
                const auto& call{p.calls[callIndex]};
                vehicleCost += call.costOfNotTransporting;
            }
        }

        totalCost += vehicleCost;
        s[i].cost = vehicleCost;
        s[i].bChanged = false;
    }

    return totalCost;
}