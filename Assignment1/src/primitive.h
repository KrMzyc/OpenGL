#include <csugl.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glog/logging.h>

#define PI 3.14159265358279

// 抽象图元
struct Primitive {
    Primitive() : vao(-1), vbo(-1), ebo(-1) {}
    
    // 绘制函数的抽象
    // shader: 绘制所需的shader
    virtual void draw(csugl::Ref<csugl::Shader> shader) const = 0;

    // 判断图元对点的包含性
    // pos: 待判断的点的NDC坐标
    // return: 是否在内部
    virtual bool is_contained(const glm::vec2 &pos) const = 0;
    
    // 你必须手动实现子类的析构函数，用于释放gl对象
    virtual ~Primitive(){}
    
    // 图元属性
    glm::vec2 position; // 坐标
    glm::vec3 color;    // 颜色

protected:
    // OpenGL所需的vao,vbo和ebo ID
    GLuint vao, vbo, ebo;
};

// 三角形
struct Triangle : public Primitive {
    Triangle() {        
        // 原始顶点数据
        float vertices[] = {
            0.0f,  0.0f,  0.0f, 
            0.2f,  0.0f,  0.0f, 
            0.0f,  0.2f,  0.0f, 
        };
        // 顶点数组对象 Vertex Array Object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // 顶点缓冲对象 Vertex Buffer Object
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        // 设置顶点属性 (告诉GPU如何理解vertices)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glEnableVertexAttribArray(0);
    }

    ~Triangle() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void draw(csugl::Ref<csugl::Shader> shader) const override {
        // 激活shader程序
        shader->Use();
        // 设置坐标
        shader->SetVector2("_position", position);
        // 设置颜色
        shader->SetVector3("_color", color);
        // 绑定当前要绘制的顶点数组
        glBindVertexArray(vao);
        // 绘制
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    
    bool is_contained(const glm::vec2 &pos) const override {
        auto sub = pos - position;
        if (sub.x >= 0 && sub.x <= 0.2f &&    // x约束
            sub.y >= 0 && sub.y <= 0.2f &&    // y约束
            sub.x + sub.y <= 0.2f)            // x+y约束
            return true;
        return false;
    }

};

// 矩形: 要求使用索引数组实现
struct Rectangle : public Primitive {
    Rectangle() 
    {
        float vertices[] = {
            0.0f,  0.0f,  0.0f,
            0.2f,  0.0f,  0.0f,
            0.0f,  0.2f,  0.0f,
            0.2f,  0.2f,  0.0f,
        };

        unsigned int indices[] = { // 注意索引从0开始! 
            0, 1, 2, // 第一个三角形
            1, 2, 3  // 第二个三角形
        };

        // 顶点数组对象 Vertex Array Object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // 顶点缓冲对象 Vertex Buffer Object
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 设置顶点属性 (告诉GPU如何理解vertices)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
       
    }

    ~Rectangle() 
    {
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void draw(csugl::Ref<csugl::Shader> shader) const override 
    {
        // 激活shader程序
        shader->Use();
        // 设置坐标
        shader->SetVector2("_position", position);
        // 设置颜色
        shader->SetVector3("_color", color);
        // 绑定当前要绘制的顶点数组
        glBindVertexArray(vao);
        // 绘制
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    bool is_contained(const glm::vec2 &pos) const override 
    {
        auto sub = pos - position;
        if (sub.x >= 0 && sub.x <= 0.2f &&    // x约束
            sub.y >= 0 && sub.y <= 0.2f     // y约束
            )            // x+y约束
            return true;
        return false;
    }

};

// 圆: 要求使用索引数组实现
#define CircleSampleNumber 40  // 圆周顶点个数
struct Circle : public Primitive {
    Circle() 
    {
        float vertices[123] = { 0 };
        vertices[120] = 0.0f;
        vertices[121] = 0.0f;
        vertices[122] = 0.0f;

        for (int i = 0; i < 40; i++)
        {
            vertices[3 * i] = 0.2f * sin(2 * PI * i / 40);
            vertices[3 * i + 1] = 0.2f * cos(2 * PI * i / 40);
            vertices[3 * i + 2] = 0.0f;
        }

        unsigned int indices[120] = { 0 };
        for (int i = 0; i < 40; i++)
        {
            indices[3 * i] = 40;
            indices[3 * i + 1] = i % 40;
            indices[3 * i + 2] = (i + 1) % 40;
        }

        // 顶点数组对象 Vertex Array Object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // 顶点缓冲对象 Vertex Buffer Object
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 设置顶点属性 (告诉GPU如何理解vertices)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    }

    ~Circle() 
    {
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void draw(csugl::Ref<csugl::Shader> shader) const override 
    {
        // 激活shader程序
        shader->Use();
        // 设置坐标
        shader->SetVector2("_position", position);
        // 设置颜色
        shader->SetVector3("_color", color);
        // 绑定当前要绘制的顶点数组
        glBindVertexArray(vao);
        // 绘制
        glDrawElements(GL_TRIANGLES, 120, GL_UNSIGNED_INT, 0);
    }

    bool is_contained(const glm::vec2 &pos) const override 
    {
        auto sub = pos - position;
        if (pow(sub.x,2.0)+pow(sub.y,2.0)<=0.2f*0.2f)            // x+y约束
            return true;
        return false;
    }

};