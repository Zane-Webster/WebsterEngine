#include "shader/Shader.h"

Shader::Shader(std::string p_name, std::string p_path, GLenum p_type) : name(p_name), path(p_path), type(p_type) {
    ;
}

WE_LOAD_STATE Shader::Load() {
    std::filesystem::path fs_path = path;

    ext = fs_path.extension().string();

    // ensure path exists and is correct file type
    if (!std::filesystem::exists(fs_path)) return WE_LOAD_STATE::PATH_NOT_FOUND;
    if (!Utils::VectorContains(valid_exts, ext)) return WE_LOAD_STATE::UNKNOWN_FILE_TYPE;
    if (!Shader::_VerifyExtMatch()) return WE_LOAD_STATE::INCORRECT_FILE_TYPE;

    // read source from path
    source = Shader::_ReadFromPath();
    if (source == WE_EMPTY_STRING) return WE_LOAD_STATE::FAIL;

    // mark shader as ready to compile
    ready_to_compile = true;
    return WE_LOAD_STATE::SUCCESS;
}

GLuint Shader::Compile() {
    if (!ready_to_compile) {
        Logger::Error("[Shader::Compile] SHADER MARKED NOT READY TO COMPILE");
        return 0;
    }

    GLuint shader = glCreateShader(type);
    
    // convert source to char before passing to shader
    const char* source_char = source.c_str();
    glShaderSource(shader, 1, &source_char, nullptr);

    // compile
    glCompileShader(shader);

    // log state
    int success = 0;
    char log[512] = {};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, log);
        Logger::Error(log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

std::string Shader::_ReadFromPath() {
    std::ifstream file(path);

    // open file, if error return empty
    if (!file.is_open()) {
        Logger::Error("[Shader::_ReadFromPath] FILE COULD NOT BE OPENED");
        return WE_EMPTY_STRING;
    }

    // read lines to data
    std::string line = "";
    std::string data = "";
    while (std::getline(file, line)) {
        data.append(line);
        data.append("\n");
    }

    file.close();

    return data;
}

bool Shader::_VerifyExtMatch() {
    switch (type) {
        case GL_VERTEX_SHADER:
            if (ext == ".vert") return true;
            break;
        case GL_GEOMETRY_SHADER:
            if (ext == ".geom") return true;
            break;
        case GL_FRAGMENT_SHADER:
            if (ext == ".frag") return true;
            break;
    }
    return false;
}