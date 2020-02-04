#pragma once

#include <common.h>
#include <mesh_io.h>

struct Mesh
{
    char file_path[MAX_PATH_LENGTH];
    uint32_t VAO, VBO, EBO;
    uint32_t indices_count;
};

extern Array<Mesh> meshes;

void mesh_create(uint32_t *mesh_id, const char *file_path);
void mesh_bind(uint32_t mesh_id);
void mesh_draw(uint32_t mesh_id);

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
    Array<VertexAttrib> attribs;
};

void vertex_attribs_append(VertexAttribs& attribs, int size, GLenum type, GLboolean normalized = GL_FALSE);
void vertex_attribs_enable_all(const VertexAttribs& attribs, uint32_t mesh_id);
