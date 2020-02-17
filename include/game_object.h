#pragma once

#include <common.h>
#include <transform.h>
#include <mesh.h>
#include <components.h>

struct GameObject
{
    uint32_t uid;
    uint32_t mesh_id;
    Transform transform;

    Components components;
};

extern BucketAllocator<GameObject, 32> game_objects;

GameObject* get_game_object_from_id(uint32_t game_object_id);

uint32_t game_object_create(const char* obj_file_path);
void game_object_destroy(uint32_t game_object_id);

template<ComponentType type>
void game_object_add_component(uint32_t game_object_id, uint32_t id)
{
    GameObject* game_object = get_game_object_from_id(game_object_id);
    if (!game_object)
        return;
    game_object->components.types.push_back(type);
    game_object->components.ids.push_back(id);
    component_set_game_object_id<type>(id, game_object_id);
}

int32_t game_object_get_component(uint32_t game_object_id, ComponentType type);
void game_object_get_components(uint32_t game_object_id, ComponentType type, Array<uint32_t> &ids);

template<typename Material>
inline void game_object_draw(
    uint32_t game_object_id,
    Material& mat,
    const glm::mat4& VP,
    const glm::vec3& EYE)
{
    GameObject* game_object = get_game_object_from_id(game_object_id);
    if (!game_object)
        return;
    const glm::mat4& M = transform_get_model(game_object->transform);
    material_use(mat, VP * M, M, EYE);
    mesh_draw(game_object->mesh_id);
}