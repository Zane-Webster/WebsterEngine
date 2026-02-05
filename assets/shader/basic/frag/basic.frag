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
uniform mat4 light_space_matrix;

// ===== Material =====
uniform float ambient_strength;
uniform float specular_strength;
uniform float shininess;

// ===== Diffuse Texture =====
uniform sampler2D u_Diffuse;

// ===== Shadows =====
uniform sampler2D shadow_map;

// ===== Shadow Calculation =====
float ShadowFactor(vec3 world_pos, vec3 normal, vec3 light_dir)
{
    vec4 frag_pos_light_space = light_space_matrix * vec4(world_pos, 1.0);
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    if (proj_coords.z > 1.0)
        return 0.0;

    float bias = max(0.002 * (1.0 - dot(normal, -light_dir)), 0.0005);

    float shadow = 0.0;

    float softness = mix(1.5, 2.0, proj_coords.z);
    vec2 texel_size = softness / textureSize(shadow_map, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float depth = texture(
                shadow_map,
                proj_coords.xy + vec2(x, y) * texel_size
            ).r;

            shadow += (proj_coords.z - bias > depth) ? 1.0 : 0.0;
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
    vec3 tex_color = texture(u_Diffuse, vTexCoord).rgb;

    // combine everything
    vec3 final_color = lighting * vColor * tex_color;

    FragColor = vec4(final_color, 1.0);
}
