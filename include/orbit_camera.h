#pragma once

#include <common.h>

struct OrbitCamera
{
    glm::vec3 target;
    float theta, phi;
    float distance_to_target;

    float fov;
    float aspect_ratio;
    float near;
    float far;

    bool must_recompute_eye = true;
    glm::vec3 _eye = glm::vec3(0.f, 0.f, 0.f);
};

inline glm::mat4 get_projection_matrix(const OrbitCamera& camera)
{
    return glm::perspective(camera.fov, camera.aspect_ratio, camera.near, camera.far);
}
inline glm::mat4 get_view_matrix(OrbitCamera& camera)
{
    if (camera.must_recompute_eye)
    {
        float sin_theta = std::sin(camera.theta);
        float cos_theta = std::cos(camera.theta);
        float sin_phi = std::sin(camera.phi);
        float cos_phi = std::cos(camera.phi);

        camera._eye = glm::vec3(cos_theta * cos_phi, sin_theta, cos_theta * sin_phi) * camera.distance_to_target;
        camera.must_recompute_eye = false;
    }
    return glm::lookAt(camera._eye, camera.target, glm::vec3(0.f, 1.f, 0.f));
}

inline void camera_handle_scroll(OrbitCamera& camera, double xoffset, double yoffset)
{
    camera.distance_to_target *= yoffset < 0 ? 1.1 : 0.909;
    camera.must_recompute_eye = true;
}
inline void camera_handle_mouse_move(OrbitCamera& camera, MouseState mouse_state)
{
    if (mouse_state.buttons[GLFW_MOUSE_BUTTON_MIDDLE])
    {
        camera.theta = glm::clamp(camera.theta + mouse_state.dy * .01f, -HALF_PI + .01f, HALF_PI - .01f);
        camera.phi   = camera.phi + mouse_state.dx * .01f;

        camera.must_recompute_eye = true;
    }
}