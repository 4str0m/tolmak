#pragma once

#include <common.h>
#include <mesh_io.h>

struct Mesh
{
    uint32_t VAO, VBO, EBO;
    uint32_t indices_count;
};

inline void mesh_from_mesh_data(const MeshData& mesh_data, Mesh& mesh)
{
    GLCall(glGenVertexArrays(1, &mesh.VAO));
    GLCall(glGenBuffers(1, &mesh.VBO));
    GLCall(glBindVertexArray(mesh.VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() * sizeof(MeshData::Vertex), mesh_data.vertices.data(), GL_STATIC_DRAW));

    GLCall(glGenBuffers(1, &mesh.EBO));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.indices.size() * sizeof(uint32_t), mesh_data.indices.data(), GL_STATIC_DRAW));
    mesh.indices_count = mesh_data.indices.size();
}

inline void bind_mesh(const Mesh& mesh)
{
    GLCall(glBindVertexArray(mesh.VAO));
}

inline void draw_mesh(const Mesh& mesh)
{
    GLCall(glBindVertexArray(mesh.VAO));

    GLCall(glDrawElements(
        GL_TRIANGLES,      // mode
        mesh.indices_count,    // count
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    ));
}