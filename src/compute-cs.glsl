#version 430

layout(local_size_x = 1, local_size_y = 1) in; // local_size = optional
layout(rgba32f, binding = 0) uniform image2D img_output;

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    imageStore(img_output, pixel_coords, vec4(vec2(pixel_coords) / 100.0, 0., 1.0));
}

