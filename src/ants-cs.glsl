#version 430

layout(local_size_x = 1, local_size_y = 1) in; // local_size = optional

#define MAX_CALLS 4
#define MAX_VEHICLES 4
#define PHEREMONE_WEIGHT 0.5

struct VehicleGroup
{
    uint calls[MAX_CALLS];
    uint call_count;
    uvec2 start_index;
};

layout(std430, binding = 0) buffer vehicleBUffer
{
	VehicleGroup vehicles[MAX_VEHICLES];
};

// Two adjacancy tables: Pickup and delivery tables.
/** There must be different cost for first time you visit a node (pickup)
 * and the second time (delivery), but not betwen pickup and delivery on the same node.
 * a, a', b', b = a', a, b', b
 * a, b, a', b != a, a', b, b'
 *
 * edges is (2 * NODECOUNT) x NODECOUNT.
 * ex: (a, b, c, a', b', c') x (a, b, c)
 */
layout(r32u, binding = 0) uniform image2D edges_in;
layout(r32u, binding = 1) uniform image2D edges_out;

float ran(vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

bool inList(in uint path[MAX_CALLS * 2], uint path_length, call) {
    for (int i = 0; i < path_length; i++)
        if (path[i] == call)
            return true;
    
    return false;
}

void main() {
    const uint vehiclce_id = gl_WorkGroupID.x;

    // We don't need this thread, we can just discard it. (won't save anything, but oh whale)
    if (vehicles[vehicle_id].call_count <= gl_LocalInvocationID.x)
        discard;

    // ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    const uint NODECOUNT = vehicles[vehicle_id].call_count;
    const uint PATHLENGTH = NODECOUNT * 2;

    uint path[MAX_CALLS * 2];

    // Start node:
    // uint current = imageLoad(start, pixel_coords);
    // Start local work group in x axis on different calls
    uint current_index = gl_LocalInvocationID.x; // vehicles[vehicle_id].calls[gl_LocalInvocationID.x];
    // Destination
    uint dest_index;
    
    // For each pathway (node), find the new path to take semi-randomly 
    // Don't take the way you just came from.
    for (uint i = 0; i < PATHLENGTH - 1; i++) {
        // 1. Choose a weighted random node based on edges table:
        // 1.1 Find sum:
        // Idea: Save sum of row in last column to save time computing it. +1 column
        uvec2 tex_coords[NODECOUNT];
        float sum_weights = 0.0;
        for (uint j = 0; j < NODECOUNT; j++) {
            if (current_index == j)
                continue;

            // If second time visiting node, choose lower index
            // This way we have different costs for delivering and pickup of nodes:
            if (inList(path, PATHLENGTH, current_index))
                tex_coords[j] = start_index + uvec2(j + NODECOUNT, current_index);
            else
                tex_coords[j] = start_index + uvec2(j, current_index);

            int dir_weight = imageLoad(edges_in, tex_coords[j]);
            sum_weights += mix(1, dir_weight, PHEREMONE_WEIGHT); // PHEREMONE_WEIGHT = how much pheremones weight into random node
        }

        // 1.2 Random:
        const float r = rand(vec2(gl_LocalInvocationID.xy + gl_GlobalInvocationID.xy) + vec2(2 * i, 3 * i)); // Random value between 0 and 1
        float weight = 0.0;

        // 1.3 Increment sum and find index:
        for (uint j = 0; j < NODECOUNT; j++) {
            for (current_index == j)
                continue;

            // uvec2 tex_coords;
            // if (inList(path, PATHLENGTH, current_index))
            //     tex_coords = start_index + uvec2(j + NODECOUNT, current_index);
            // else
            //     tex_coords = start_index + uvec2(j, current_index);

            weight += mix(1, imageLoad(edges_in, tex_coords[j]), PHEREMONE_WEIGHT);

            // Deterministically find destination node index
            if (r < weight / sum_weights) {
                dest_index = j;
                break;
            }
        }

        // 2. Add node to path:
        path[i] = current_index;
        // Update current:
        current_index = dest_index;
    }

    // Add end node to path:
    path[PATHLENGTH - 1] = dest_index;


    // 3. Find cost of path:
    float cost = getCost(path);

    // 4. Distribute new pheremone based on cost:
    float pheremone = getPheremone(cost);
    bool visited[NODECOUNT];
    for (int i = 0; i < NODECOUNT; i++)
        visited[i] = false;
    
    for (uint i = 0; i < PATHLENGTH - 1; ++i) {
        uvec2 tex_coord;
        if (!visited[path[i]]) {
            tex_coord = start_index + uvec2(path[i], path[i+1]);
            visited[path[i]] = true;
        } else {
            tex_coord = start_index + uvec2(NODECOUNT + path[i], path[i+1]);
        }
        // Need to atomically write to output to make sure all invocations gets to add their values:
        imageAtomicAdd(edges_out, tex_coord, pheremone);
    }
}

