#pragma once
#include "problem.h"
#include <vector>
#include <random>

template <std::size_t I>
std::vector<std::vector<int>> toNestedList(const int (&a)[I]) {
    std::vector<std::vector<int>> out;
    std::size_t beg{0};
    for (uint8_t end{0}; end <= I; ++end) { // Allow for one past end as this copies the range before the end
        if (end == I || a[end] == 0) {
            out.push_back(std::vector<int>{a + beg, a + end});
            std::transform(out.back().begin(), out.back().end(), out.back().begin(),
            [](const auto& i){ return i-1; });

            beg = end + 1;
        }
    }
    return out;
}

std::vector<int> fromNestedList(const std::vector<std::vector<int>>& list) {
    std::vector<int> out;
    out.reserve(std::accumulate(list.begin(), list.end(), std::size_t{0}, [](const auto& a, const auto& b){
        return a + b.size() + 1;
    }));
    for (std::size_t i{0}; i < list.size(); ++i) {
        auto inner = list.at(i);
        for (const auto& v : inner)
            out.push_back(v+1);
        if (i < list.size() - 1)
            out.push_back(0);
    }
    return out;
}

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