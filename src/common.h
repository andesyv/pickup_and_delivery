#pragma once
#include "problem.h"
#include <vector>
#include <random>
#include <ctime>

template <std::size_t I>
Solution toNestedList(const int (&a)[I]) {
    Solution out;
    std::size_t beg{0};
    for (index_t end{0}; end <= I; ++end) { // Allow for one past end as this copies the range before the end
        if (end == I || a[end] == 0) {
            out.push_back(std::vector<int>{a + beg, a + end});
            std::transform(out.back().begin(), out.back().end(), out.back().begin(),
            [](const auto& i){ return i-1; });

            beg = end + 1;
        }
    }
    return out;
}

std::vector<int> fromNestedList(const Solution& list) {
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