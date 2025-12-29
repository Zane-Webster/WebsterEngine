#include "shader/ShaderHandler.h"

ShaderHandler::ShaderHandler() {
    ;
}

void ShaderHandler::Destroy() {
    for (auto& [name, program] : shader_programs) {
        if (program != 0) {
            glDeleteProgram(program);
        }
    }
}

WE_LOAD_STATE ShaderHandler::AddShader(std::string name, std::string path, GLenum type) {
    shaders[name].push_back(std::make_unique<Shader>(name, path, type));

    WE_LOAD_STATE load_state = WE_LOAD_STATE::SUCCESS;

    load_state = shaders[name].back()->Load();
    if (load_state != WE_LOAD_STATE::SUCCESS) {
        Logger::LoadState("ShaderHandler::NewShader", load_state);
        return load_state;
    }

    return WE_LOAD_STATE::SUCCESS;
}

void ShaderHandler::CompileProgram(std::string name) {
    if (!shaders.contains(name)) {
        Logger::Error("[ShaderHandler::CompileProgram] PROGRAM NOT FOUND: " + name);
        return;
    }
    
    shader_programs[name] = glCreateProgram();
    std::vector<GLuint> compiled_shaders;

    for (const auto& shader : shaders[name]) {
        if (!shader->ready_to_compile) {
            Logger::Error("[ShaderHandler::CompileProgram] SHADER NOT READY: " + shader->path);
            glDeleteProgram(shader_programs[name]);
            return;
        }

        // --- compile shader ---
        GLuint shader_id = glCreateShader(shader->type);

        const char* src = shader->source.c_str();
        glShaderSource(shader_id, 1, &src, nullptr);
        glCompileShader(shader_id);

        GLint success = 0;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

        if (!success) {
            char log[1024];
            glGetShaderInfoLog(shader_id, 1024, nullptr, log);
            Logger::Error(log);

            glDeleteShader(shader_id);
            glDeleteProgram(shader_programs[name]);
            return;
        }

        glAttachShader(shader_programs[name], shader_id);
        compiled_shaders.push_back(shader_id);
    }

    // --- link program ---
    glLinkProgram(shader_programs[name]);

    GLint link_success = 0;
    glGetProgramiv(shader_programs[name], GL_LINK_STATUS, &link_success);

    if (!link_success) {
        char log[1024];
        glGetProgramInfoLog(shader_programs[name], 1024, nullptr, log);
        Logger::Error(log);

        for (GLuint s : compiled_shaders)
            glDeleteShader(s);

        glDeleteProgram(shader_programs[name]);
        return;
    }

    // shaders no longer needed after linking
    for (GLuint s : compiled_shaders) {
        glDeleteShader(s);
    }

    Logger::Info("SHADER: [" + name + "] SUCCESSFULLY COMPILED");
}

GLuint ShaderHandler::GetProgram(std::string name) {
    if (!shaders.contains(name)) {
        Logger::Error("[ShaderHandler::GetProgram] PROGRAM NOT FOUND: " + name);
        return 0;
    }
    return shader_programs[name];
}