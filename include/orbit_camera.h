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
};

inline glm::mat4 get_projection_matrix(const OrbitCamera& camera)
{
    return glm::perspective(camera.fov, camera.aspect_ratio, camera.near, camera.far);
}
inline glm::mat4 get_view_matrix(const OrbitCamera& camera)
{
	float sin_theta = std::sin(camera.theta);
	float cos_theta = std::cos(camera.theta);
	float sin_phi = std::sin(camera.phi);
	float cos_phi = std::cos(camera.phi);

	glm::vec3 pos(cos_theta * cos_phi, sin_theta, cos_theta * sin_phi);
	pos *= camera.distance_to_target;
    return glm::lookAt(pos, camera.target, glm::vec3(0.f, 1.f, 0.f));
}

inline void camera_handle_scroll(OrbitCamera& camera, double xoffset, double yoffset)
{
	camera.distance_to_target *= yoffset < 0 ? 1.1 : 0.909;
}
inline void camera_handle_mouse_move(OrbitCamera& camera, MouseState mouse_state)
{
	if (mouse_state.buttons[GLFW_MOUSE_BUTTON_LEFT])
	{
		camera.theta = glm::clamp(camera.theta + mouse_state.dy * .01f, -HALF_PI + .01f, HALF_PI - .01f);
		camera.phi   = camera.phi + mouse_state.dx * .01f;
	}
}