#ifndef SRC_SHADER_H_
#define SRC_SHADER_H_

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
    ~Shader();
    
    std::string name = WE_EMPTY_STRING;
    std::string path = WE_EMPTY_STRING;
    GLenum type = GL_NONE;

    WE_LOAD_STATE Load();
    GLuint Compile();

private:
    const std::vector<std::string> valid_exts = {".vert", ".geom", ".frag"};

    bool ready_to_compile = false;

    std::string ext = WE_EMPTY_STRING;
    std::string source = WE_EMPTY_STRING;
    
    std::string _ReadFromPath();
    bool _VerifyExtMatch();

};

#endif // SRC_SHADER_H_