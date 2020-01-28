#pragma once

#include <string>

struct Mesh
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 uv;
        glm::normal normal;
    };

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

void load_obj(const std::string &filename, Mesh& mesh);