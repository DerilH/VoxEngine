#version 450
#pragma shader_stage(vertex)
layout(set = 2, binding = 0) uniform GlobalData {
    mat4 proj;
    mat4 view;
} globalData;

layout(set = 3, binding = 0) uniform UniformBufferObject {
    mat4 model;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main() {
    gl_Position = globalData.proj * globalData.view * ubo.model * vec4(inPosition, 1.0);
    outColor = inColor;
}