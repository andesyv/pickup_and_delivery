#pragma once
#include "problem.h"

/// Basically the same code as in the feasibility check. (TODO: Refactor)
Result<int, std::runtime_error> getCost(const Problem &problem, const Solution& solution);