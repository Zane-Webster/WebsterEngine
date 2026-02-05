#version 460 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

uniform sampler2D shadow_map;
uniform mat4 light_space_matrix;

void main()
{
    FragColor = texture(skybox, TexCoords);
}
