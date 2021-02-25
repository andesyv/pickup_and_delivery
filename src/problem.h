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
// Index type:
typedef uint8_t index_t;

/// Vehicle data
struct Vehicle {
    index_t homeNodeIndex;
    int startingTime;
    int capacity;
    std::vector<index_t> availableCalls;
};

/// Requested trips
struct Call {
    index_t origin;
    index_t destination;
    int size;
    int costOfNotTransporting;
    int lowerTimewindowPickup;
    int upperTimewindowPickup;
    int lowerTimewindowDelivery;
    int upperTimewindowDelivery;
};

/// Cost of trips
struct Trip {
    index_t vehicleIndex;
    index_t origin;
    index_t destination;
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
    std::map<std::pair<index_t, index_t>, VehicleCall> vehicleCalls; // vehicle index, call index
};

typedef std::vector<std::vector<int>> Solution;

std::vector<std::string_view> split(const std::string_view& str, char c);

template <typename T>
inline T stoi(const std::string_view& sv) {
    return std::stoi(std::string{sv});
}

template<>
inline index_t stoi(const std::string_view& sv) {
    uint16_t temp = std::stoi(std::string{sv});
    return static_cast<index_t>(temp);
}

Result<Problem, std::runtime_error> load(const std::string& path);


template <std::size_t I>
Solution toNestedList(const int (&a)[I]) {
    Solution out;
    std::size_t beg{0};
    for (index_t end{0}; end <= I; ++end) { // Allow for one past end as this copies the range before the end
        if (end == I || a[end] == 0) {
            out.push_back(std::vector<int>{a + beg, a + end});
            for (auto& v : out.back())
                v -= 1;

            beg = end + 1;
        }
    }
    return out;
}

std::vector<int> fromNestedList(const Solution& list);