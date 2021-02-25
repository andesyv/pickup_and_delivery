#include <iostream>
#include <vector>
#include "problem.h"
#include "feasability.h"
#include <algorithm>
#include "cost.h"
#include <numeric>
#include "common.h"
#include <chrono>
#include "operators.h"

Solution blindRandomSearch(const Problem& p) {
    constexpr int MAX_SEARCH = 10000;
    auto best = genInitialSolution(p); // init to dummy solution
    auto cost = getCost(p, best).val_or_max();

    for (int i{0}; i < MAX_SEARCH; ++i) {
        auto current = genRandSolution(p);
        auto result = checkFeasability(p, current);
        if (!result) {
            auto newCost = getCost(p, current).val_or_max();
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

    auto best = genInitialSolution(p); // init to dummy solution
    auto cost = getCost(p, best).val_or_max();

    for (int i{0}; i < MAX_SEARCH; ++i) {
        const auto r = ran() % 100 * 0.01f;

        // Available operators
        const std::vector operators{
            op::ex2,
            op::ex3,
            op::ins1
        };

        const auto current = r < 0.4f ? operators[0](best) : r < 0.4f + 0.3f ? operators[1](best) : operators[2](best);
        const auto result = checkFeasability(p, current);
        if (!result) {
            auto newCost = getCost(p, current).val_or_max();
            if (newCost && newCost < cost) {
                best = current;
                cost = newCost;
            }
        }
    }

    return best;
}

int main() {
    std::cout << "Hello world!" << std::endl;

    for (const auto& file : {
        "./src/data/Call_7_Vehicle_3.txt",
        "./src/data/Call_18_Vehicle_5.txt",
        "./src/data/Call_035_Vehicle_07.txt",
        "./src/data/Call_080_Vehicle_20.txt",
        "./src/data/Call_130_Vehicle_40.txt"
        }) {
        
        std::cout << std::endl << std::endl << "File: " << file << std::endl;
        long long totalTime{0};
        long long totalCost{0};
        int bestCost{std::numeric_limits<int>::max()};
        std::vector<std::vector<int>> bestSolution{};

        auto pResult = load(file);
        if (!pResult) {
            std::cout << pResult.err().what() << std::endl;
            return 1;
        }
        auto problem = pResult.val();


        for (int i{0}; i < 10; ++i) {
            std::chrono::steady_clock::time_point t1{std::chrono::steady_clock::now()};
            const auto solution = localSearch(problem);
            // const auto solution = blindRandomSearch(problem);
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

            const auto cost = cResult.val();
            if (cost < bestCost) {
                bestCost = cost;
                bestSolution = solution;
            }
            std::cout << "Cost: " << cost << std::endl;
            totalCost += cost;
            
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            totalTime += ms;

            std::cout << "runtime: " << ms << "ms" << std::endl;
        }

        auto improvementPercent = [&](){
            auto initCost = getCost(problem, genInitialSolution(problem));
            auto bestCost = getCost(problem, bestSolution);

            return (initCost && bestCost) ? 100.0 * (initCost.val() - bestCost.val()) / initCost.val() : 0.0;
        };

        std::cout << std::endl << std::endl;

        std::cout << "Average cost: " << static_cast<double>(totalCost) / 10.0 << std::endl;
        std::cout << "Average runtime: " << static_cast<double>(totalTime) / 10.0 << "ms" << std::endl;
        std::cout << "Best cost: " << bestCost << std::endl;
        std::cout << "Best solution: ";
        for (const auto& v : fromNestedList(bestSolution))
            std::cout << v << ", ";
        std::cout << "Improvement (%): " << std::to_string(improvementPercent()) << std::endl;
        
    }

    return 0;
}