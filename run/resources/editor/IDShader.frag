#version 450
#pragma shader_stage(fragment)
layout (set = 3, binding = 1) uniform data {
    uint objectIdLo;
    uint objectIdHi;
};

layout (location = 0) out uvec2 outId;
void main() {
    //r = lo, g = hi
    outId = uvec2(objectIdLo, objectIdHi);
}