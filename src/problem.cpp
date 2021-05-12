#include "problem.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <utility>
#include <numeric>

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
        p.vehicles.emplace_back(stoi<index_t>(data[1])-1, stoi<int>(data[2]), stoi<int>(data[3]));
    }

    // Call count:
    if (next()) return std::runtime_error{"End of file"};
    std::size_t callCount{0};
    ss = std::stringstream{str};
    ss >> callCount;

    // Vehicle calls
    if (next()) return std::runtime_error{"End of file"};
    auto lines = split(str, '\n');
    for (index_t i{0}; i < p.vehicles.size(); ++i) {
        auto calls = split(lines.at(i), ',');
        std::transform(calls.begin()+1, calls.end(), std::back_inserter(p.vehicles.at(i).availableCalls),
        [](const auto& s){ return stoi<index_t>(s)-1; }); // -1 because Zero-indexed calls
    }

    // Calls
    if (next()) return std::runtime_error{"End of file"};
    p.calls.reserve(callCount);
    lines = split(str, '\n');
    for (index_t i{0}; i < callCount; ++i) {
        auto call = split(lines.at(i), ',');
        p.calls.emplace_back(
            stoi<index_t>(call[1])-1,
            stoi<index_t>(call[2])-1, 
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
            stoi<index_t>(data[0])-1,
            stoi<index_t>(data[1])-1,
            stoi<index_t>(data[2])-1,
            stoi<int>(data[3]),
            stoi<int>(data[4])
        );
    }

    // Cost vehicle + call combinations
    if (next()) return std::runtime_error{"End of file"};
    // p.vehicleCalls.reserve(10);
    lines = split(str, '\n');
    for (const auto& line : lines) {
        auto data = split(line, ',');
        p.vehicleCalls[{
            stoi<index_t>(data[0])-1, // zero-indexed
            stoi<index_t>(data[1])-1 // zero-indexed
        }] = VehicleCall{
            stoi<int>(data[2]),
            stoi<int>(data[3]),
            stoi<int>(data[4]),
            stoi<int>(data[5])
        };
    }

    return p;
}

Solution toNestedList(const SolutionComp& s) {
    Solution out;
    std::size_t beg{0};
    for (index_t end{0}; end <= s.size(); ++end) { // Allow for one past end as this copies the range before the end
        if (end == s.size() || s[end] == -1) {
            out.push_back(std::vector<int>{s.begin() + beg, s.begin() + end});

            beg = end + 1;
        }
    }
    return out;
}

SolutionCached toCachedSolution(const Solution& s) {
    SolutionCached out;
    out.reserve(s.size());
    std::transform(s.begin(), s.end(), std::back_inserter(out), [](const auto& l){
        return VehicleSolution{ .calls = l };
    });
    return out;
}

std::vector<int> fromNestedList(const Solution& list) {
    std::vector<int> out;
    out.reserve(
        std::accumulate(list.begin(), list.end(), std::size_t{0}, [](const auto& a, const auto& b){
            return a + b.size() + 1;
        })
    );
    for (std::size_t i{0}; i < list.size(); ++i) {
        auto inner = list.at(i);
        for (const auto& v : inner)
            out.push_back(v+1);
        if (i < list.size() - 1)
            out.push_back(0);
    }
    return out;
}

std::vector<int> fromNestedList(const SolutionCached& list) {
    std::vector<int> out;
    out.reserve(
        std::accumulate(list.begin(), list.end(), std::size_t{0}, [](const auto& a, const auto& b){
            return a + b.calls.size() + 1;
        })
    );
    for (std::size_t i{0}; i < list.size(); ++i) {
        auto inner = list.at(i).calls;
        for (const auto& v : inner)
            out.push_back(v+1);
        if (i < list.size() - 1)
            out.push_back(0);
    }
    return out;
}

SolutionComp fromNestedListZeroIndexed(const Solution& list) {
    std::vector<int> out;
    out.reserve(
        std::accumulate(list.begin(), list.end(), std::size_t{0}, [](const auto& a, const auto& b){
            return a + b.size() + 1;
        })
    );
    for (std::size_t i{0}; i < list.size(); ++i) {
        auto inner = list.at(i);
        for (const auto& v : inner)
            out.push_back(v);
        if (i < list.size() - 1)
            out.push_back(-1);
    }
    return out;
}