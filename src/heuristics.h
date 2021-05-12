#pragma once
#include "problem.h"
#include <random>

// Solution generation
Solution genInitialSolution(const Problem& p);
SolutionCached genInitialSolutionCached(const Problem& p);
Solution genRandSolution(const Problem& p);
Solution genRandSolution(const Problem& p, std::default_random_engine& engine);

// Searches
Solution blindRandomSearch(const Problem& p, std::default_random_engine& ran);
Solution localSearch(const Problem& p, std::default_random_engine& ran);
Solution simulatedAnnealing(const Problem& p, std::default_random_engine& ran);
Solution simulatedAnnealing2ElectricBoogaloo(const Problem& p, std::default_random_engine& ran);
Solution adaptiveSearch(const Problem& p);
Solution adaptiveSearch(const Problem& p, std::default_random_engine& engine);
SolutionCached adaptiveCachedSearch(const Problem& p, std::default_random_engine& engine);

using HeuristicParallelSignature = Solution (*)(const Problem&, std::default_random_engine&);
using HeuristicParallelSignatureCached = SolutionCached (*)(const Problem&, std::default_random_engine&);
using HeuristicSimpleSignature = Solution (*)(const Problem&);