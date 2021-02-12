#pragma once
#include "problem.h"
#include <vector>
#include <random>

std::vector<std::vector<int>> genInitialSolution(const Problem& p) {
    std::vector<std::vector<int>> routes;
    routes.resize(p.vehicles.size()+1);

    for (int i{0}; i < p.calls.size(); ++i)
        routes.back().push_back(i);
    return routes;
}

std::vector<std::vector<int>> genRandSolution(const Problem& p) {
    static std::default_random_engine ran{1234567890};

    std::vector<std::vector<int>> routes;
    const auto vSize{p.vehicles.size()};
    routes.resize(vSize+1);

    std::vector<int> remainingCalls{};
    remainingCalls.reserve(p.calls.size());
    for (int i{0}; i < p.calls.size(); ++i)
        remainingCalls.push_back(i);
    std::shuffle(remainingCalls.begin(), remainingCalls.end(), ran);

    while (!remainingCalls.empty()) {
        routes.at(ran() % vSize).push_back(remainingCalls.back());
        remainingCalls.pop_back();
    }

    for (auto& route : routes) {
        route.insert(route.end(), route.begin(), route.end());
        // Reshuffle each route after duplicating:
        std::shuffle(route.begin(), route.end(), ran);
    }

    return routes;
}