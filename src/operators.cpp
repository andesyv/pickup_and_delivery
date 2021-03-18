#include "operators.h"
#include <algorithm>
#include <limits>
#include <random>
#include <ctime>
#include <numeric>
#include <set>

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

// 3-exchange operator
Solution ex3(Solution s) {
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

    // Insert two of call id into random car:
    auto& car = s[carIds[ran() % carIds.size()]];
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

Solution tex2(const Problem& p, Solution s) {
    return s;
}

}