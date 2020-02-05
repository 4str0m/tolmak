#pragma once

#include <common.h>

struct MeshData
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normal;
        glm::vec3 tangent = glm::vec3(0.f);
        glm::vec3 bitangent = glm::vec3(0.f);
    };

    Array<Vertex> vertices;
    Array<uint32_t> indices;
};

void obj_load(MeshData& mesh, const char *filename);