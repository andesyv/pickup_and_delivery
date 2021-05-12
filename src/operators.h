#pragma once
#include <functional>
#include <vector>
#include "problem.h"
#include <utility>
#include <type_traits>
#include <random>

namespace op {

/// Legacy operators:
// 2-exchange operator
Solution ex2(Solution s);
Solution ex2(Solution s, std::default_random_engine& engine);
SolutionCached ex2(SolutionCached s, std::default_random_engine& engine);
SolutionComp ex2_comp(SolutionComp s);
bool exchance(std::vector<int>& c1, std::vector<int>& c2);

// 3-exchange operator
Solution ex3(Solution s);
Solution ex3(Solution s, std::default_random_engine& engine);
SolutionComp ex3_comp(SolutionComp s);

// 1-reinsert operator
Solution ins1(Solution s);
Solution ins1(Solution s, std::default_random_engine& engine);
SolutionCached ins1(SolutionCached s, std::default_random_engine& engine);
SolutionComp ins1_comp(SolutionComp s);


/// Custom operators:
/**
 * @brief Feasible reinsert
 * Like the 1-reinsert but attempts to also create more feasible solutions
 * @param s Previous solution
 * @return New (maybe) feasible solution
 */
Solution fesins(const Problem& p, Solution s);
SolutionCached fesins(const Problem& p, SolutionCached s, std::default_random_engine& engine);

/**
 * @brief time-window reorder
 * 2-exchange that attempts to reorder solution based on time-slots
 * @param p 
 * @param s 
 * @return Solution 
 */
Solution freorder(const Problem& p, Solution s);
SolutionCached freorder(const Problem& p, SolutionCached s);

/**
 * @brief Multi-threading shuffle
 * Uses multiple threads to simultaneously generate a bunch of
 * random solutions and chooses the most promising looking one.
 */
Solution multishuffle(const Problem& p, Solution s);

/**
 * @brief Swaps or moves from car to dummy car
 * Takes a random car and either moves the call to the dummy,
 * or swaps with a random call from the dummy.
 * @param p 
 * @param s 
 * @return Solution 
 */
Solution backinsert(const Problem& p, Solution s);

Solution scramble(const Problem& p, Solution s);


// Fuck yeah concepts!
template <typename F>
concept Operator = requires(F f) {
    std::function<Solution(Solution)>{f};
};
}