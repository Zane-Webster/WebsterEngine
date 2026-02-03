#version 460 core

in vec3 vColor;
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoord;

out vec4 FragColor;

// ===== Camera =====
uniform vec3 camera_pos;

// ===== Directional Light =====
uniform vec3 light_dir;
uniform vec3 light_color;

// ===== Material =====
uniform float ambient_strength;
uniform float specular_strength;
uniform float shininess;

// ===== Diffuse Texture =====
uniform sampler2D u_Diffuse;

// ===== Shadows =====
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

// ===== Shadow Calculation =====
float ShadowFactor(vec3 worldPos, vec3 normal, vec3 lightDir)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;

    float bias = max(0.002 * (1.0 - dot(normal, -lightDir)), 0.0005);

    float shadow = 0.0;

    float softness = mix(1.5, 2.0, projCoords.z);
    vec2 texelSize = softness / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float depth = texture(
                shadowMap,
                projCoords.xy + vec2(x, y) * texelSize
            ).r;

            shadow += (projCoords.z - bias > depth) ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
    return shadow;
}

void main()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(camera_pos - vWorldPos);

    // ambient
    vec3 ambient = ambient_strength * light_color;

    // diffuse
    float diff = max(dot(N, -light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // blinn-phong specular
    vec3 H = normalize(V - light_dir);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specular = specular_strength * spec * light_color;

    float shadow = ShadowFactor(vWorldPos, N, light_dir);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    // ===== texture sampling =====
    vec3 texColor = texture(u_Diffuse, vTexCoord).rgb;

    // combine everything
    vec3 finalColor = lighting * vColor * texColor;

    FragColor = vec4(finalColor, 1.0);
}
