#include <iostream>
#include <vector>
#include "problem.h"
#include "feasability.h"
#include <algorithm>
#include "cost.h"

template <std::size_t I>
std::vector<std::vector<int>> toNestedList(const int (&a)[I]) {
    std::vector<std::vector<int>> out;
    std::size_t beg{0};
    for (uint8_t end{0}; end <= I; ++end) { // Allow for one past end as this copies the range before the end
        if (end == I || a[end] == 0) {
            out.push_back(std::vector<int>{a + beg, a + end});
            std::transform(out.back().begin(), out.back().end(), out.back().begin(),
            [](const auto& i){ return i-1; });

            beg = end + 1;
        }
    }
    return out;
}

// std::vector<int> fromNestedList(const std::vector<std::vector<int>>& list) {

// }

int main() {
    std::cout << "Hello world!" << std::endl;
    
    auto pResult = load("./src/data/Call_7_Vehicle_3.txt");
    if (!pResult) {
        std::cout << pResult.err().what() << std::endl;
        return 1;
    }
    auto problem = pResult.val();


    // std::vector<std::vector<int>> solution{
    //     {6, 6, 4, 4}, // First vehicle
    //     {1, 1}, // Second vehicle
    //     {2, 3, 3, 2, 0, 0}, // Third vehicle
    //     {5, 5} // Dummy vehicle (outsourcing)
    // };
    auto solution = toNestedList({1, 1, 0, 0, 0, 7, 7, 3, 4, 4, 3, 2, 2, 6, 6});

    if (auto err = checkFeasability(problem, solution)) {
        std::cout << "Not feasable: " << err.value().what() << std::endl;
        return 1;
    }
    std::cout << "Feasable!" << std::endl;

    auto cResult = getCost(problem, solution);
    if (!cResult) {
        std::cout << cResult.err().what() << std::endl;
        return 1;
    }
    auto cost = cResult.val();
    std::cout << "Cost was " << cost << std::endl;

    return 0;
}