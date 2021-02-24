#pragma once
#include <vector>
#include <string>
#include <variant>
#include <stdexcept>
#include <limits>
#include <map>

// Maybe monad / neither implementation based on https://github.com/LoopPerfect/neither and std::optional
template <typename T, typename E = std::exception>
struct Result {
    std::variant<T,E> value;

    Result(const T& v) : value{v} {}
    Result(const E& e) : value{e} {}

    operator bool() const { return value.index() == 0; }
    T val() const { return std::get<T>(value); }
    E err() const { return std::get<E>(value); }
    T max() const { return std::numeric_limits<T>::max(); }
    T val_or_max() const { return static_cast<bool>(*this) ? val() : max(); }
    T operator->() const { return std::get<T>(value); }
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
    std::map<std::pair<uint8_t, uint8_t>, VehicleCall> vehicleCalls; // vehicle index, call index
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