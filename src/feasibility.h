#pragma once
#include "problem.h"
#include <optional>

// Returns nothing if feasible, error otherwise
std::optional<std::runtime_error> checkfeasibility(const Problem& problem, const Solution& solution);

template <typename T>
std::optional<std::runtime_error> checkfeasibility(const Problem& p, const T& s) {
    return checkfeasibility(p, toNestedList(s));
}