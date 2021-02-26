#include "cost.h"
#include <optional>
#include <unordered_set>
#include <utility>

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