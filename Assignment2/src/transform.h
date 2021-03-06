#include <csugl.h>

struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
    // rotate axis
    glm::vec3 axis;
    // rotate angle
    float angle;
    Transform() = default;
    Transform(const glm::vec3 &position, const glm::vec3 &axis,
                     float angle, const glm::vec3 &scale)
    : position(position), axis(axis), angle(angle), scale(scale) {
    }

    inline glm::mat4 get_trans_mat() const {
        // 实现平移、旋转、缩放变换的组合矩阵
        glm::mat4 t = glm::mat4(1.0f);
        t = glm::translate(t, position);
        t = glm::rotate(t, angle, axis);
        t = glm::scale(t, scale);
        return t;
    }
};