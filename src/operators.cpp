#include "operators.h"
#include <algorithm>
#include <limits>
#include <random>
#include <ctime>
#include <numeric>
#include <set>
#include <array>
#include <optional>
#include <thread>
#include <future>
#include "feasibility.h"
#include "cost.h"

template <typename T>
auto find_nested_minmax(const T& begin, const T& end){
    using itT = typename T::value_type;
    using vT = typename itT::value_type;
    std::pair<vT, vT> vals{
        std::numeric_limits<vT>::max(),
        std::numeric_limits<vT>::min()
    };
    for (auto it{begin}; it != end; ++it) {
        for (const auto& l = *it; const auto& v : l) {
            if (v < vals.first)
                vals.first = v;
            if (vals.second < v)
                vals.second = v;
        }
    }
    return begin == end ? std::pair<vT, vT>{} : vals;
}

template <>
auto find_nested_minmax<typename SolutionCached::iterator>(const typename SolutionCached::iterator& begin, const typename SolutionCached::iterator& end){
    using itT = typename SolutionCached::value_type;
    std::pair vals{
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min()
    };
    for (auto it{begin}; it != end; ++it) {
        for (const auto& l = it->calls; const auto& v : l) {
            if (v < vals.first)
                vals.first = v;
            if (vals.second < v)
                vals.second = v;
        }
    }
    return begin == end ? std::pair<int, int>{} : vals;
}

// minmax for compact solution representation
template <typename T>
auto find_minmax(const T& begin, const T& end) {
    using vT = typename T::value_type;
    std::pair<vT, vT> vals{
        std::numeric_limits<vT>::max(),
        std::numeric_limits<vT>::min()
    };

    for (auto it{begin}; it != end; ++it) {
        const auto& v{*it};
        if (-1 < v && v < vals.first)
            vals.first = v;
        if (vals.second < v)
            vals.second = v;
    }

    return begin == end ? std::pair<vT, vT>{} : vals;
}

static std::default_random_engine ran{static_cast<unsigned int>(std::time(nullptr))};

namespace op {
// 2-exchange operator
Solution ex2(Solution s) {
    return ex2(s, ran);
}

Solution ex2(Solution s, std::default_random_engine& ran) {
    const auto [min, max] = find_nested_minmax(s.begin(), s.end());
    if (max - min < 2)
        return s;

    // Find two random call ids
    const auto r = [&](){ return ran() % (max + 1 - min) + min; };
    auto a{r()}, b{r()};
    // Loop until you have two different ones:
    // Note: If really unlucky might loop for a long time
    while (a == b)
        b = r();

    // It should be guaranteed that the random calls are represented in the solution (else it would be invalid)
    std::pair<int*, int*>   apos{std::make_pair(nullptr, nullptr)},
                            bpos{std::make_pair(nullptr, nullptr)};
    for (auto& l : s) {
        for (auto it{l.begin()}; it != l.end(); ++it) {
            const auto& v = *it;

            if (v == a)
                if (!apos.first)
                    apos.first = &(*it);
                else
                    apos.second = &(*it);

            if (v == b)
                if (!bpos.first)
                    bpos.first = &(*it);
                else
                    bpos.second = &(*it);
        }
    }

#ifdef _DEBUG
    // Early termination in case we somehow ended up with nullptr's
    for (const auto& [a, b] : {apos, bpos})
        if (!a || !b)
            throw std::runtime_error{"Attempting to swap nullptr"};
#endif

    // Swap first:
    std::swap(*apos.first, *bpos.first);
    // Swap last:
    std::swap(*apos.second, *bpos.second);

    return s;
}

SolutionCached ex2(SolutionCached s, std::default_random_engine& ran) {
    const auto [min, max] = find_nested_minmax(s.begin(), s.end());
    if (max - min < 2)
        return s;

    // Find two random call ids
    const auto r = [&](){ return ran() % (max + 1 - min) + min; };
    auto a{r()}, b{r()};
    // Loop until you have two different ones:
    // Note: If really unlucky might loop for a long time
    while (a == b)
        b = r();

    // It should be guaranteed that the random calls are represented in the solution (else it would be invalid)
    std::pair<int*, int*>   apos{std::make_pair(nullptr, nullptr)},
                            bpos{std::make_pair(nullptr, nullptr)};
    for (auto& l : s) {
        for (auto it{l.calls.begin()}; it != l.calls.end(); ++it) {
            const auto& v = *it;

            if (v == a) {
                if (!apos.first)
                    apos.first = &(*it);
                else
                    apos.second = &(*it);
                l.bChanged = true;
            }

            if (v == b) {
                if (!bpos.first)
                    bpos.first = &(*it);
                else
                    bpos.second = &(*it);
                l.bChanged = true;
            }
        }
    }

#ifdef _DEBUG
    // Early termination in case we somehow ended up with nullptr's
    for (const auto& [a, b] : {apos, bpos})
        if (!a || !b)
            throw std::runtime_error{"Attempting to swap nullptr"};
#endif

    // Swap first:
    std::swap(*apos.first, *bpos.first);
    // Swap last:
    std::swap(*apos.second, *bpos.second);

    return s;
}

SolutionComp ex2_comp(SolutionComp s) {
    const auto [min, max] = find_minmax(s.begin(), s.end());
    if (max - min < 2)
        return s;

    // Find two random call ids
    const auto r = [&](){ return ran() % (max + 1 - min) + min; };
    auto a{r()}, b{r()};
    // Loop until you have two different ones:
    // Note: If really unlucky might loop for a long time
    while (a == b)
        b = r();

    // It should be guaranteed that the random calls are represented in the solution (else it would be invalid)
    std::pair<int*, int*>   apos{std::make_pair(nullptr, nullptr)},
                            bpos{std::make_pair(nullptr, nullptr)};
    for (auto it{s.begin()}; it != s.end(); ++it) {
        const auto& v = *it;
        if (v < 0)
            continue;

        if (v == a)
            if (!apos.first)
                apos.first = &(*it);
            else
                apos.second = &(*it);

        if (v == b)
            if (!bpos.first)
                bpos.first = &(*it);
            else
                bpos.second = &(*it);
    }

#ifdef _DEBUG
    // Early termination in case we somehow ended up with nullptr's
    for (const auto& [a, b] : {apos, bpos})
        if (!a || !b)
            throw std::runtime_error{"Attempting to swap nullptr"};
#endif

    // Swap first:
    std::swap(*apos.first, *bpos.first);
    // Swap last:
    std::swap(*apos.second, *bpos.second);

    return s;
}

bool exchance(std::vector<int>& c1, std::vector<int>& c2) {
    if (c1.empty() || c2.empty())
        return false;

    const auto  a{c1.at(ran() % c1.size())},
                b{c2.at(ran() % c2.size())};

    // It should be guaranteed that the random calls are represented in the solution (else it would be invalid)
    std::pair<int*, int*>   apos{std::make_pair(nullptr, nullptr)},
                            bpos{std::make_pair(nullptr, nullptr)};
    
    for (auto it{c1.begin()}; it != c1.end(); ++it) {
        auto& v = *it;
        if (v == a)
            if (!apos.first)
                apos.first = &v;
            else
                apos.second = &v;
    }

    for (auto it{c2.begin()}; it != c2.end(); ++it) {
        auto& v = *it;
        if (v == b)
            if (!bpos.first)
                bpos.first = &v;
            else
                bpos.second = &v;
    }

#ifdef _DEBUG
    // Early termination in case we somehow ended up with nullptr's
    for (const auto& [a, b] : {apos, bpos})
        if (!a || !b)
            throw std::runtime_error{"Attempting to swap nullptr"};
#endif

    // Swap first:
    std::swap(*apos.first, *bpos.first);
    // Swap last:
    std::swap(*apos.second, *bpos.second);

#ifdef _DEBUG
    if (c1.size() % 2 || c2.size() % 2)
        throw std::runtime_error{"Invalid solution!"};
#endif

    return true;
}

// 3-exchange operator
Solution ex3(Solution s) {
    return ex3(s, ran);
}

Solution ex3(Solution s, std::default_random_engine& ran) {
    const auto [min, max] = find_nested_minmax(s.begin(), s.end());
    if (max - min < 3)
        return s;

    // Find three random call ids
    const auto r = [&](){ return ran() % (max + 1 - min) + min; };
    auto a{r()}, b{r()}, c{r()};
    // Loop until you have three different ones:
    // Note: If really unlucky might loop for a long time
    while (a == b)
        b = r();
    while (a == c || b == c)
        c = r();

    // It should be guaranteed that the random calls are represented in the solution (else it would be invalid)
    std::pair<int*, int*>   apos{std::make_pair(nullptr, nullptr)},
                            bpos{std::make_pair(nullptr, nullptr)},
                            cpos{std::make_pair(nullptr, nullptr)};
    for (auto& l : s) {
        for (auto it{l.begin()}; it != l.end(); ++it) {
            const auto& v = *it;

            if (v == a)
                if (!apos.first)
                    apos.first = &(*it);
                else
                    apos.second = &(*it);

            if (v == b)
                if (!bpos.first)
                    bpos.first = &(*it);
                else
                    bpos.second = &(*it);

            if (v == c)
                if (!cpos.first)
                    cpos.first = &(*it);
                else
                    cpos.second = &(*it);
        }
    }

#ifdef _DEBUG
    // Early termination in case we somehow ended up with nullptr's
    for (const auto& [a, b] : {apos, bpos, cpos})
        if (!a || !b)
            throw std::runtime_error{"Attempting to swap nullptr"};
#endif

    // Swap first:
    std::swap(*apos.first, *bpos.first);
    std::swap(*bpos.first, *cpos.first);
    // Swap last:
    std::swap(*apos.second, *bpos.second);
    std::swap(*bpos.second, *cpos.second);

    return s;
}

SolutionComp ex3_comp(SolutionComp s) {
    const auto [min, max] = find_minmax(s.begin(), s.end());
    if (max - min < 3)
        return s;

    // Find three random call ids
    const auto r = [&](){ return ran() % (max + 1 - min) + min; };
    auto a{r()}, b{r()}, c{r()};
    // Loop until you have three different ones:
    // Note: If really unlucky might loop for a long time
    while (a == b)
        b = r();
    while (a == c || b == c)
        c = r();

    // It should be guaranteed that the random calls are represented in the solution (else it would be invalid)
    std::pair<int*, int*>   apos{std::make_pair(nullptr, nullptr)},
                            bpos{std::make_pair(nullptr, nullptr)},
                            cpos{std::make_pair(nullptr, nullptr)};
    for (auto it{s.begin()}; it != s.end(); ++it) {
        const auto& v = *it;
        if (v < 0)
            continue;

        if (v == a)
            if (!apos.first)
                apos.first = &(*it);
            else
                apos.second = &(*it);

        if (v == b)
            if (!bpos.first)
                bpos.first = &(*it);
            else
                bpos.second = &(*it);

        if (v == c)
            if (!cpos.first)
                cpos.first = &(*it);
            else
                cpos.second = &(*it);
    }

#ifdef _DEBUG
    // Early termination in case we somehow ended up with nullptr's
    for (const auto& [a, b] : {apos, bpos, cpos})
        if (!a || !b)
            throw std::runtime_error{"Attempting to swap nullptr"};
#endif

    // Swap first:
    std::swap(*apos.first, *bpos.first);
    std::swap(*bpos.first, *cpos.first);
    // Swap last:
    std::swap(*apos.second, *bpos.second);
    std::swap(*bpos.second, *cpos.second);

    return s;
}

// 1-reinsert operator
Solution ins1(Solution s) {
    return ins1(s, ran);
}

Solution ins1(Solution s, std::default_random_engine& ran) {
    const auto [min, max] = find_nested_minmax(s.begin(), s.end());
    if (min == max)
        return s;

    // Find a random call id
    const auto a = ran() % (max + 1 - min) + min;

    // Remove from list:
    for (auto& l : s)
        std::erase(l, a);
    
    // Insert two of call id into random car:
    auto& car = s[ran() % s.size()];
    // Optional hint to compiler to add more make next two inserts cheaper
    car.reserve(car.size() + 2);
    if (car.empty())
        car.push_back(a);
    else
        car.insert(car.begin() + ran() % car.size(), a);

    // No reason to check second time because size will atleast be 1
    car.insert(car.begin() + ran() % car.size(), a);

    return s;
}

SolutionCached ins1(SolutionCached s, std::default_random_engine& ran) {
    const auto [min, max] = find_nested_minmax(s.begin(), s.end());
    if (min == max)
        return s;

    // Find a random call id
    const auto a = ran() % (max + 1 - min) + min;

    // Remove from list:
    for (auto& l : s)
        if (0 < std::erase(l.calls, a))
            l.bChanged = true;
    
    // Insert two of call id into random car:
    const auto ranIndex = ran() % s.size();
    auto& car = s[ranIndex].calls;
    // Optional hint to compiler to add more make next two inserts cheaper
    car.reserve(car.size() + 2);
    if (car.empty())
        car.push_back(a);
    else
        car.insert(car.begin() + ran() % car.size(), a);

    // No reason to check second time because size will atleast be 1
    car.insert(car.begin() + ran() % car.size(), a);

    s[ranIndex].bChanged = true;

    return s;
}

SolutionComp ins1_comp(SolutionComp s) {
    return fromNestedListZeroIndexed(ins1(toNestedList(s)));

    // Currently can't get this shit to work, so commented out for the time being
/*
    const auto [min, max] = find_minmax(s.begin(), s.end());
    if (min == max)
        return s;

    // Find a random call id
    const auto a = ran() % (max + 1 - min) + min;
    
    std::vector<std::pair<int, int>> carIndexes;
    // Count cars and pos of a:
    auto aindex{-1}, start{0};
    for (auto i{0}; i < s.size(); ++i) {
        if (s[i] == a && aindex < 0)
            aindex = i;
        if (s[i] == -1) {
            carIndexes.emplace_back(start, start + 1);
            start = i + 1;
        }
    }
    // Dummy vehicle at the end
    carIndexes.emplace_back(start, s.size());

    // Choose a random car:
    const auto car = ran() % carIndexes.size();

    // Choose 2 random indexes in that car:
    const auto cindex = carIndexes[car];
    const auto i1 = cindex.first + ran() % (cindex.second - cindex.first);
    auto i2{i1};
    while (i1 == i2)
        i2 = cindex.first + ran() % (cindex.second - cindex.first + 1); // Offset by one because of i1

    const bool forward = aindex <= i1 && aindex <= i2;
    
    if (forward) {
        for (int i{0}, offset{0}; i < s.size(); ++i) {
            if (s[i] == a) {
                offset -= 1;
                continue;
            }

            if (offset != 0) {
                if (i == i1 || i == i2) {
                    s[i] = a;
                    offset += 1;
                } else {
                    s[i + offset] = s[i];
                }
            }
        }
    } else {
        for (int i{static_cast<int>(s.size()) - 1}, offset{0}; 0 < i; --i) {
            if (s[i] == a) {
                offset += 1;
                continue;
            }

            if (offset != 0) {
                if (i == i1 || i == i2) {
                    s[i] = a;
                    offset -= 1;
                } else {
                    s[i + offset] = s[i];
                }
            }
        }
    }

    return s;
    */
}

Solution fesins(const Problem& p, Solution s) {
    const auto [min, max] = find_nested_minmax(s.begin(), s.end());
    if (min == max)
        return s;

    // Find a random call id
    const auto a = ran() % (max + 1 - min) + min;

    // Remove from list:
    for (auto& l : s)
        std::erase(l, a);
    
    // Find possible cars:
    std::vector<unsigned int> carIds;
    carIds.reserve(s.size());
    for (auto i{0}; i < p.vehicles.size(); ++i) {
        // If vehicle has call, add it to list.
        const auto& ac = p.vehicles[i].availableCalls;
        if (std::find(ac.begin(), ac.end(), a) != ac.end())
            carIds.push_back(i);
    }
    // Also add dummy
    carIds.push_back(static_cast<unsigned int>(p.vehicles.size()));

    // Find the one with least momentary weight ratio (filter out ones that already have too much weight):
    // Weight ratio is calculated as max(currentWeight_0, currentWeight_1, ..., currentWeight_n) / maxWeight
    auto leastWeightRatio{std::make_pair(std::numeric_limits<double>::max(), carIds.size() - 1)};
    for (unsigned int i{0}; i < carIds.size() - 1; ++i) {
        const auto maxCapacity = p.vehicles[i].capacity;
        int capacity{0};
        double ratio{0.0};
        std::set<int> calls{};
        for (const auto& route{s[i]}; const auto call : route) {
            if (calls.insert(call).second) {
                capacity += p.calls[call].size;
                // Early exit if already above max capacity
                if (maxCapacity < capacity) {
                    ratio = 2.0;
                    break;
                }
                ratio = std::max(ratio, static_cast<double>(capacity) / maxCapacity);
            } else
                capacity -= p.calls[call].size;
        }

        if (ratio < leastWeightRatio.first && ratio < 1.0)
            leastWeightRatio = {ratio, i};
    }

    // Insert two of call id into random car:
    auto& car = s[carIds[leastWeightRatio.second]];
    // Optional hint to compiler to add more make next two inserts cheaper
    car.reserve(car.size() + 2);
    if (car.empty())
        car.push_back(a);
    else
        car.insert(car.begin() + ran() % car.size(), a);

    // No reason to check second time because size will atleast be 1
    car.insert(car.begin() + ran() % car.size(), a);

    return s;
}

SolutionCached fesins(const Problem& p, SolutionCached s, std::default_random_engine& ran) {
    const auto [min, max] = find_nested_minmax(s.begin(), s.end());
    if (min == max)
        return s;

    // Find a random call id
    const auto a = ran() % (max + 1 - min) + min;

    // Remove from list:
    for (auto& l : s)
        if (0 < std::erase(l.calls, a))
            l.bChanged = true;
    
    // Find possible cars:
    std::vector<unsigned int> carIds;
    carIds.reserve(s.size());
    for (auto i{0}; i < p.vehicles.size(); ++i) {
        // If vehicle has call, add it to list.
        const auto& ac = p.vehicles[i].availableCalls;
        if (std::find(ac.begin(), ac.end(), a) != ac.end())
            carIds.push_back(i);
    }
    // Also add dummy
    carIds.push_back(static_cast<unsigned int>(p.vehicles.size()));

    // Find the one with least momentary weight ratio (filter out ones that already have too much weight):
    // Weight ratio is calculated as max(currentWeight_0, currentWeight_1, ..., currentWeight_n) / maxWeight
    auto leastWeightRatio{std::make_pair(std::numeric_limits<double>::max(), carIds.size() - 1)};
    for (unsigned int i{0}; i < carIds.size() - 1; ++i) {
        const auto maxCapacity = p.vehicles[i].capacity;
        int capacity{0};
        double ratio{0.0};
        std::set<int> calls{};
        for (const auto& route{s[i].calls}; const auto call : route) {
            if (calls.insert(call).second) {
                capacity += p.calls[call].size;
                // Early exit if already above max capacity
                if (maxCapacity < capacity) {
                    ratio = 2.0;
                    break;
                }
                ratio = std::max(ratio, static_cast<double>(capacity) / maxCapacity);
            } else
                capacity -= p.calls[call].size;
        }

        if (ratio < leastWeightRatio.first && ratio < 1.0)
            leastWeightRatio = {ratio, i};
    }

    // Insert two of call id into random car:
    auto& car = s[carIds[leastWeightRatio.second]].calls;
    // Optional hint to compiler to add more make next two inserts cheaper
    car.reserve(car.size() + 2);
    if (car.empty())
        car.push_back(a);
    else
        car.insert(car.begin() + ran() % car.size(), a);

    // No reason to check second time because size will atleast be 1
    car.insert(car.begin() + ran() % car.size(), a);

    s[carIds[leastWeightRatio.second]].bChanged = true;

    return s;
}

Solution freorder(const Problem& p, Solution s) {
    // Find cars we can operate on
    std::vector<uint16_t> applicableCars;
    applicableCars.reserve(s.size()-1);
    for (auto i{0}; i < s.size()-1; ++i)
        if (4 <= s[i].size())
            applicableCars.push_back(i);
    
    // Early exit if we can't do any operations
    if (applicableCars.empty())
        return s;

    const auto carIndex = applicableCars.at(ran() % applicableCars.size());
    auto& car = s.at(carIndex);
    // Choose two random calls from car:
    int a{car.at(ran() % car.size())},b{a};
    while (b == a)
        b = car.at(ran() % car.size());

    // Each possible configuration for sets:
    const std::array<std::array<int, 4>, 6> configurations{
        std::to_array({a, b, a, b}),
        std::to_array({a, a, b, b}),
        std::to_array({a, b, b, a}),
        std::to_array({b, a, b, a}),
        std::to_array({b, b, a, a}),
        std::to_array({b, a, a, b}),
    };

    // Check for each possible configuration if we find any that fits the time schedule:
    const auto conf = [&]() -> std::optional<std::array<int, 4>> {
        for (const auto& conf : configurations) {
            bool bPossible{true};
            std::set<int> calls;
            auto time{p.vehicles.at(carIndex).startingTime};

            for (const auto& callId : conf) {
                const auto& call = p.calls.at(callId);
                const bool bInserted = calls.insert(callId).second;
                if ((bInserted ? call.upperTimewindowPickup : call.upperTimewindowDelivery) < time) {
                    bPossible = false;
                    break;
                }
                time = std::max(bInserted ? call.lowerTimewindowPickup : call.lowerTimewindowDelivery, time);
            }

            if (bPossible)
                return conf;
        }

        return std::nullopt;
    }();

    // Exit if no configurations worked
    if (!conf)
        return s;
    
    // If we found a working configuration, swap according to that one:
    std::array poss{car.end(), car.end(), car.end(), car.end()};
    unsigned int i{0};
    for (auto it{car.begin()}; it != car.end(); ++it) {
        if (*it == a || *it == b) {
            poss[i] = it;
            ++i;
        }
    }

    // Asign the new configuration to the solution
    for (auto j{0}; j < 4; ++j)
        *poss.at(j) = conf->at(j);

    return s;
}

SolutionCached freorder(const Problem& p, SolutionCached s) {
    // Find cars we can operate on
    std::vector<uint16_t> applicableCars;
    applicableCars.reserve(s.size()-1);
    for (auto i{0}; i < s.size()-1; ++i)
        if (4 <= s[i].calls.size())
            applicableCars.push_back(i);
    
    // Early exit if we can't do any operations
    if (applicableCars.empty())
        return s;

    const auto carIndex = applicableCars.at(ran() % applicableCars.size());
    auto& car = s.at(carIndex).calls;
    // Choose two random calls from car:
    int a{car.at(ran() % car.size())},b{a};
    while (b == a)
        b = car.at(ran() % car.size());

    // Each possible configuration for sets:
    const std::array<std::array<int, 4>, 6> configurations{
        std::to_array({a, b, a, b}),
        std::to_array({a, a, b, b}),
        std::to_array({a, b, b, a}),
        std::to_array({b, a, b, a}),
        std::to_array({b, b, a, a}),
        std::to_array({b, a, a, b}),
    };

    // Check for each possible configuration if we find any that fits the time schedule:
    const auto conf = [&]() -> std::optional<std::array<int, 4>> {
        for (const auto& conf : configurations) {
            bool bPossible{true};
            std::set<int> calls;
            auto time{p.vehicles.at(carIndex).startingTime};

            for (const auto& callId : conf) {
                const auto& call = p.calls.at(callId);
                const bool bInserted = calls.insert(callId).second;
                if ((bInserted ? call.upperTimewindowPickup : call.upperTimewindowDelivery) < time) {
                    bPossible = false;
                    break;
                }
                time = std::max(bInserted ? call.lowerTimewindowPickup : call.lowerTimewindowDelivery, time);
            }

            if (bPossible)
                return conf;
        }

        return std::nullopt;
    }();

    // Exit if no configurations worked
    if (!conf)
        return s;
    
    // If we found a working configuration, swap according to that one:
    std::array poss{car.end(), car.end(), car.end(), car.end()};
    unsigned int i{0};
    for (auto it{car.begin()}; it != car.end() && i < poss.size(); ++it) {
        if (*it == a || *it == b) {
            poss[i] = it;
            ++i;
        }
    }

    // Asign the new configuration to the solution
    for (auto j{0}; j < 4; ++j)
        *poss.at(j) = conf->at(j);

    s.at(carIndex).bChanged = true;

    return s;
}

Solution multishuffle(const Problem& p, Solution s) {
    // Set a static seed to use for random number generation
    static auto randomSeed{static_cast<unsigned int>(std::time(nullptr))};
    using RetType = std::optional<std::pair<int, Solution>>;
    
    const auto threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads{};
    threads.reserve(threadCount);
    std::vector<std::future<RetType>> futures;
    futures.reserve(threadCount);

    const auto operation = [&p, &s](
        std::promise<RetType>&& ret,
        std::default_random_engine&& ran
    ) -> void {
        const auto newSolution = ins1(s, ran);
        const auto feasability = checkfeasibility(p, newSolution);
        if (!feasability) {
            const auto cost = getCost(p, newSolution);
            if (cost) {
                ret.set_value(std::make_pair(cost.val_or_max(), newSolution));
                return;
            }
        }

        ret.set_value(std::nullopt);
    };

    // Initiate threads
    for (unsigned int i{0}; i < threadCount; ++i) {
        std::promise<RetType> p{};
        futures.push_back(p.get_future());
        threads.emplace_back(operation, std::move(p), std::default_random_engine{randomSeed + i});
    }

    // Assemble and compare threads
    auto best = std::make_pair(std::numeric_limits<int>::max(), s);
    for (auto i{0}; i < threads.size(); ++i) {
        auto [t, f] = std::tie(threads.at(i), futures.at(i));
        t.join();
        if (!f.valid())
            throw std::runtime_error{"Future is somehow not set..."};
        const auto& opt = f.get();
        if (opt) {
            const auto& [cost, sol] = *opt;
            if (cost < best.first)
                best = *opt;
        }
    }

    // Increment seed so we make sure we get different pseudo-random numbers next iteration.
    randomSeed += threadCount;

    return best.second;
}

Solution backinsert(const Problem& p, Solution s) {
    constexpr float EXCHANCE_CHANCE = 0.4f;

    auto& dummy = s.back();
    auto& car = s.at(ran() % (s.size() - 1));

    // 40% chance to do 2-exchance instead of reinsert
    if (ran() % 100 * 0.01 < EXCHANCE_CHANCE) {
        // If we coudln't exchange, we can continue with the reinsert option instead.
        if (exchance(car, dummy))
            return s;
    }

    if (car.empty())
        return s;

    const auto v = car.at(ran() % car.size());
    
    // Remove from car
    car.erase(std::remove(car.begin(), car.end(), v), car.end());

    // Reinsert into dummy
    dummy.reserve(dummy.size() + 2);
    if (dummy.empty())
        dummy.push_back(v);
    else
        dummy.insert(dummy.begin() + ran() % dummy.size(), v);
    dummy.insert(dummy.begin() + ran() % dummy.size(), v);

#ifdef _DEBUG
    for (const auto& c : s)
        if (c.size() % 2)
            throw std::runtime_error{"Invalid solution!"};
#endif

    return s;
}

Solution scramble(const Problem& p, Solution s) {
    return s;
}

}