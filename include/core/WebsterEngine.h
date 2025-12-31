#ifndef WE_CORE_WEBSTERENGINE_H_
#define WE_CORE_WEBSTERENGINE_H_

#include <iostream>
#include <array>
#include <cstdint>
#include <vector>
#include <memory>

#include <glad/gl.h>
#include <glm/glm.hpp>

#define WE_EMPTY_STRING "WE_EMPTY_STRING"
#define WE_EMPTY_VECTOR {}
#define WE_EMPTY_MAP {}

namespace WE {
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
        SKYBOX,
        TEXT,
        TEXTURE
    };

    struct RenderItem {
        RenderItem(std::string p_name, WE::RENDERITEM_TYPE p_type, GLuint p_shader_program, WE::UINT8 p_layer, bool p_active, glm::vec2 p_origin = glm::vec2(0.0f)) : name(p_name), type(p_type), shader_program(p_shader_program), layer(p_layer), active(p_active), origin(p_origin) {
            ;
        }
        std::string name = WE_EMPTY_STRING;
        WE::RENDERITEM_TYPE type = WE::RENDERITEM_TYPE::OBJECT;
        GLuint shader_program = 0;

        // 0 = LOWEST, 255 = HIGHEST
        WE::UINT8 layer = 0;
        bool active = true;
        glm::vec2 origin = glm::vec2(0.0f);

        std::shared_ptr<void> ptr = nullptr;

        RenderItem() = default;
    };

    using RENDER_BATCH = std::pair<GLuint, std::vector<std::shared_ptr<WE::RenderItem>>>;
}

#endif // WE_CORE_WEBSTERENGINE_H_