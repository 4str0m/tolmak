#pragma once

#include <common.h>

static uint32_t UID = 1;

struct GameObject
{
    uint32_t uid;
    uint32_t mesh_id;
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
    const GameObject& game_object,
    Material& mat,
    const glm::mat4& MVP,
    const glm::mat4& M,
    const glm::vec3& EYE)
{
    material_use(mat, MVP, M, EYE);
    mesh_draw(game_object.mesh_id);
}