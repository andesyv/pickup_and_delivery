#pragma once
#include <vector>
#include <string>
#include <variant>
#include <stdexcept>

// Maybe monad / neither implementation based on https://github.com/LoopPerfect/neither and std::optional
template <typename T, typename E = std::exception>
struct Result {
    std::variant<T,E> value;

    Result(const T& v) : value{v} {}
    Result(const E& e) : value{e} {}

    operator bool() const { return value.index() == 0; }
    T val() { return std::get<T>(value); }
    E err() { return std::get<E>(value); }
    T operator->() { return std::get<T>(value); }
};

// Note: Nodes are (currently) not zero-indexed
typedef uint8_t Node;

/// Vehicle data
struct Vehicle {
    uint8_t homeNodeIndex;
    int startingTime;
    int capacity;
    std::vector<uint8_t> availableCalls;
};

/// Requested trips
struct Call {
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
struct Problem {
    std::size_t nodeCount;
    std::vector<Vehicle> vehicles;
    std::vector<Call> calls;
    std::vector<Trip> trips;
    std::vector<VehicleCall> vehicleCalls;
};

std::vector<std::string_view> split(const std::string_view& str, char c);

template <typename T>
inline T stoi(const std::string_view& sv) {
    return std::stoi(std::string{sv});
}

template<>
inline uint8_t stoi(const std::string_view& sv) {
    uint16_t temp = std::stoi(std::string{sv});
    return static_cast<uint8_t>(temp);
}

Result<Problem, std::runtime_error> load(const std::string& path);