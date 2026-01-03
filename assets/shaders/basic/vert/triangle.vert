#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view_projection;

out vec3 vColor;
out vec3 vNormal;

void main()
{
    vColor = aColor;
    vNormal = transpose(inverse(mat3(model))) * aNormal;

    gl_Position = view_projection * model * vec4(aPos, 1.0);
}
