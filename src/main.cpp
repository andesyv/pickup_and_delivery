#include <iostream>
#include <vector>
#include "problem.h"
#include "feasability.h"
#include <algorithm>
#include "cost.h"
#include <numeric>
#include "common.h"
#include <chrono>

std::pair<std::vector<std::vector<int>>, double> blindRandomSearch(const Problem& p) {
    constexpr int MAX_SEARCH = 100000;
    auto best = genInitialSolution(p); // init to dummy solution
    auto cost = getCost(p, best);

    std::pair<unsigned long long, int> totalCost{cost, 1};

    for (int i{0}; i < MAX_SEARCH; ++i) {
        auto current = genRandSolution(p);
        if (!checkFeasability(p, current)) {
            auto newCost = getCost(p, current);
            totalCost.first += newCost;
            totalCost.second += 1;
            if (newCost < cost)
                best = current;
        }
    }

    return {best, totalCost.first / static_cast<double>(totalCost.second)};
}

int main() {
    std::cout << "Hello world!" << std::endl;

    for (const auto& file : {
        "./src/data/Call_7_Vehicle_3.txt"
        }) {
        
        std::cout << "File: " << file << std::endl;
        long long totalTime{0};

        for (int i{0}; i < 10; ++i) {
            auto pResult = load(file);
            if (!pResult) {
                std::cout << pResult.err().what() << std::endl;
                return 1;
            }
            auto problem = pResult.val();

            std::chrono::steady_clock::time_point t1{std::chrono::steady_clock::now()};
            const auto [solution, avgCost] = blindRandomSearch(problem);
            auto duration = std::chrono::steady_clock::now() - t1;

            std::cout << "Best solution: ";    
            for (auto l = fromNestedList(solution); auto v : l) {
                std::cout << std::to_string(v) << ",";
            }
            std::cout << std::endl;
            auto cResult = getCost(problem, solution);
            if (!cResult) {
                std::cout << cResult.err().what() << std::endl;
                return 1;
            }
            std::cout << "Cost: " << cResult.val() << std::endl;
            std::cout << "Avg cost: " << avgCost << std::endl;
            
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            totalTime += ms;

            std::cout << "runtime: " << ms << "ms" << std::endl;
        }

        std::cout << "Average runtime: " << static_cast<double>(totalTime) / 10.0 << "ms" << std::endl;
    }

    return 0;
}