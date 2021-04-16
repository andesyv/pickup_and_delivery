#pragma once
#include "problem.h"

// Solution generation
Solution genInitialSolution(const Problem& p);
Solution genRandSolution(const Problem& p);

// Searches
Solution blindRandomSearch(const Problem& p);
Solution localSearch(const Problem& p);
Solution simulatedAnnealing(const Problem& p);
Solution simulatedAnnealing2ElectricBoogaloo(const Problem& p);
Solution adaptiveSearch(const Problem& p);