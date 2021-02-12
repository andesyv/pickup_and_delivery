
#include <vector>
#include <tuple>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

// Note: Nodes are (currently) not zero-indexed
typedef uint8_t Node;

/// Vehicle data
struct Vehicle {
    uint8_t index;
    uint8_t homeNodeIndex;
    int startingTime;
    int capacity;
    std::vector<uint8_t> availableCalls;
};

/// Requested trips
struct Call {
    uint8_t index;
    Node origin;
    Node destination;
    int size;
    int costOfNotTransporting;
    int lowerTimewindowPickup;
    int upperTimewindowPickup;
    int lowerTimewindowDelivery;
    int upperTimewindowDelivery;
};

/// Cost of trips
struct Trip {
    uint8_t vehicleIndex;
    Node origin;
    Node destination;
    int time;
    int cost;
};

/// Cost and times of vehicle + call combinations
struct VehicleCall {
    uint8_t vehicleIndex;
    uint8_t callIndex;
    int originNodeTime;
    int originNodeCosts;
    int destNodeTime;
    int destNodeCosts;
};
typedef std::tuple<
    std::vector<Node>,
    std::vector<Vehicle>,
    std::vector<Call>,
    std::vector<Trip>,
    std::vector<VehicleCall>
    > Problem;

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

template <typename T>
T stot(const std::string& s){
    std::stringstream ss{s};
    T result{0};
    ss >> result;
    return result;
};

template <typename T> T stot(const std::string_view& sv) {
    return stot<T>(std::string{sv});
}

void load(const std::string& path) {
    std::ifstream ifs{path};
    if (!ifs) {
        std::cout << "Failed to open file!" << std::endl;
        return;
    }
    std::string str{};

    auto next = [&](){
        ifs.ignore(300, '\n'); // Ignore line
        if (!std::getline(ifs, str, '%')) return true; // Read block of info.
        return false;
    };
    
    // Node count:
    if (next()) return;
    std::size_t nodeCount{0};
    // std::string_view sv{std::find(str.begin(), str.end(), '\n'), str.end()};
    std::stringstream ss{str};
    ss >> nodeCount;

    // Vehicle count:
    if (next()) return;
    std::vector <Vehicle> vehicles;
    int vehicleCount{0};
    ss.str(str);
    ss >> vehicleCount;
    vehicles.reserve(vehicleCount);

    // Vehicles:
    if (next()) return;
    for (auto s : split(str, '\n')) {
        auto data = split(s, ',');

        if (data.size() != 4) return;
        vehicles.emplace_back(stot<uint8_t>(data[0]), stot<uint8_t>(data[1]), stot<int>(data[2]), stot<int>(data[3]));
    }

    // Call count:
    if (next()) return;
    std::size_t callCount{0};
    ss = std::stringstream{str};
    ss >> callCount;

    // Vehicle calls
    if (next()) return;
    auto lines = split(str, '\n');
    for (uint8_t i{0}; i < vehicles.size(); ++i) {
        auto calls = split(lines.at(i), ',');
        std::transform(calls.begin()+1, calls.end(), std::back_inserter(vehicles.at(i).availableCalls),
        [](const auto& s){ return stot<uint8_t>(s)-1; }); // -1 because Zero-indexed calls
    }

    // Calls
    if (next()) return;
    std::vector<Call> calls{callCount};
    lines = split(str, '\n');
    for (uint8_t i{0}; i < callCount; ++i) {
        auto call = split(lines.at(i), ',');
        calls.emplace_back(
            i + 1,
            stot<uint8_t>(call[1]),
            stot<uint8_t>(call[2]), 
            stot<int>(call[3]),
            stot<int>(call[4]),
            stot<int>(call[5]),
            stot<int>(call[6]),
            stot<int>(call[7]),
            stot<int>(call[8])
        );
    }

    // Travel times and costs:
    if (next()) return;
    std::vector<Trip> trips;
    trips.reserve(100);
    lines = split(str, '\n');
    for (const auto& line : lines) {
        auto data = split(line, ',');
        trips.emplace_back(
            stot<uint8_t>(data[0])-1, // zero-indexed
            stot<uint8_t>(data[1]),
            stot<uint8_t>(data[2]),
            stot<int>(data[3]),
            stot<int>(data[4])
        );
    }

    // Cost vehicle + call combinations
    if (next()) return;
    std::vector<VehicleCall> vehicleCalls;
    vehicleCalls.reserve(10);
    lines = split(str, '\n');
    for (const auto& line : lines) {
        auto data = split(line, ',');
        vehicleCalls.emplace_back(
            stot<uint8_t>(data[0])-1, // zero-indexed
            stot<uint8_t>(data[1])-1, // zero-indexed
            stot<int>(data[2]),
            stot<int>(data[3]),
            stot<int>(data[4]),
            stot<int>(data[5])
        );
    }

    
}