#ifndef SRC_TRIANGLE_H_
#define SRC_TRIANGLE_H_

#include <iostream>

#include <memory>
#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glad/gl.h>

#include "core/WebsterEngine.h"
#include "core/Logger.h"

class Triangle {
public:
    Triangle(std::vector<GLfloat> vertices);
    void Destroy();

    //////// BASIC ATTRIBUTES
    std::shared_ptr<WE_TRIANGLE_VERTICES> vertices = std::make_shared<WE_TRIANGLE_VERTICES>();

    //////// BASIC FUNCTIONS
    // verifys vertex_array and vertex_buffer is valid (non-zero)
    bool Verify();
    void LoadVertices(std::vector<GLfloat> vertices);
    void Build();
    void Render();
    void RenderOutline();

    //////// NORMAL FUNCTIONS
    glm::vec3 GetNormal();
    void UpdateNormal();
    void UpdateFullVertices();

    //////// COLOR FUNCTIONS
    void SetColor(glm::vec3 color);
    glm::vec3 GetColor();

private:
    std::shared_ptr<WE_TRIANGLE_VERTICES_NORMAL> full_vertices = std::make_shared<WE_TRIANGLE_VERTICES_NORMAL>();

    glm::vec3 normal = glm::vec3(0.0f);
    
    GLuint vertex_array = 0;
    GLuint vertex_buffer = 0;
};

#endif // SRC_TRIANGLE_H_