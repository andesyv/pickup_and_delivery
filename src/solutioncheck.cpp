

#include <iostream>
#include "feasibility.h"
#include "problem.h"
#include "cost.h"
#include <sstream>

int main(int argc, char *argv[]) {
    const auto file = "./data/Call_7_Vehicle_3.txt";
    std::cout << "Problem is: " << file << std::endl;
    const auto pResult = load(file);
    if (!pResult)
    {
        std::cout << "Err: " << pResult.err().what() << std::endl;
        return 1;
    }
    const auto problem = pResult.val();

    const auto solution = [&](){
        if (1 < argc) {
            std::vector<int> input;
            input.reserve(argc - 1);
            int v;
            for (int i{1}; i < argc; ++i) {
                std::stringstream ss{argv[i]};
                ss >> v;
                input.push_back(v);
            }

            return toNestedList(input);
        } else
            return toNestedList({3, 3, 0, 7, 1, 7, 1, 0, 5, 5, 6, 6, 0, 2, 4, 4, 2});
    }();
    
    std::cout << "Solution is: ";
    for (const auto& n : fromNestedList(solution))
        std::cout << n << " ";
    std::cout << std::endl;

    const auto result = checkfeasibility(problem, solution);
    if (!result) {
        std::cout << "Solution is feasible." << std::endl;

        const auto cost = getCost(problem, solution);
        if (cost)
            std::cout << "Solution cost: " << cost.val_or_max() << std::endl;
        else {
            std::cout << "Err: " << cost.err().what() << std::endl;
            return 1;
        }
    } else
        std::cout << "Solution is not feasible. :(" << std::endl;

    return 0;
}