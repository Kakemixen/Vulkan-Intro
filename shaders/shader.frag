#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0, location = 0) uniform sampler2D texSampler[2];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler[1], fragTexCoord);
}
