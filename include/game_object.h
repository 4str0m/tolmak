#pragma once

#include <common.h>
#include <transform.h>
#include <mesh.h>

struct GameObject
{
    uint32_t uid;
    uint32_t mesh_id;
    Transform transform;
};

void game_object_create(GameObject& game_object, const char* obj_file_path);

template<typename Material>
inline void game_object_draw(
    GameObject& game_object,
    Material& mat,
    const glm::mat4& VP,
    const glm::vec3& EYE)
{
    const glm::mat4& M = transform_get_model(game_object.transform);
    material_use(mat, VP * M, M, EYE);
    mesh_draw(game_object.mesh_id);
}