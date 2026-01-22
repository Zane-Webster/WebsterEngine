#ifndef WE_CORE_WEBSTERENGINE_H_
#define WE_CORE_WEBSTERENGINE_H_

#include <iostream>
#include <array>
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <SDL3/SDL.h>

#include "cfg/cfg.h"

#define WE_EMPTY_STRING "WE_EMPTY_STRING"
#define WE_EMPTY_VECTOR {}
#define WE_EMPTY_MAP {}

#define WE_GRAVITY -4.0f
#define WE_PENETRATION_SLOP 0.01f
#define WE_CORRECTION_PERCENT 0.8f

namespace WE {
    inline const std::string WINDOW_TITLE = "WebsterEngine | " + std::to_string(WE_VERSION_MAJOR) + "." + std::to_string(WE_VERSION_MINOR) + "." + std::to_string(WE_VERSION_PATCH);

    using UINT8 = std::uint8_t;
    using UINT16 = std::uint16_t;

    using TRIANGLE_VERTICES = std::array<GLfloat, 18>;
    using TRIANGLE_VERTICES_NORMAL = std::array<GLfloat, 27>;

    enum class WINDOW_RESOLUTION {
        HD,
        FHD,
        QHD
    };

    enum class STATE {
        BOOT,
        MAIN_MENU,
        EDITOR,
        PAUSE_MENU,
        EXIT
    };

    enum class LOAD_STATE {
        SUCCESS,
        FAIL,
        PATH_NOT_FOUND,
        UNKNOWN_FILE_TYPE,
        INCORRECT_FILE_TYPE
    };

    enum class RENDERITEM_TYPE {
        OBJECT,
        STATIC_OBJECT,
        DYNAMIC_OBJECT,
        SKYBOX,
        TEXT,
        TEXTURE
    };

    enum class KEYSET {
        WASD,
        ARROWS
    };

    enum class COLLIDER_TYPE {
        UNDECLARED,
        AABB,
        SPHERE,
        CAPSULE
    };

    struct RenderItem {
        RenderItem(std::string p_name, WE::RENDERITEM_TYPE p_type, GLuint p_shader_program, std::shared_ptr<void> p_ptr = nullptr, WE::UINT8 p_layer = 0, bool p_active = true, glm::vec2 p_origin = glm::vec2(0.0f), bool p_raycastable = true) : name(p_name), type(p_type), shader_program(p_shader_program), ptr(p_ptr), layer(p_layer), active(p_active), origin(p_origin), raycastable(p_raycastable) {
            ;
        }
        std::string name = WE_EMPTY_STRING;
        WE::RENDERITEM_TYPE type = WE::RENDERITEM_TYPE::OBJECT;
        GLuint shader_program = 0;

        // 0 = LOWEST, 255 = HIGHEST
        WE::UINT8 layer = 0;
        bool active = true;
        glm::vec2 origin = glm::vec2(0.0f);

        bool raycastable = true;

        std::shared_ptr<void> ptr = nullptr;

        RenderItem() = default;
    };

    struct Light {
        Light(std::string p_name, glm::vec3 p_direction, glm::vec3 p_color = glm::vec3(1.0f)) : name(p_name), direction(p_direction), color(p_color) {};
        std::string name = WE_EMPTY_STRING;
        glm::vec3 direction = glm::vec3(0.0f);
        glm::vec3 color = glm::vec3(1.0f);

        Light() = default;
    };

    struct Material {
        Material(float p_ambient_strength, float p_specular_strength, float p_shininess) : ambient_strength(p_ambient_strength), specular_strength(p_specular_strength), shininess(p_shininess) {};
        float ambient_strength = 0.1f;
        float specular_strength = 0.5f;
        float shininess = 32.0f;
        
        Material() = default;
    };

    struct ShaderUniforms {
        GLint model = -1;
        GLint view_projection = -1;
        GLint camera_pos = -1;
        GLint light_dir = -1;
        GLint light_color = -1;
        GLint ambient_strength = -1;
        GLint specular_strength = -1;
        GLint shininess = -1;
    };

    struct RenderBatch {
        GLuint program = 0;
        std::vector<std::shared_ptr<WE::RenderItem>> items = WE_EMPTY_VECTOR;
        WE::ShaderUniforms uniforms = {};
    };

    struct Ray {
        glm::vec3 origin = glm::vec3(0.0f);
        glm::vec3 direction = glm::vec3(0.0f);
    };

    struct RayHit {
        float distance = std::numeric_limits<float>::infinity();
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        WE::RenderItem* item = nullptr;
    };

    // ======== COLLISION ========
    
    struct AABB {
        glm::vec3 min = glm::vec3(0.0f);
        glm::vec3 max = glm::vec3(0.0f);
    };

    struct ColliderShape {
        ColliderShape(WE::COLLIDER_TYPE p_type) : type(p_type) {}
        virtual ~ColliderShape() = default;

        glm::vec3 center = glm::vec3(0.0f);

        WE::COLLIDER_TYPE type;
        ColliderShape() = default;
    };

    struct AABBShape final : ColliderShape {
        AABBShape(WE::AABB p_local_box) : ColliderShape(WE::COLLIDER_TYPE::AABB), local_box(p_local_box) {}
        
        WE::AABB local_box;
        WE::AABB world_box;
    };

    struct SphereShape final : ColliderShape {
        SphereShape(float p_radius) : ColliderShape(WE::COLLIDER_TYPE::SPHERE), radius(p_radius) {}

        float radius = 0.0f;
    };

    struct CapsuleShape final : ColliderShape {
        CapsuleShape(float p_radius, float p_height) : ColliderShape(WE::COLLIDER_TYPE::CAPSULE), radius(p_radius), height(p_height) {}

        float radius = 0.0f;
        float height = 0.0f;

        glm::vec3 base = glm::vec3(0.0f);
        glm::vec3 tip = glm::vec3(0.0f);
    };

    struct CollisionManifold {
        bool hit = false;
        glm::vec3 normal = glm::vec3(0.0f);
        float penetration = 0.0f;
    };
}

#endif // WE_CORE_WEBSTERENGINE_H_