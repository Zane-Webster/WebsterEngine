#ifndef SRC_WEBSTERENGINE_H_
#define SRC_WEBSTERENGINE_H_

#include <iostream>

#define WE_EMPTY_STRING "WE_EMPTY_STRING"

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

#endif // SRC_WEBSTERENGINE_H_