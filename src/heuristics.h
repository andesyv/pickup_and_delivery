#pragma once
#include "problem.h"
#include <random>
#include <chrono>

// Solution generation
Solution genInitialSolution(const Problem& p);
SolutionCached genInitialSolutionCached(const Problem& p);
Solution genRandSolution(const Problem& p);
Solution genRandSolution(const Problem& p, std::default_random_engine& engine);
SolutionCached genRandSolutionCached(const Problem& p, std::default_random_engine& engine);

// Searches
Solution blindRandomSearch(const Problem& p, std::default_random_engine& ran);
Solution localSearch(const Problem& p, std::default_random_engine& ran);
Solution simulatedAnnealing(const Problem& p, std::default_random_engine& ran);
Solution simulatedAnnealing2ElectricBoogaloo(const Problem& p, std::default_random_engine& ran);
Solution adaptiveSearch(const Problem& p);
Solution adaptiveSearch(const Problem& p, std::default_random_engine& engine);
SolutionCached adaptiveCachedSearch(const Problem& p, std::default_random_engine& engine, const std::chrono::high_resolution_clock::time_point* p_start = nullptr, long long availableTime = -1);

using HeuristicParallelSignature = Solution (*)(const Problem&, std::default_random_engine&);
using HeuristicParallelSignatureCached = SolutionCached (*)(const Problem&, std::default_random_engine&, const std::chrono::high_resolution_clock::time_point*, long long);
using HeuristicSimpleSignature = Solution (*)(const Problem&);

using TimeUnit = std::chrono::microseconds;