#pragma once

#include <common.h>

#define N_POINT_LIGHTS 10u

struct PointLight
{
    glm::vec3 pos = glm::vec3(0.f);
    glm::vec3 color = glm::vec3(0.f);
};

extern PointLight point_lights[N_POINT_LIGHTS];

void init_lights();
void draw_lights(const glm::mat4& mvp);
