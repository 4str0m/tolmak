#pragma once

#include <common.h>

struct Camera
{
    float theta = 0.f;
    float phi   = 0.f;
    float distance_to_target = 10.f;

    float fov           = glm::radians(45.f);
    float aspect_ratio  = 0.f;
    float near          = 0.5f;
    float far           = 500.f;

    bool must_recompute = true;
    glm::vec3 eye       = glm::vec3(0.f, 0.f, -10.f);

    glm::vec3 forward   = glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 left      = glm::vec3(-1.f, 0.f, 0.f);
    glm::vec3 up        = glm::vec3(0.f, 1.f, 0.f);

    float speed         = 0.1f;
    bool free_flying = false;
};

inline void camera_handle_scroll(Camera& camera, double xoffset, double yoffset)
{
    float fact = yoffset < 0 ? 1.1 : 0.909;
    if (camera.free_flying) {
        camera.fov *= fact;
    } else {
        glm::vec3 target = camera.eye + camera.forward * camera.distance_to_target;
        camera.distance_to_target *= fact;
        camera.eye = target - camera.forward * camera.distance_to_target;
    }
}

inline glm::mat4 get_projection_matrix(const Camera& camera)
{
    return glm::perspective(camera.fov, camera.aspect_ratio, camera.near, camera.far);
}

inline glm::mat4 get_view_matrix(Camera& camera)
{
    if (camera.must_recompute)
    {
        float sin_theta = std::sin(camera.theta);
        float cos_theta = std::cos(camera.theta);
        float sin_phi = std::sin(camera.phi);
        float cos_phi = std::cos(camera.phi);

        glm::vec3 target = camera.eye + camera.forward * camera.distance_to_target;
        camera.forward = glm::vec3(cos_theta * cos_phi, sin_theta, cos_theta * sin_phi);
        camera.left = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), camera.forward));
        camera.up = glm::normalize(glm::cross(camera.forward, camera.left));

        if (!camera.free_flying)
            camera.eye = target - camera.forward * camera.distance_to_target;
        
        camera.must_recompute = false;        
    }
    return glm::lookAt(camera.eye, camera.eye + camera.forward, camera.up);
}

inline void camera_update(Camera& camera)
{
    if (keyboard_state.action[GLFW_KEY_UP] || keyboard_state.action[GLFW_KEY_W])
        camera.eye += camera.forward * camera.speed;

    if (keyboard_state.action[GLFW_KEY_DOWN] || keyboard_state.action[GLFW_KEY_S])
        camera.eye -= camera.forward * camera.speed;


    if (keyboard_state.action[GLFW_KEY_LEFT] || keyboard_state.action[GLFW_KEY_A])
        camera.eye += camera.left * camera.speed;

    if (keyboard_state.action[GLFW_KEY_RIGHT] || keyboard_state.action[GLFW_KEY_D])
        camera.eye -= camera.left * camera.speed;


    if (keyboard_state.action[GLFW_KEY_SPACE]) {
        if (keyboard_state.mods[GLFW_KEY_SPACE] & GLFW_MOD_CONTROL)
            camera.eye -= glm::vec3(0.f, 1.f, 0.f) * camera.speed;
        else
            camera.eye += glm::vec3(0.f, 1.f, 0.f) * camera.speed;
    }
}

inline void camera_handle_mouse_move(Camera& camera)
{
    if (camera.free_flying) {
        camera.theta = glm::clamp(camera.theta - mouse_state.dy * .002f, -HALF_PI + .01f, HALF_PI - .01f);
        camera.phi   = camera.phi + mouse_state.dx * .002f;

        camera.must_recompute = true;        
    } else {
        if (mouse_state.action[GLFW_MOUSE_BUTTON_MIDDLE] == GLFW_PRESS)
        {
            camera.theta = glm::clamp(camera.theta - mouse_state.dy * .01f, -HALF_PI + .01f, HALF_PI - .01f);
            camera.phi   = camera.phi + mouse_state.dx * .01f;

            camera.must_recompute = true;
        }
    }
}