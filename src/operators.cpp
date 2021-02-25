#include "operators.h"
#include <algorithm>
#include <limits>
#include <random>
#include <ctime>

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

    const auto& end = s.back().end();
    // It should be guaranteed that the random calls are represented in the solution (else it would be invalid)
    auto    apos{std::make_pair(end, end)},
            bpos{std::make_pair(end, end)};
    for (auto& l : s) {
        for (auto it{l.begin()}; it != l.end(); ++it) {
            const auto& v = *it;

            if (v == a)
                if (apos.first == end)
                    apos.first = it;
                else
                    apos.second = it;

            if (v == b)
                if (bpos.first == end)
                    bpos.first = it;
                else
                    bpos.second = it;
        }
    }

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

    const auto& end = s.back().end();
    // It should be guaranteed that the random calls are represented in the solution (else it would be invalid)
    auto    apos{std::make_pair(end, end)},
            bpos{std::make_pair(end, end)},
            cpos{std::make_pair(end, end)};
    for (auto& l : s) {
        for (auto it{l.begin()}; it != l.end(); ++it) {
            const auto& v = *it;

            if (v == a)
                if (apos.first == end)
                    apos.first = it;
                else
                    apos.second = it;

            if (v == b)
                if (bpos.first == end)
                    bpos.first = it;
                else
                    bpos.second = it;

            if (v == c)
                if (cpos.first == end)
                    cpos.first = it;
                else
                    cpos.second = it;
        }
    }

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
}