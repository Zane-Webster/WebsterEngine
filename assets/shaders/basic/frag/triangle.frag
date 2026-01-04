#version 460 core

in vec3 vColor;
in vec3 vNormal;
in vec3 vWorldPos;

out vec4 FragColor;

// ===== Camera =====
uniform vec3 camera_pos;

// ===== Directional Light =====
uniform vec3 light_dir;   // normalized, pointing *from* light
uniform vec3 light_color;

// ===== Material =====
uniform float ambient_strength;
uniform float specular_strength;
uniform float shininess;

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

    vec3 lighting = ambient + diffuse + specular;
    vec3 finalColor = lighting * vColor;

    FragColor = vec4(finalColor, 1.0);
}
