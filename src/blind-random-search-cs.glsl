#version 430

layout(local_size_x = 1, local_size_y = 1) in; // local_size = optional

#define MAX_CALLS 10
#define MAX_VEHICLES 4
#define SOLUTION_COUNT 4 // Going to be the count of work groups

struct Vehicle {
    uint calls[MAX_CALLS];
    uint call_count;
};

struct Solution {
    Vehicle vehicles[MAX_VEHICLES];
};

layout(std430, binding = 0) buffer solutionBuffer
{
	Solution solution[SOLUTION_COUNT];
};

float ran(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void swap(inout uint a, inout uint b) {
    uint c = a;
    a = b;
    b = c;
}

// 2-exchange
Solution ex2(Solution s) {
    // Preprocessing:
    // filter out empty cars
    uint vehicle_indices[MAX_VEHICLES];
    uint vehicle_count = 0;
    for (uint i = 0; i < MAX_VEHICLES; i++) {
        if (0 < s.vehicles[i].call_count) {
            vehicle_indices[vehicle_count] = i;
            vehicle_count++;
        }
    }
    
    // Find random car:
    float r = ran(gl_LocalInvocationID.xy);
    uint car_a = vehicle_indices[uint(r * vehicle_count)];
    r = ran(gl_LocalInvocationID.xy + vec2(2.2 * r, 1.7 * r));
    uint car_b = vehicle_indices[uint(r * vehicle_count)];

    // Find random call:
    r = ran(gl_LocalInvocationID.xy + vec2(0.35 * r, 0.871 * r));
    uint call_a_index = uint(r * s.vehicles[car_a].call_count);
    uint call_a = s.vehicles[car_a].calls[call_a_index];
    r = ran(gl_LocalInvocationID.xy + vec2(3.82 * r, 1.2 * r));
    uint call_b_index = uint(r * s.vehicles[car_b].call_count);
    uint call_b = s.vehicles[car_b].calls[call_b_index];
    if (2 < s.vehicles[car_b].call_count) {
        while (call_a == call_b) {
            r = ran(gl_LocalInvocationID.xy + vec2(3.82 * r, 1.2 * r));
            call_b_index = uint(r * s.vehicles[car_b].call_count);
            call_b = s.vehicles[car_b].calls[call_b_index];
        }
    // If vehicle b is too small to change call, and the calls are the same,
    // swapping won't do anything. So just return early:
    } else if (call_a == call_b) {
        return s;
    }

    // Find last indices to calls:
    uint call_a_index_2;
    for (uint i = 0; i < s.vehicles[car_a].call_count; i++) {
        if (i != call_a_index && call_a == s.vehicles[car_a].calls[i]) {
            call_a_index_2 = i;
            break;
        }
    }
    
    uint call_b_index_2;
    for (uint i = 0; i < s.vehicles[car_b].call_count; i++) {
        if (i != call_b_index && call_b == s.vehicles[car_b].calls[i]) {
            call_b_index_2 = i;
            break;
        }
    }

    // Swap calls:
    swap(s.vehicles[car_a].calls[call_a_index], s.vehicles[car_b].calls[call_b_index]);
    swap(s.vehicles[car_a].calls[call_a_index_2], s.vehicles[car_b].calls[call_b_index_2]);

    return s;
}

int costFeasibility() {
    
}

void main() {
    gl_LocalInvocationID
}