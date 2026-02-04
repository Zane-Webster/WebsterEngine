#version 460 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

void main()
{
    FragColor = texture(skybox, TexCoords);
}
