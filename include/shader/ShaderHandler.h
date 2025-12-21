#ifndef SRC_SHADERHANDLER_H_
#define SRC_SHADERHANDLER_H_

#include <iostream>

#include <glad/gl.h>

#include <unordered_map>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "shader/Shader.h"

class ShaderHandler {
public:
    ShaderHandler();
    ~ShaderHandler();

    WE_LOAD_STATE NewShader();

private:
    std::unordered_map<std::string, Shader> shaders = {};
    std::unordered_map<std::string, GLuint> shader_programs = {};

};

#endif // SRC_SHADERHANDLER_H_