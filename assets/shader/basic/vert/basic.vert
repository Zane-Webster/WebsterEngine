#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view_projection;

out vec3 vColor;
out vec3 vNormal;
out vec3 vWorldPos;
out vec2 vTexCoord;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    vColor = aColor;
    vNormal = transpose(inverse(mat3(model))) * aNormal;
    vTexCoord = aTexCoord;

    gl_Position = view_projection * worldPos;
}