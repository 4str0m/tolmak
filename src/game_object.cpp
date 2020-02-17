#include <game_object.h>

static uint32_t UID = 1;
BucketAllocator<GameObject, 32> game_objects;

GameObject* get_game_object_from_id(uint32_t game_object_id)
{
    for (auto it = game_objects.begin(); it != game_objects.end(); ++it)
        if (it->uid == game_object_id)
            return &*it;
    return nullptr;
}

uint32_t game_object_create(const char* obj_file_path)
{
    GameObject* game_object = game_objects.allocate();

    game_object->uid = UID++;

    mesh_create(&game_object->mesh_id, obj_file_path);
    return game_object->uid;
}

int32_t game_object_get_component(uint32_t game_object_id, ComponentType type)
{
    GameObject* game_object = get_game_object_from_id(game_object_id);
    if (!game_object)
        return -1;
    for (int32_t i = 0; i < (int32_t)game_object->components.size(); ++i)
    {
        if (game_object->components.types[i] == type)
            return i;
    }
    return -1;
}

void game_object_get_components(uint32_t game_object_id, ComponentType type, Array<uint32_t> &ids)
{
    GameObject* game_object = get_game_object_from_id(game_object_id);
    if (!game_object)
        return;

    ids.clear();
    for (uint32_t i = 0; i < (uint32_t)game_object->components.size(); ++i)
    {
        if (game_object->components.types[i] == type)
            ids.push_back(i);
    }
}
