#pragma once

#include <common.h>

enum class ComponentType : uint32_t
{
    TEST_COMPONENT,

    COMPONENT_TYPE_COUNT
};

struct Components
{
    Array<ComponentType> types;
    Array<uint32_t> ids;

    inline uint32_t size() const { return types.size(); }
};

struct TestComponent
{
    int32_t game_object_id = -1;
    
};
extern Array<TestComponent> test_components;

inline uint32_t test_component_create()
{
    test_components.push_back(TestComponent());
    return test_components.size()-1;
}

// void test_components_update()
// {
//     for (uint32_t i = 0; i < test_components.size(); ++i) {
        
//     }
// }

template<ComponentType type>
inline void component_set_game_object_id(uint32_t id, uint32_t game_object_id)
{
    LOG(WARNING, "unknown component type %u.", type);
}

template<>
inline void component_set_game_object_id<ComponentType::TEST_COMPONENT>(uint32_t id, uint32_t game_object_id)
{ test_components[id].game_object_id = game_object_id; }
