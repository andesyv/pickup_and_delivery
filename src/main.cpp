#include <iostream>
#include <vector>
#include "problemLoader.h"

int main() {
    std::cout << "Hello world!" << std::endl;
    auto problem = load("./src/data/Call_7_Vehicle_3.txt");

    std::vector<std::vector<int>> solution{
        {6, 6, 4, 4}, // First vehicle
        {1, 1}, // Second vehicle
        {2, 3, 3, 2, 0, 0}, // Third vehicle
        {5, 5} // Dummy vehicle (outsourcing)
    };

    return 0;
}