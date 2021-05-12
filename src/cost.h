#pragma once
#include "problem.h"

/// Basically the same code as in the feasibility check. (TODO: Refactor)
Result<int, std::runtime_error> getCost(const Problem &problem, const Solution& solution);
Result<int, std::runtime_error> getCost(const Problem &problem, const SolutionComp& solution);

// Checks feasibilty and cost at the same time. Lack of feasibility gives a runtime_error.
#ifndef NDEBUG
using FeasibilityCostRet = Result<int, std::runtime_error>;
#else
// using FeasibilityCostRet = std::optional<int>;
class FeasibilityCostRet : public std::optional<int> {
public:
    FeasibilityCostRet(const int& other) : std::optional<int>{other} {}
    FeasibilityCostRet(int&& other) : std::optional<int>{std::move(other)} {}
    FeasibilityCostRet(std::nullopt_t) : std::optional<int>{std::nullopt} {}

    auto val() const { return *(*this); }
    auto val_or_max() const { return *(*this); }
};
#endif
FeasibilityCostRet getFeasibleCost(const Problem& p, SolutionCached& s);