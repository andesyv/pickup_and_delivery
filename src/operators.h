#pragma once
#include <functional>
#include <vector>
#include "problem.h"
#include <utility>
#include <type_traits>

namespace op {
// 2-exchange operator
Solution ex2(Solution s);
SolutionComp ex2_comp(SolutionComp s);

// 3-exchange operator
Solution ex3(Solution s);
SolutionComp ex3_comp(SolutionComp s);

// 1-reinsert operator
Solution ins1(Solution s);
SolutionComp ins1_comp(SolutionComp s);

// Fuck yeah concepts!
template <typename F>
concept Operator = requires(F f) {
    std::function<Solution(Solution)>{f};
};
}