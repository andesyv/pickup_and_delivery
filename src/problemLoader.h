
#include <vector>
#include <tuple>

typedef uint8_t Node;

/// Vehicle data
struct Vehicle {
    uint8_t index;
    uint8_t homeNodeIndex;
    int startingTime;
    int capacity;
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