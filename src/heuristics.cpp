#include "heuristics.h"
#include "operators.h"
#include "cost.h"
#include "feasability.h"
#include <random>
#include <ctime>
#include <numbers>
#include <cmath>
#include <iostream>

Solution genInitialSolution(const Problem& p) {
    Solution routes;
    routes.resize(p.vehicles.size()+1);

    routes.back().reserve(p.calls.size() * 2);
    for (int i{0}; i < p.calls.size(); ++i) {
        routes.back().push_back(i);
        routes.back().push_back(i);
    }
    return routes;
}

Solution genRandSolution(const Problem& p) {
    // Random engine, seeded by current time.
    static std::default_random_engine ran{static_cast<unsigned int>(std::time(nullptr))};

    Solution routes;
    const auto vSize{p.vehicles.size()};
    routes.resize(vSize+1);

    std::vector<int> remainingCalls{};
    remainingCalls.reserve(p.calls.size());
    for (int i{0}; i < p.calls.size(); ++i)
        remainingCalls.push_back(i);
    std::shuffle(remainingCalls.begin(), remainingCalls.end(), ran);

    while (!remainingCalls.empty()) {
        routes.at(ran() % (vSize+1)).push_back(remainingCalls.back());
        remainingCalls.pop_back();
    }

    for (auto& route : routes) {
        route.insert(route.end(), route.begin(), route.end());
        // Reshuffle each route after duplicating:
        std::shuffle(route.begin(), route.end(), ran);
    }

    return routes;
}

Solution blindRandomSearch(const Problem& p) {
    constexpr int MAX_SEARCH = 10000;

    
    auto best = genInitialSolution(p); // init to dummy solution
    auto cost = getCost(p, best).val_or_max();

    for (int i{0}; i < MAX_SEARCH; ++i) {
        const auto current = genRandSolution(p);
        const auto result = checkFeasability(p, current);
        if (!result) {
            const auto newCost = getCost(p, current).val_or_max();
            if (newCost && newCost < cost) {
                best = current;
                cost = newCost;
            }
        }
    }


    return best;
}

Solution localSearch(const Problem& p) {
    constexpr int MAX_SEARCH = 10000;
    // Random engine, seeded by current time.
    static std::default_random_engine ran{static_cast<unsigned int>(std::time(nullptr))};
    // Available operators
    const std::vector operators{
        op::ex2,
        op::ex3,
        op::ins1
    };


    auto best = genInitialSolution(p); // init to dummy solution
    auto cost = getCost(p, best).val_or_max();

    for (int i{0}; i < MAX_SEARCH; ++i) {
        // Use random operator
        const auto r = ran() % 100 * 0.01f;
        const auto current = r < 0.4f ? operators[0](best) : r < 0.4f + 0.3f ? operators[1](best) : operators[2](best);

        const auto result = checkFeasability(p, current);
        if (!result) {
            const auto newCost = getCost(p, current).val_or_max();
            if (newCost && newCost < cost) {
                best = current;
                cost = newCost;
            }
        }
    }


    return best;
}

Solution simulatedAnnealing(const Problem& p) {
    constexpr int MAX_SEARCH = 10000;
    // Random engine, seeded by current time.
    static std::default_random_engine ran{static_cast<unsigned int>(std::time(nullptr))};
    // Available operators
    const std::vector operators{
        op::ex2,
        op::ex3,
        op::ins1
    };


    auto best = genInitialSolution(p); // init to dummy solution
    auto incumbent = best;
    auto bestCost = getCost(p, best).val_or_max();
    auto incumbentCost = getCost(p, incumbent).val_or_max();

    // Initial temperature calculated from initial cost
    double temperature = -bestCost/std::log(0.99);
    // Cooling factor calculated from iterations and initial temperature
    double coolingFactor = std::exp(std::log(0.22/temperature) / MAX_SEARCH);

    const auto jumpProbability = [&](const auto& costDiff){
        return std::pow(std::numbers::e, -costDiff / temperature);
    };

    const auto rand = [&](){
        return ran() % 1000000 * 0.000001;
    };

    for (int i{0}; i < MAX_SEARCH; ++i, temperature *= coolingFactor) {
        // Use random operator
        const auto r = ran() % 100 * 0.01f;
        const auto current = r < 0.4f ? operators[0](incumbent) : r < 0.4f + 0.3f ? operators[1](incumbent) : operators[2](incumbent);

        // Check if solution is feasible
        const auto result = checkFeasability(p, current);
        if (!result) {
            const auto cost = getCost(p, current);
            if (cost) {
                const auto currentCost = cost.val_or_max();
                const auto costDiff = currentCost - incumbentCost;

                const auto r2 = rand();
                const auto p = jumpProbability(costDiff);
                // Check for new local best
                if (costDiff < 0) {
                    incumbent = current;
                    incumbentCost = currentCost;

                    // Check for new total best
                    if (incumbentCost < bestCost) {
                        best = incumbent;
                        bestCost = incumbentCost;
                    }
                // Randomly choose a worse solution as a local best
                } else if (r2 < p) {
                    incumbent = current;
                    incumbentCost = currentCost;
                }
            } else {
                // Should never check the cost of a infeasible solution, so just throw a logic error.
                throw std::logic_error{"Failed to calculate cost for feasible solution"};
            }
        }
    }
    return best;
}