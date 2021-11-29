#include "transform.h"
#include "primitive.h"

#include <glad/glad.h>

#include <glog/logging.h>
#define PI 3.14159

///////////////////////
/// Quad
///////////////////////

Quad::Quad() { init(1.0f); }

Quad::Quad(int width, int height) {
    CHECK(height > 0) << "Quad: invalid height value: " << height;
    init(static_cast<float>(width) / height);
}

Quad::~Quad() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

// 绘制四边形
void Quad::draw(csugl::Ref<csugl::Shader> shader, const Transform& trans) const {    
    shader->use();
    // 传入当前变换的矩阵
    shader->set_mat4("_model", trans.get_trans_mat());
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

// 按 宽/长 比例生成顶点数据
void Quad::init(float ratio) {
    float vertices[] = {
        //坐标                           //纹理坐标  
        -0.5f * ratio,  -0.5f, 0.0f,     0.0f, 0.0f, 
         0.5f * ratio,  -0.5f, 0.0f,     1.0f, 0.0f, 
         0.5f * ratio,   0.5f, 0.0f,     1.0f, 1.0f, 
        -0.5f * ratio,   0.5f, 0.0f,     0.0f, 1.0f, 
    };
    uint32_t indices[] = {
        0, 1, 2, 
        2, 3, 0, 
    };
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, vertices, GL_STATIC_DRAW);
    // 设置两个顶点属性，分别是 position 和 texCoords
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void *)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void *)(sizeof(float) * 3));
    
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, indices, GL_STATIC_DRAW);
}

///////////////////////
/// Circle
///////////////////////

// 在此实现圆

Circle::Circle() {
    //
    float vertices[200] = { 0 };
    for (int i = 0; i < 40; i++) {
        vertices[5 * i] = 0.5f * cos(2 * 3.1415926 * i / 40);
        vertices[5 * i + 1] = 0.5f * sin(2 * 3.1415926 * i / 40);
        vertices[5 * i + 2] = 0.0f;
        vertices[5 * i + 3] = vertices[5 * i] + 0.5f;
        vertices[5 * i + 4] = vertices[5 * i + 1] + 0.5f;
    }

    // 顶点数组对象 Vertex Array Object
    //glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 200, vertices, GL_STATIC_DRAW);
    // 设置两个顶点属性，分别是 position 和 texCoords
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(sizeof(float) * 3));
}

Circle::~Circle() {
    // 不要忘记释放 vao vbo ibo
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Circle::draw(csugl::Ref<csugl::Shader> shader, const Transform& trans) const {
    //        // 激活shader程序
    shader->use();
    // 设置坐标
    shader->set_mat4("_model", trans.get_trans_mat());
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 200);
}
