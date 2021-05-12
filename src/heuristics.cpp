#include "heuristics.h"
#include "operators.h"
#include "cost.h"
#include "feasibility.h"
#include <random>
#include <ctime>
#include <numbers>
#include <cmath>
#include <iostream>
#include <array>
#include <ranges>
#include <algorithm>
#include <numeric>
#include "zip.hpp"
#include <functional>

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

SolutionCached genInitialSolutionCached(const Problem& p) {
    SolutionCached routes;
    routes.resize(p.vehicles.size()+1);

    auto& dummy = routes.back().calls;
    dummy.reserve(p.calls.size() * 2);
    for (int i{0}; i < p.calls.size(); ++i) {
        dummy.push_back(i);
        dummy.push_back(i);
    }
    return routes;
}

Solution genRandSolution(const Problem& p) {
    // Random engine, seeded by current time.
    static std::default_random_engine engine{static_cast<unsigned int>(std::time(nullptr))};

    return genRandSolution(p, engine);
}

Solution genRandSolution(const Problem& p, std::default_random_engine& ran) {
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

Solution blindRandomSearch(const Problem& p, std::default_random_engine& ran) {
    constexpr int MAX_SEARCH = 10000;

    
    auto best = fromNestedListZeroIndexed(genInitialSolution(p)); // init to dummy solution
    auto cost = getCost(p, best).val_or_max();

    for (int i{0}; i < MAX_SEARCH; ++i) {
        const auto current = fromNestedListZeroIndexed(genRandSolution(p, ran));
        const auto result = checkfeasibility(p, current);
        if (!result) {
            const auto newCost = getCost(p, current).val_or_max();
            if (newCost && newCost < cost) {
                best = current;
                cost = newCost;
            }
        }
    }


    return toNestedList(best);
}

Solution localSearch(const Problem& p, std::default_random_engine& ran) {
    constexpr int MAX_SEARCH = 10000;
    // Available operators
    const std::vector operators{
        op::ex2_comp,
        op::ex3_comp,
        op::ins1_comp
    };


    auto best = fromNestedListZeroIndexed(genInitialSolution(p)); // init to dummy solution
    auto cost = getCost(p, best).val_or_max();

    for (int i{0}; i < MAX_SEARCH; ++i) {
        // Use random operator
        const auto r{ran() % 100 * 0.01f};
        const auto current{r < 0.4f ? operators[0](best) : r < 0.4f + 0.3f ? operators[1](best) : operators[2](best)};

        const auto result = checkfeasibility(p, current);
        if (!result) {
            const auto newCost = getCost(p, current).val_or_max();
            if (newCost && newCost < cost) {
                best = current;
                cost = newCost;
            }
        }
    }


    return toNestedList(best);
}

Solution simulatedAnnealing(const Problem& p, std::default_random_engine& ran) {
    constexpr int MAX_SEARCH = 10000;
    // Available operators
    const auto operators = std::to_array<Solution (*)(Solution)>({
        &op::ex2,
        &op::ex3,
        &op::ins1
    });


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
        const auto result = checkfeasibility(p, current);
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

Solution simulatedAnnealing2ElectricBoogaloo(const Problem& p, std::default_random_engine& ran) {
    constexpr int MAX_SEARCH = 10000;
    // Available operators
    const auto operators = std::to_array<Solution(*)(const Problem&, Solution)>({
        op::fesins,
        op::freorder,
        op::backinsert,
    });


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
        const auto current = r < 0.65f ? operators[0](p, incumbent) : r < 0.27f + 0.08f ? operators[1](p, incumbent) : operators[2](p, incumbent);

        // Check if solution is feasible
        const auto result = checkfeasibility(p, current);
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

template <typename T,typename U>                                                   
auto operator+(const std::pair<T,U> & l,const std::pair<T,U> & r) {   
    return std::make_pair(l.first+r.first,l.second+r.second);                                    
}

template <typename T,typename U>                                                   
void operator+=(std::pair<T,U> & l,const std::pair<T,U> & r) {   
    l = l + r;
}

Solution adaptiveSearch(const Problem& p) {
    // Random engine, seeded by current time.
    static std::default_random_engine engine{static_cast<unsigned int>(std::time(nullptr))};

    return adaptiveSearch(p, engine);
}

Solution adaptiveSearch(const Problem& p, std::default_random_engine& ran) {
    constexpr unsigned int MAX_SEARCH = 10000;
    constexpr unsigned int SEGMENT_SIZE = 100;
    constexpr unsigned int ESCAPE_CONDITION = 1000;
    constexpr float MIN_WEIGHT = 0.05f;
    constexpr float REPLACE_WEIGHT_RATIO = 0.5f;

    const auto fesins = [&](Solution s){ return op::fesins(p, s); };
    // Available operators (heuristics)
    const auto operators = std::to_array<std::function<Solution(Solution)>>({
        [](auto s){ return op::ex2(s); },
        [](auto s){ return op::ins1(s); },
        [&p](Solution s){ return op::fesins(p, s); },
    });

    std::array<float, operators.size()> weights;
    for (auto& w : weights)
        w = 1.f / weights.size();


    auto best = genInitialSolution(p); // init to dummy solution
    auto localBest = best; // Init local best (incumbent) to dummy solution
    auto bestCost = getCost(p, best).val_or_max();
    auto localBestCost = bestCost;
    
    auto iterationsSinceNewBest = 0u;

    // Temperature is here the same as percentage for acceptance. Initial temperature is 0.99.
    double temperature = 0.70;
    // Cooling factor calculated from iterations and initial temperature
    double coolingFactor = std::exp(std::log(0.01 / temperature) / MAX_SEARCH);

    const auto rand = [&](){
        return ran() % 1000000 * 0.000001;
    };

    // Note: Annoying that c++ doesnt have const reference captures
    const auto selectOperatorIndex = [&weights = std::as_const(weights)](auto r) {
        auto acc{0.0};
        for (std::size_t i{0}; i < weights.size(); ++i)
            if (r < acc + weights[i])
                return i;
            else
                acc += weights[i];
        return weights.size() - 1;
    };

    const auto accept = [&](const auto& cost) {
        const auto p = temperature * cost / localBestCost;
        return cost < localBestCost || rand() < temperature;
    };

    for (unsigned int i{0}; i < MAX_SEARCH;) {
        std::array<std::pair<unsigned int, unsigned int>, weights.size()> scores{};
        // Just to be safe:
        for (auto& score : scores) score = {0u, 0u};

        for (unsigned int j{0}; j < SEGMENT_SIZE; ++i, ++j, ++iterationsSinceNewBest, temperature *= coolingFactor) {
            if (ESCAPE_CONDITION < iterationsSinceNewBest) {
                // Apply escape algorithm (something to bring us out of local optima)
                localBest = op::freorder(p, localBest);
                const auto cost = getCost(p, localBest);
                if (!cost)
                    throw std::logic_error{"What?"};
                localBestCost = cost.val_or_max();
                iterationsSinceNewBest = 0;
            }

            const auto r = rand();
            const auto opIndex = selectOperatorIndex(r);
            const auto& op = operators[opIndex];
            const auto current = op(localBest);
            unsigned int score = 0;

            const auto result = checkfeasibility(p, current);
            if (!result) {
                score += 1; // Get 1 score from finding a feasible solution
                const auto cost = getCost(p, current).val_or_max();
                
                if (cost < bestCost) {
                    score += 3;
                    best = current;
                    bestCost = cost;
                    iterationsSinceNewBest = 0;
                }

                // Acceptance criteria:
                if (accept(cost)) {
                    score += 1;
                    localBest = current;
                    localBestCost = cost;
                }
            }
            
            // Update scores
            scores[opIndex] += std::make_pair(score, 1u);
        }

        // After each segment, adjust the weights to next segment using scores form last segment and control r
        std::array<float, weights.size()> normalizedScores;
        // Normalize according to count
        for (auto j{0u}; j < weights.size(); ++j) {
            const auto& [score, count] = scores[j];
            normalizedScores[j] = count != 0 ? static_cast<float>(score) / count : 0.f;
        }
        // Normalize all scores so sum is 1
        const auto sumWeight = std::accumulate(normalizedScores.begin(), normalizedScores.end(), 0.f);
        for (auto& ns : normalizedScores)
            ns /= sumWeight;
        
        // Linear blend between old and new weights:
        for (auto [w, ns] : zip(weights, normalizedScores))
            w = (1.f - REPLACE_WEIGHT_RATIO) * w + REPLACE_WEIGHT_RATIO * ns;

        // Check if any weight is below minimum and if so, clamp and normalize again.
        if (std::any_of(weights.begin(), weights.end(), [MIN_WEIGHT](const auto& w){ return w < MIN_WEIGHT || 1.f < w; })) {
            float sum = 0.f;
            for (auto& w : weights)
                sum += w < MIN_WEIGHT ? MIN_WEIGHT : w;
            
            for (auto& w : weights)
                w = w < MIN_WEIGHT ? MIN_WEIGHT : w / sum;
        }
    }

    return best;
}

SolutionCached adaptiveCachedSearch(const Problem& p, std::default_random_engine& ran) {
    constexpr unsigned int MAX_SEARCH = 10000;
    constexpr unsigned int SEGMENT_SIZE = 100;
    constexpr unsigned int ESCAPE_CONDITION = 1000;
    constexpr float MIN_WEIGHT = 0.05f;
    constexpr float REPLACE_WEIGHT_RATIO = 0.5f;

    const auto fesins = [&](Solution s){ return op::fesins(p, s); };
    // Available operators (heuristics)
    using OperatorSignature = std::function<SolutionCached(SolutionCached)>;
    const auto operators = std::to_array<OperatorSignature>({
        [&ran](auto s){ return op::ex2(s, ran); },
        [&ran](auto s){ return op::ins1(s, ran); },
        [&p, &ran](auto s){ return op::fesins(p, s, ran); },
    });

    std::array<float, operators.size()> weights;
    for (auto& w : weights)
        w = 1.f / weights.size();


    auto best = genInitialSolutionCached(p); // init to dummy solution
    auto localBest = best; // Init local best (incumbent) to dummy solution
#ifndef NDEBUG
    const auto initialCostResult = getFeasibleCost(p, best);
    if (!initialCostResult)
        throw initialCostResult.err();
#endif
    auto bestCost = getFeasibleCost(p, best).val_or_max();
    auto localBestCost = bestCost;
    
    auto iterationsSinceNewBest = 0u;

    // Temperature is here the same as percentage for acceptance. Initial temperature is 0.99.
    double temperature = 0.70;
    // Cooling factor calculated from iterations and initial temperature
    double coolingFactor = std::exp(std::log(0.01 / temperature) / MAX_SEARCH);

    const auto rand = [&](){
        return ran() % 1000000 * 0.000001;
    };

    // Note: Annoying that c++ doesnt have const reference captures
    const auto selectOperatorIndex = [&weights = std::as_const(weights)](auto r) {
        auto acc{0.0};
        for (std::size_t i{0}; i < weights.size(); ++i)
            if (r < acc + weights[i])
                return i;
            else
                acc += weights[i];
        return weights.size() - 1;
    };

    const auto accept = [&](const auto& cost) {
        const auto p = temperature * cost / localBestCost;
        return cost < localBestCost || rand() < temperature;
    };

    for (unsigned int i{0}; i < MAX_SEARCH;) {
        std::array<std::pair<unsigned int, unsigned int>, weights.size()> scores{};
        // Just to be safe:
        for (auto& score : scores) score = {0u, 0u};

        for (unsigned int j{0}; j < SEGMENT_SIZE; ++i, ++j, ++iterationsSinceNewBest, temperature *= coolingFactor) {
//             if (ESCAPE_CONDITION < iterationsSinceNewBest) {
//                 // Apply escape algorithm (something to bring us out of local optima)
//                 localBest = op::freorder(p, localBest);
//                 const auto cost = getFeasibleCost(p, localBest);
// #ifndef NDEBUG
//                 if (!cost) {
//                     std::cout << cost.err().what() << std::endl;
//                     throw std::logic_error{"What?"};
//                 }
// #endif
//                 localBestCost = cost.val_or_max();
//                 iterationsSinceNewBest = 0;
//             }

            const auto r = rand();
            const auto opIndex = selectOperatorIndex(r);
            const auto& op = operators[opIndex];
            auto current = op(localBest);
            unsigned int score = 0;

            const auto result = getFeasibleCost(p, current);
            if (result) {
                score += 1; // Get 1 score from finding a feasible solution
                const auto cost = result.val_or_max();
                if (cost < bestCost) {
                    score += 3;
                    best = current;
                    bestCost = cost;
                    iterationsSinceNewBest = 0;
                }

                // Acceptance criteria:
                if (accept(cost)) {
                    score += 1;
                    localBest = current;
                    localBestCost = cost;
                }
            }
            
            // Update scores
            scores[opIndex] += std::make_pair(score, 1u);
        }

        // After each segment, adjust the weights to next segment using scores form last segment and control r
        std::array<float, weights.size()> normalizedScores;
        // Normalize according to count
        for (auto j{0u}; j < weights.size(); ++j) {
            const auto& [score, count] = scores[j];
            normalizedScores[j] = count != 0 ? static_cast<float>(score) / count : 0.f;
        }
        // Normalize all scores so sum is 1
        const auto sumWeight = std::accumulate(normalizedScores.begin(), normalizedScores.end(), 0.f);
        for (auto& ns : normalizedScores)
            ns /= sumWeight;
        
        // Linear blend between old and new weights:
        for (auto [w, ns] : zip(weights, normalizedScores))
            w = (1.f - REPLACE_WEIGHT_RATIO) * w + REPLACE_WEIGHT_RATIO * ns;

        // Check if any weight is below minimum and if so, clamp and normalize again.
        if (std::any_of(weights.begin(), weights.end(), [MIN_WEIGHT](const auto& w){ return w < MIN_WEIGHT || 1.f < w; })) {
            float sum = 0.f;
            for (auto& w : weights)
                sum += w < MIN_WEIGHT ? MIN_WEIGHT : w;
            
            for (auto& w : weights)
                w = w < MIN_WEIGHT ? MIN_WEIGHT : w / sum;
        }
    }

    return best;
}