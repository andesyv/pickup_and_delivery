#pragma once
#include <functional>
#include <vector>
#include "problem.h"
#include <utility>
#include <type_traits>
#include <random>

namespace op {
// 2-exchange operator
Solution ex2(Solution s);
SolutionComp ex2_comp(SolutionComp s);

// 3-exchange operator
Solution ex3(Solution s);
SolutionComp ex3_comp(SolutionComp s);

// 1-reinsert operator
Solution ins1(Solution s);
Solution ins1(Solution s, const std::default_random_engine& engine);
SolutionComp ins1_comp(SolutionComp s);

/**
 * @brief Feasible reinsert
 * Like the 1-reinsert but attempts to also create more feasible solutions
 * @param s Previous solution
 * @return New (maybe) feasible solution
 */
Solution fesins(const Problem& p, Solution s);

/**
 * @brief time-window reorder
 * 2-exchange that attempts to reorder solution based on time-slots
 * @param p 
 * @param s 
 * @return Solution 
 */
Solution freorder(const Problem& p, Solution s);

/**
 * @brief Multi-threading shuffle
 * Uses multiple threads to simultaneously generate a bunch of
 * random solutions and chooses the most promising looking one.
 */
Solution multishuffle(const Problem& p, Solution s);

// Fuck yeah concepts!
template <typename F>
concept Operator = requires(F f) {
    std::function<Solution(Solution)>{f};
};
}