#pragma once

#include <common.h>
#include <mesh_io.h>

struct Mesh
{
    uint32_t VAO, VBO, EBO;
    uint32_t indices_count;
};

void mesh_bind(const Mesh& mesh);
void mesh_from_mesh_data(const MeshData& mesh_data, Mesh& mesh);
void mesh_draw(const Mesh& mesh);

struct VertexAttribs
{
    struct VertexAttrib
    {
        int size;
        GLenum type;
        GLboolean normalized;
        void* offset;
    };

    uint64_t total_size = 0;
    std::vector<VertexAttrib> attribs;
};

void vertex_attribs_append(VertexAttribs& attribs, int size, GLenum type, GLboolean normalized = GL_FALSE);
void vertex_attribs_enable_all(const VertexAttribs& attribs, const Mesh& mesh);
