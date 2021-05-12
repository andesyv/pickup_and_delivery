#version 430

#define PATHLENGTH 4
#define NODECOUNT 3

layout(local_size_x = 1, local_size_y = 1) in; // local_size = optional

// Two adjacancy tables: Pickup and delivery tables.
/** There must be different cost for first time you visit a node (pickup)
 * and the second time (delivery), but not betwen pickup and delivery on the same node.
 * a, a', b', b = a', a, b', b
 * a, b, a', b != a, a', b, b'
 *
 * edges is (2 * NODECOUNT) x NODECOUNT.
 * ex: (a, b, c, a', b', c') x (a, b, c)
 */
layout(rgba32f, binding = 0) uniform image2D edges;

// 2d/3d texture of integer pixel positions equaling ant start pixel-index.
layout(rg16i, binding = 2) uniform image2D start_node;

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    uint path[PATHLENGTH];

    // Start node:
    uint current = imageLoad(start, pixel_coords);
    // Destination
    uint dest;
    
    // For each pathway (node), find the new path to take semi-randomly 
    // Don't take the way you just came from.
    for (int i = 0; i < PATHLENGTH - 1; i++) {
        // 1. Choose a weighted random node based on edges table:
        // 1.1 Find sum:
        // Idea: Save sum of row in last column to save time computing it. +1 column
        float sum_weights = 0.0;
        for (int j = 0; j < NODECOUNT; j++) {
            if (current == j)
                continue;
            
            float dir_weight = 0.0;

            // If second time visiting node, choose lower index
            // This way we have different costs for delivering and pickup of nodes:
            if (inList(path, current))
                dir_weight = edges[NODECOUNT + current, j];
            else
                dir_weight = edges[current, j];
            sum_weights += mix(1.0, dir_weight, PHEREMONE_WEIGHT); // PHEREMONE_WEIGHT = how much pheremones weight into random node
        }

        // 1.2 Random:
        float r = rand(); // Random value between 0 and 1
        float weight = 0.0;

        // 1.3 Increment sum and find index:
        for (int j = 0; j < NODECOUNT; j++) {
            for (current == j)
                continue;

            weight += mix(1.0, edges[current, j], PHEREMONE_WEIGHT);

            // Deterministically find destination node index
            if (r < weight / sum_weights) {
                dest = j;
                break;
            }
        }

        // 2. Add node to path:
        path[i] = current;
        // Update current:
        current = dest;
    }

    // Add end node to path:
    path[PATHLENGTH - 1] = dest;


    // 3. Find cost of path:
    float cost = getCost(path);

    // 4. Distribute new pheremone based on cost:
    float pheremone = getPheremone(cost);
    bool visited[NODECOUNT] = 0;
    for (int i = 0; i < PATHLENGTH - 1; ++i) {
        if (!visited[path[i]])
            edges[path[i], path[i+1]] += pheremone;
        else
            edges[NODECOUNT + path[i], path[i+1]] += pheremone;
    }



    // imageStore(img_output, pixel_coords, vec4(vec2(pixel_coords) / 100.0, 0., 1.0));
}

