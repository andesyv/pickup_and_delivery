#pragma once
#include <vector>
#include <string>
#include <variant>
#include <stdexcept>
#include <limits>
#include <map>
#include <optional>

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

    // Prevent all implicit conversions from Result
    template <typename K>
    operator K() const = delete;
};

// Note: Nodes are (currently) not zero-indexed
// Index type:
typedef uint16_t index_t;

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

using Solution = std::vector<std::vector<int>>;
struct VehicleSolution {
    std::vector<int> calls;
    std::optional<int> cost{std::nullopt};
    bool bChanged{true};

    /**
     * @brief Whether the solution is declared infeasible.
     * If the solution was not changed, but it still doesn't
     * have a cost, it's infeasible.
     * 
     * @return true == infeasible, false == uncertain
     */
    bool infeasible() const { return !bChanged && !cost; }
    bool feasible() const { return !bChanged && cost; }
};
using SolutionCached = std::vector<VehicleSolution>;

/** Compact memory layout solution representation
 * Since it's a one dimensional vector the memory representation
 * is more or less guaranteed to be sequential, meaning that
 * it better preserves cache locality.
 */
using SolutionComp = std::vector<int>;

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

Solution toNestedList(const SolutionComp& s);
SolutionCached toCachedSolution(const Solution& s);

std::vector<int> fromNestedList(const Solution& list);
std::vector<int> fromNestedList(const SolutionCached& list);
SolutionComp fromNestedListZeroIndexed(const Solution& list);

// https://stackoverflow.com/questions/60151514/using-stdvector-as-view-on-to-raw-memory
template<typename T>
class array_view {
   T* ptr_;
   std::size_t len_;
public:
   array_view(T* ptr, std::size_t len) noexcept: ptr_{ptr}, len_{len} {}

   T& operator[](int i) noexcept { return ptr_[i]; }
   T const& operator[](int i) const noexcept { return ptr_[i]; }
   auto size() const noexcept { return len_; }

   auto begin() noexcept { return ptr_; }
   auto begin() const noexcept { return ptr_; }
   auto end() noexcept { return ptr_ + len_; }
   auto end() const noexcept { return ptr_ + len_; }
};