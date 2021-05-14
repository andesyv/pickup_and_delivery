#include <iostream>
#include <iomanip>
#include <vector>
#include "problem.h"
#include "cost.h"
#include <chrono>
#include "heuristics.h"
#ifdef PARALLEL_EXECUTION
#pragma message("PARALLEL_EXECUTION is enabled")
#include <thread>
#include <mutex>
#endif
#include <future>

#ifdef FILE_OUTPUT
#pragma message("Outputting to file is enabled")
#include <fstream>
#include <filesystem>
#endif

#ifdef RUN_FOR_10_MINUTES
#pragma message("Using all time available on run")
#endif

auto powi(auto n, auto e) {
    if (e <= 0)
        return 1;
    return n * powi(n, e-1);
}

/**
 * @brief Finds the available time the heuristic has available using the
 * sum of a geometric series.
 * @see https://en.wikipedia.org/wiki/Geometric_series#Sum
 * @param Seconds Compile time constant of total seconds available
 * @param R Compile time constant of geometric series ratio (r)
 * @param n Instance count
 * @param i Index
 */
template <long long Seconds, int R>
auto findAvailableTime(auto n, auto i) {
    const auto a = (1 - R) * Seconds / (1 - powi(R, n));
    return a * (1 - powi(R, i+1)) / (1 - R);
    // return a * powi(R, i);
}

int main(int argc, char *argv[])
{
    const auto program_start = std::chrono::high_resolution_clock::now();
    std::cout << "Hello world!" << std::endl;

#ifdef FILE_OUTPUT
    // Output file setup
    std::ofstream outf{"output.csv", std::ofstream::trunc | std::ofstream::out};
    if (!outf) {
        std::cout << "Failed to open output file \"output.csv\". Exiting." << std::endl;
        return 1;
    }

    std::ofstream outs{"solutions.txt", std::ofstream::trunc | std::ofstream::out};
    if (!outs) {
        std::cout << "Failed to open output file \"solutions.txt\". Exiting." << std::endl;
        return 1;
    }
#endif

    // Input files:
    std::vector<std::pair<const char*, long long>> files{
        {"./data/Call_7_Vehicle_3.txt", 0},
        {"./data/Call_18_Vehicle_5.txt", 0},
        {"./data/Call_035_Vehicle_07.txt", 0},
        {"./data/Call_080_Vehicle_20.txt", 0},
        {"./data/Call_130_Vehicle_40.txt", 0}
    };

    // Possibility to run with argument paths aswell
    // If that is the case, use them instead
    if (1 < argc)
    {
        files.clear();
        for (int i{1}; i < argc; ++i)
            files.emplace_back(argv[i], 0);
    }

#ifdef RUN_FOR_10_MINUTES
    // Calculate available time for each instance. (max milliseconds for each instance, floored)
    constexpr long long MAX_SECONDS = 9 * 60;
    constexpr auto MAX_TIME = std::chrono::duration_cast<TimeUnit>(std::chrono::seconds{MAX_SECONDS}).count();
    for (int i{0}; i < files.size(); ++i)
        files[i].second = findAvailableTime<MAX_TIME, 3>(files.size(), i);
#endif

    for (const auto [file, availableTime] : files)
    {
        std::cout << std::endl << file << std::endl;
        const bool bLastFile = file == files.back().first;

#ifdef FILE_OUTPUT
        // If file output is enabled, write some file handle info at the top
        outf << std::filesystem::path{file}.filename().string() << std::endl;
#ifdef PARALLEL_EXECUTION
        outf << "\"Name\",\"Average objective\",\"Best objective\",\"Improvement (%)\",\"Running time\"" << std::endl;
#else
        outf << "\"Name\",\"Best objective\",\"Improvement (%)\"" << std::endl;
#endif
#endif

        // Problem loading
        auto pResult = load(file);
        if (!pResult)
        {
            std::cout << pResult.err().what() << std::endl;
            return 1;
        }
        const auto problem = pResult.val();

#ifdef ALL_ALGORITHMS
#pragma message("Building with all search algorithms")

        const std::vector<std::pair<HeuristicParallelSignature, std::string>> searchAlgorithms{
            {blindRandomSearch, "Random Search"},
            {localSearch, "Local Search"},
            {simulatedAnnealing, "Simulated Annealing (old)"},
            {simulatedAnnealing2ElectricBoogaloo, "Simulated Annealing (new)"},
            {adaptiveSearch, "Adaptive Search"}
        };

        for (const auto& [search, algname] : searchAlgorithms)
        {
#ifndef NDEBUG
            std::cout << "Algorithm: " << algname << std::endl;
#endif

#else
            const auto& [search, algname] = std::make_pair<HeuristicParallelSignatureCached, std::string>(&adaptiveCachedSearch, "Adaptive Cached Search");
#endif

            long long totalTime{0};
            long long totalCost{0};
            int bestCost{std::numeric_limits<int>::max()};
            SolutionCached bestSolution{};

            constexpr unsigned int THREAD_COUNT = 10;

#ifdef PARALLEL_EXECUTION
            std::vector<std::thread> threads{};
            threads.reserve(THREAD_COUNT);
            std::mutex m;
#endif

            std::vector<std::future<int>> returnVals;

            const auto loop = [&](std::promise<int> &&p, std::default_random_engine&& ran, const std::chrono::high_resolution_clock::time_point* p_start = nullptr, long long availableTime = -1) {
                std::chrono::steady_clock::time_point t1{std::chrono::steady_clock::now()};
                auto solution = search(problem, ran, p_start, availableTime);
                auto duration = std::chrono::steady_clock::now() - t1;

                // Scope so mutex lock can do it's thing.
                {
#ifdef PARALLEL_EXECUTION
                    std::lock_guard lock{m};
#endif
#ifndef NDEBUG
                    std::cout << "Best solution: ";
                    for (auto l = fromNestedList(solution); auto v : l)
                    {
                        std::cout << std::to_string(v) << ",";
                    }
                    std::cout << std::endl;
#endif
                    auto cResult = getFeasibleCost(problem, solution);
#ifndef NDEBUG
                    if (!cResult)
                    {
                        std::cout << cResult.err().what() << std::endl;
                        p.set_value(1);
                        return;
                    }
#endif
                    const auto cost = cResult.val();
                    if (cost < bestCost)
                    {
                        bestCost = cost;
                        bestSolution = solution;
                    }
                    totalCost += cost;

                    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                    totalTime += ms;

#ifndef NDEBUG
                    std::cout << "Cost: " << cost << std::endl;
                    std::cout << "runtime: " << ms << "ms" << std::endl;
#endif
                }

                // Remember to set return value
                p.set_value(0);
            };

#ifdef PARALLEL_EXECUTION

            // Init promises (for return values for each thread)
            std::vector<std::promise<int>> promises;
            promises.resize(THREAD_COUNT);
            returnVals.reserve(THREAD_COUNT);

            // Init threads
            for (auto i{0}; i < THREAD_COUNT; ++i)
            {
                auto p = std::move(promises[i]);
                returnVals.push_back(p.get_future());
                auto ran = std::default_random_engine{static_cast<unsigned int>(std::time(nullptr) + i * 3)};
#ifdef RUN_FOR_10_MINUTES
                threads.push_back(std::thread{loop, std::move(p), std::move(ran), &program_start, availableTime});
#else
                threads.push_back(std::thread{loop, std::move(p), std::move(ran)});
#endif
            }

            // Wait for all threads to finish
            // for (auto& [t, f] : Zip{threads, returnVals}) {
            //     t.join();
            //     if (0 != f.get())
            //         return f.get();
            // }

            auto [tit, rit] = std::make_tuple(threads.begin(), returnVals.begin());
            for (; tit != threads.end() && rit != returnVals.end(); ++tit, ++rit)
            {
                auto &t = *tit;
                auto &f = *rit;
                t.join();
                if (0 != f.get())
                    return f.get();
            }

#else
            // If non-multithread, run loop manually. Once.
            std::promise<int> p{};
            auto f = p.get_future();
            loop(std::move(p), std::default_random_engine{static_cast<unsigned int>(std::time(nullptr))}, &program_start, availableTime);
            if (0 != f.get())
                return f.get();
#endif

            auto improvementPercent = [&]() {
                auto initCost = getCost(problem, genInitialSolution(problem));
                auto bestCost = getFeasibleCost(problem, bestSolution);

                return (initCost && bestCost) ? 100.0 * (initCost.val() - bestCost.val()) / initCost.val() : 0.0;
            };

#ifndef NDEBUG
            std::cout << std::endl
                      << std::endl;

#ifdef PARALLEL_EXECUTION
            std::cout << "Average cost: " << static_cast<double>(totalCost) / THREAD_COUNT << std::endl;
            std::cout << "Average runtime: " << static_cast<double>(totalTime) / THREAD_COUNT << "ms" << std::endl;
#endif
            std::cout << "Best cost: " << bestCost << std::endl;
            std::cout << "Best solution: ";
            for (const auto &v : fromNestedList(bestSolution))
                std::cout << v << ", ";
            std::cout << "Improvement (%): " << std::to_string(improvementPercent()) << std::endl;
#endif

#ifdef FILE_OUTPUT
            // Write results to file:
            outf << "\"" << algname << "\",";
            outf << totalCost / THREAD_COUNT << ",";
#ifdef PARALLEL_EXECUTION            
            outf << bestCost << ",";
#endif
            outf << std::fixed << std::setprecision(2) << improvementPercent();
#ifdef PARALLEL_EXECUTION
            outf << "," << static_cast<double>(totalTime) / 10.0 << "ms";
#endif
            outf << std::endl;

            // Write solutions to file
            for (const auto &v : fromNestedList(bestSolution))
                outs << v << " ";
            outs << std::endl;

#endif
#ifdef ALL_ALGORITHMS
        }
#endif
#ifdef FILE_OUTPUT
        outf << std::endl;
#endif

    }

    std::cout << "Program execution time: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - program_start).count() << " seconds." << std::endl;

    return 0;
}