#pragma once

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "camera.h"

struct CameraController {
    Camera camera{};

    CameraController() {
        m_distance = glm::length(camera.position - m_target);

        const glm::vec3 direction = glm::normalize(camera.position - m_target);

        m_yaw   = std::atan2(direction.x, direction.z);
        m_pitch = std::asin(direction.y);
        UpdateCamera();
    }

    void SetTarget(const glm::vec3 &target) {
        m_target = target;
        UpdateCamera();
    }

    void Orbit(float dx, float dy) {
        constexpr float sensitivity = 0.005f;

        m_yaw -= dx * sensitivity;
        m_pitch -= dy * sensitivity;

        constexpr float limit = glm::radians(89.0f);

        m_pitch = std::clamp(m_pitch, -limit, limit);

        UpdateCamera();
    }

    void Pan(float dx, float dy, float viewport_height) {
        if (viewport_height <= 0.0f) {
            return;
        }

        const float fovy = glm::radians(camera.fovy);

        const float world_per_pixel = 2.0f * m_distance * std::tan(fovy * 0.5f) / viewport_height;

        const glm::vec3 right = camera.rotation * glm::vec3(1.0f, 0.0f, 0.0f);

        const glm::vec3 up = camera.rotation * glm::vec3(0.0f, 1.0f, 0.0f);

        m_target += (-right * dx + up * dy) * world_per_pixel;

        UpdateCamera();
    }

    void Zoom(float delta) {
        constexpr float sensitivity = 0.1f;

        m_distance *= std::exp(-delta * sensitivity);
        m_distance = std::clamp(m_distance, 0.1f, 1000.0f);

        UpdateCamera();
    }

    void UpdateCamera() {
        const float cos_pitch = std::cos(m_pitch);

        const glm::vec3 offset{std::sin(m_yaw) * cos_pitch, std::sin(m_pitch), std::cos(m_yaw) * cos_pitch};

        camera.position = m_target + offset * m_distance;
        camera.LookAt(m_target);
    }

private:
    glm::vec3 m_target{};
    float m_distance{8.0f}, m_yaw{}, m_pitch{};
};
