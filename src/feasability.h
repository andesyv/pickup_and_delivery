#pragma once
#include "problem.h"
#include <optional>

// Returns nothing if feasable, error otherwise
std::optional<std::runtime_error> checkFeasability(const Problem& problem, const Solution& solution);

template <typename T>
std::optional<std::runtime_error> checkFeasability(const Problem& p, const T& s) {
    return checkFeasability(p, toNestedList(s));
}