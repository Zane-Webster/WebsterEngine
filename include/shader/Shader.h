#ifndef SRC_SHADER_H_
#define SRC_SHADER_H_

#include <iostream>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

class Shader {
public:
    Shader();
    ~Shader();

    WE_LOAD_STATE Load();
    void Compile();

private:

};

#endif // SRC_SHADER_H_