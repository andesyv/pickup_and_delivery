#include <iostream>
#include <vector>
#include "problem.h"
#include "cost.h"
#include <chrono>
#include "heuristics.h"
#include <thread>
#include <mutex>
#include <future>

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
        const auto problem = pResult.val();

        constexpr auto THREAD_COUNT = 10;
        std::vector<std::thread> threads{};
        threads.reserve(THREAD_COUNT);
        std::vector<std::future<int>> returnVals;
        std::mutex m;

        const auto loop = [&](std::promise<int>&& p) {
            std::chrono::steady_clock::time_point t1{std::chrono::steady_clock::now()};
            const auto solution = localSearch(problem);
            auto duration = std::chrono::steady_clock::now() - t1;

            // Scope so mutex lock can do it's thing.
            {
                std::lock_guard lock{m};

                std::cout << "Best solution: ";    
                for (auto l = fromNestedList(solution); auto v : l) {
                    std::cout << std::to_string(v) << ",";
                }
                std::cout << std::endl;
                auto cResult = getCost(problem, solution);
                if (!cResult) {
                    std::cout << cResult.err().what() << std::endl;
                    p.set_value(1);
                    return;
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

            // Remember to set return value
            p.set_value(0);
        };

        // Init promises (for return values for each thread)
        std::vector<std::promise<int>> promises;
        promises.resize(THREAD_COUNT);
        returnVals.reserve(THREAD_COUNT);

        // Init threads
        for (auto i{0}; i < THREAD_COUNT; ++i) {
            auto p = std::move(promises[i]);
            returnVals.push_back(p.get_future());
            threads.push_back(std::thread{loop, std::move(p)});
        }

        // Wait for all threads to finish
        for (auto& t : threads)
            t.join();
        
        for (auto& f : returnVals)
            if (0 != f.get())
                return f.get();

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