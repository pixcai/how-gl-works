#pragma once

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

enum class CameraType {
    Perspective,
    Orthographic,
};

struct Camera {
    CameraType type{CameraType::Perspective};

    glm::vec3 position{0.0f, 4.0f, 8.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};

    float fovy{45.0f}, aspect{1.0f}, near{0.1f}, far{100.0f};
    float left{-1.0f}, right{1.0f}, bottom{-1.0f}, top{1.0f};

    void SetPerspective(float fovy, float aspect, float near, float far) {
        type         = CameraType::Perspective;
        this->fovy   = fovy;
        this->aspect = aspect;
        this->near   = near;
        this->far    = far;
    }

    void SetOrthographic(float left, float right, float bottom, float top, float near, float far) {
        type         = CameraType::Orthographic;
        this->left   = left;
        this->right  = right;
        this->bottom = bottom;
        this->top    = top;
        this->near   = near;
        this->far    = far;
    }

    void LookAt(glm::vec3 target) {
        auto view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
        rotation  = glm::normalize(glm::quat_cast(glm::inverse(view)));
    }

    glm::mat4 GetViewMatrix() const {
        auto inverse_rotate    = glm::mat4_cast(glm::conjugate(rotation));
        auto inverse_translate = glm::translate(glm::mat4(1.0f), -position);
        return inverse_rotate * inverse_translate;
    }

    glm::mat4 GetProjectionMatrix() const {
        if (type == CameraType::Orthographic) {
            return glm::ortho(left, right, bottom, top, near, far);
        }
        return glm::perspective(glm::radians(fovy), aspect, near, far);
    }
};
