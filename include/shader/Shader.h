#ifndef WE_SHADER_SHADER_H_
#define WE_SHADER_SHADER_H_

#include <iostream>

#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

#include <glad/gl.h>

#include "core/WebsterEngine.h"
#include "core/Logger.h"
#include "utils/Utils.h"

class Shader {
public:
    Shader(std::string name, std::string path, GLenum type);
    
    std::string name = WE_EMPTY_STRING;
    std::string path = WE_EMPTY_STRING;
    std::string source = WE_EMPTY_STRING;
    GLenum type = GL_NONE;

    bool ready_to_compile = false;

    WE_LOAD_STATE Load();
    GLuint Compile();

private:
    const std::vector<std::string> valid_exts = {".vert", ".geom", ".frag"};

    std::string ext = WE_EMPTY_STRING;
    
    std::string _ReadFromPath();
    bool _VerifyExtMatch();

};

#endif // WE_SHADER_SHADER_H_