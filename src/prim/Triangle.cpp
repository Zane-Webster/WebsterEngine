#include "prim/Triangle.h"

Triangle::Triangle(WE_TRIANGLE_VERTICES p_vertices) {
    Triangle::LoadVertices(p_vertices);
    Triangle::Build();
}

void Triangle::Destroy() {
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    vertex_array = 0;
    vertex_buffer = 0;
}

//=============================
// BASIC FUNCTIONS
//=============================

bool Triangle::Verify() {
    if (vertex_array == 0 || vertex_buffer == 0) return false;
    return true;
}

void Triangle::LoadVertices(WE_TRIANGLE_VERTICES p_vertices) {
    vertices = std::make_shared<WE_TRIANGLE_VERTICES>();
    full_vertices = std::make_shared<WE_TRIANGLE_VERTICES_NORMAL>();
    normal = glm::vec3(0.0f);

    if (p_vertices.size() == 18) {
        std::copy(p_vertices.begin(), p_vertices.end(), vertices->begin());
        Triangle::UpdateFullVertices();
    }
}

void Triangle::Build() {
    if (vertex_array || vertex_buffer) {
        glDeleteVertexArrays(1, &vertex_array);
        glDeleteBuffers(1, &vertex_buffer);
    }
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);

    glBindVertexArray(vertex_array);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, full_vertices->size() * sizeof(GLfloat), full_vertices->data(), GL_STATIC_DRAW);

    GLsizei stride = 9 * sizeof(GLfloat);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(0));
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Triangle::Render() {
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 3);  // Draw the triangle (filled)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Triangle::RenderOutline() {
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_LINE_LOOP, 0, 3);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//=============================
// NORMAL FUNCTIONS
//=============================

glm::vec3 Triangle::GetNormal() {
    Triangle::UpdateNormal();
    return normal;
}

void Triangle::UpdateNormal() {
    glm::vec3 point_1 = glm::vec3((*vertices)[0], (*vertices)[1], (*vertices)[2]);
    glm::vec3 point_2 = glm::vec3((*vertices)[6], (*vertices)[7], (*vertices)[8]);
    glm::vec3 point_3 = glm::vec3((*vertices)[12], (*vertices)[13], (*vertices)[14]);
    normal = glm::normalize(glm::cross(point_2 - point_1, point_3 - point_1));
}

void Triangle::UpdateFullVertices() {
    Triangle::UpdateNormal();

    int v_stride = 6; // vertices/true_vertices stride
    int f_stride = 9; // full_vertices stride

    for (int i = 0; i < 3; i++) {
        int v_idx = i * v_stride;
        int f_idx = i * f_stride;

        // Position
        (*full_vertices)[f_idx + 0] = (*vertices)[v_idx + 0];
        (*full_vertices)[f_idx + 1] = (*vertices)[v_idx + 1];
        (*full_vertices)[f_idx + 2] = (*vertices)[v_idx + 2];

        // Color
        (*full_vertices)[f_idx + 3] = (*vertices)[v_idx + 3];
        (*full_vertices)[f_idx + 4] = (*vertices)[v_idx + 4];
        (*full_vertices)[f_idx + 5] = (*vertices)[v_idx + 5];

        // Normal
        (*full_vertices)[f_idx + 6] = normal.x;
        (*full_vertices)[f_idx + 7] = normal.y;
        (*full_vertices)[f_idx + 8] = normal.z;
    }
}
//=============================
// COLOR FUNCTIONS
//=============================

void Triangle::SetColor(glm::vec3 color) {
    int stride = 6;
    for (int i = 0; i < 3; i++) {
        int index = i * stride + 3; // +3 skips past x,y,z to get to r,g,b
        
        (*vertices)[index + 0] = color.r;
        (*vertices)[index + 1] = color.g;
        (*vertices)[index + 2] = color.b;
    }

    Triangle::UpdateFullVertices();

    Triangle::Build();
}

glm::vec3 Triangle::GetColor() {
    return glm::vec3((*vertices)[3],(*vertices)[4],(*vertices)[5]);
}