#ifndef SRC_SHADERHANDLER_H_
#define SRC_SHADERHANDLER_H_

#include <iostream>

#include <glad/gl.h>

#include <memory>
#include <unordered_map>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

#include "shader/Shader.h"

class ShaderHandler {
public:
    ShaderHandler();
    void Destroy();

    WE_LOAD_STATE AddShader(std::string name, std::string path, GLenum type);
    void CompileProgram(std::string name);

    GLuint GetProgram(std::string name);

private:
    std::unordered_map<std::string, std::vector<std::unique_ptr<Shader>>> shaders = {};
    std::unordered_map<std::string, GLuint> shader_programs = {};

};

#endif // SRC_SHADERHANDLER_H_