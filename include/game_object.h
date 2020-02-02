#pragma once

#include <common.h>
#include <transform.h>

static uint32_t UID = 1;

struct GameObject
{
    uint32_t uid;
    uint32_t mesh_id;
    Transform transform;
};


inline void game_object_create(GameObject& game_object, const char* obj_file_path)
{
    game_object.uid = UID++;

    mesh_create(&game_object.mesh_id, obj_file_path);

    VertexAttribs attribs;
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 2, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_append(attribs, 3, GL_FLOAT);
    vertex_attribs_enable_all(attribs, game_object.mesh_id);
}

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