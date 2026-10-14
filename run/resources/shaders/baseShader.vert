#version 450
layout(set = 2, binding = 0) uniform GlobalData {
    mat4 view;
    mat4 proj;
} globalData;

layout(set = 3, binding = 0) uniform UniformBufferObject {
    mat4 model;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = globalData.proj * globalData.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = vec3(0,1,0);
}