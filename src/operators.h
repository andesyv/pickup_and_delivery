#pragma once
#include <functional>
#include <vector>
#include "problem.h"
#include <utility>
#include <type_traits>

namespace op {
Solution ex3(Solution s) {
    return s;
}

Solution ex2(Solution s) {
    return s;
}

Solution ins1(Solution s) {
    return s;
}
}

// Fuck yeah concepts!
template <typename F>
concept Operator = requires(F f) {
    std::function<Solution(Solution)>{f};
};