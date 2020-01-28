#pragma once

#include <common.h>

struct Mesh
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normal;
    };

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

void load_obj(const char *filename, Mesh& mesh);
