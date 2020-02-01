#pragma once

#include <common.h>

#define N_POINT_LIGHTS 10u

struct PointLight
{
    glm::vec3 pos = glm::vec3(0.f);
    glm::vec3 color = glm::vec3(0.f);
};

extern PointLight point_lights[N_POINT_LIGHTS];

void lights_init();
void lights_draw(const glm::mat4& mvp);
