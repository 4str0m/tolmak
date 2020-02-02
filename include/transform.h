#pragma once

#include <common.h>

struct Transform
{
    glm::vec3 pos   = glm::vec3(0.f);
    glm::vec3 scale = glm::vec3(1.f);
    glm::vec3 rot   = glm::vec3(0.f);

    bool should_recompute_model = true;
    glm::mat4 model;
};

inline void transform_translate(Transform &transform, const glm::vec3& offset)
{
    transform.pos += offset;
    transform.should_recompute_model = true;
}
inline void transform_scale(Transform &transform, const glm::vec3& scale)
{
    transform.scale *= scale;
    transform.should_recompute_model = true;
}
inline void transform_rotate(Transform &transform, const glm::vec3& rotation)
{
    transform.rot += rotation;
    transform.should_recompute_model = true;
}

inline const glm::mat4& transform_get_model(Transform &transform)
{
    if (transform.should_recompute_model)
    {
        glm::mat4 scale_trans = glm::scale(glm::translate(glm::mat4(1.f), transform.pos), transform.scale);
        transform.model = glm::rotate(
            glm::rotate(
                glm::rotate(scale_trans, transform.rot.x, glm::vec3(1.f, 0.f, 0.f)),
                transform.rot.y, glm::vec3(0.f, 1.f, 0.f)),
            transform.rot.z, glm::vec3(0.f, 0.f, 1.f));
    }
    return transform.model;
}