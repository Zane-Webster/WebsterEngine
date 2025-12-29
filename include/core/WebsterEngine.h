#ifndef WE_CORE_WEBSTERENGINE_H_
#define WE_CORE_WEBSTERENGINE_H_

#include <iostream>
#include <array>

#include <glad/gl.h>

#define WE_EMPTY_STRING "WE_EMPTY_STRING"

using WE_TRIANGLE_VERTICES = std::array<GLfloat, 18>;
using WE_TRIANGLE_VERTICES_NORMAL = std::array<GLfloat, 27>;

enum WE_WINDOW_RESOLUTION {
    HD,
    FHD,
    QHD
};

enum WE_STATE {
    BOOT,
    MAIN_MENU,
    EDITOR,
    PAUSE_MENU,
    EXIT
};

enum WE_LOAD_STATE {
    SUCCESS,
    FAIL,
    PATH_NOT_FOUND,
    UNKNOWN_FILE_TYPE,
    INCORRECT_FILE_TYPE
};

class WE {
public:
    struct RenderItem {
        
    };
};

#endif // WE_CORE_WEBSTERENGINE_H_