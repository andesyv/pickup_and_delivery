#include "problem.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>

std::vector<std::string_view> split(const std::string_view& str, char c) {
    std::vector<std::string_view> views;
    auto next = [&](const auto& it){
        return (it == str.end()) ? it : std::find(it+1, str.end(), c);
    };
    for (auto beg{str.begin()}, end{next(beg)}; beg != str.end(); beg = end, end = next(end)) {
        if (beg != str.begin())
            beg = beg + 1;
        if (beg != end)
            views.push_back(std::string_view{beg, end});
    }
    return views;
}

Result<Problem, std::runtime_error> load(const std::string& path) {
    std::ifstream ifs{path};
    if (!ifs) {
        std::cout << "Failed to open file!" << std::endl;
        return std::runtime_error{"Failed to open file!"};
    }
    std::string str{}; // Input buffer string
    Problem p;

    auto next = [&](){
        ifs.ignore(300, '\n'); // Ignore line
        if (!std::getline(ifs, str, '%')) return true; // Read block of info into str.
        return false;
    };


    
    // Node count:
    if (next()) return std::runtime_error{"End of file"};
    // std::string_view sv{std::find(str.begin(), str.end(), '\n'), str.end()};
    std::stringstream ss{str};
    ss >> p.nodeCount;

    // Vehicle count:
    if (next()) return std::runtime_error{"End of file"};
    int vehicleCount{0};
    ss.str(str);
    ss >> vehicleCount;
    p.vehicles.reserve(vehicleCount);

    // Vehicles:
    if (next()) return std::runtime_error{"End of file"};
    for (auto s : split(str, '\n')) {
        auto data = split(s, ',');

        if (data.size() != 4) return std::runtime_error{"Input file missing data in vehicle list"};
        p.vehicles.emplace_back(stoi<uint8_t>(data[1])-1, stoi<int>(data[2]), stoi<int>(data[3]));
    }

    // Call count:
    if (next()) return std::runtime_error{"End of file"};
    std::size_t callCount{0};
    ss = std::stringstream{str};
    ss >> callCount;

    // Vehicle calls
    if (next()) return std::runtime_error{"End of file"};
    auto lines = split(str, '\n');
    for (uint8_t i{0}; i < p.vehicles.size(); ++i) {
        auto calls = split(lines.at(i), ',');
        std::transform(calls.begin()+1, calls.end(), std::back_inserter(p.vehicles.at(i).availableCalls),
        [](const auto& s){ return stoi<uint8_t>(s)-1; }); // -1 because Zero-indexed calls
    }

    // Calls
    if (next()) return std::runtime_error{"End of file"};
    p.calls.reserve(callCount);
    lines = split(str, '\n');
    for (uint8_t i{0}; i < callCount; ++i) {
        auto call = split(lines.at(i), ',');
        p.calls.emplace_back(
            stoi<uint8_t>(call[1])-1,
            stoi<uint8_t>(call[2])-1, 
            stoi<int>(call[3]),
            stoi<int>(call[4]),
            stoi<int>(call[5]),
            stoi<int>(call[6]),
            stoi<int>(call[7]),
            stoi<int>(call[8])
        );
    }

    // Travel times and costs:
    if (next()) return std::runtime_error{"End of file"};
    p.trips.reserve(100);
    lines = split(str, '\n');
    for (const auto& line : lines) {
        auto data = split(line, ',');
        p.trips.emplace_back(
            stoi<uint8_t>(data[1])-1,
            stoi<uint8_t>(data[2])-1,
            stoi<int>(data[3]),
            stoi<int>(data[4])
        );
    }

    // Cost vehicle + call combinations
    if (next()) return std::runtime_error{"End of file"};
    p.vehicleCalls.reserve(10);
    lines = split(str, '\n');
    for (const auto& line : lines) {
        auto data = split(line, ',');
        p.vehicleCalls.emplace_back(
            stoi<uint8_t>(data[0])-1, // zero-indexed
            stoi<uint8_t>(data[1])-1, // zero-indexed
            stoi<int>(data[2]),
            stoi<int>(data[3]),
            stoi<int>(data[4]),
            stoi<int>(data[5])
        );
    }

    return p;
}