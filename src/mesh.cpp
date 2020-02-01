#include <mesh.h>

void mesh_bind(const Mesh& mesh)
{
    GLCall(glBindVertexArray(mesh.VAO));
}

void mesh_from_mesh_data(const MeshData& mesh_data, Mesh& mesh)
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

void mesh_draw(const Mesh& mesh)
{
    GLCall(glBindVertexArray(mesh.VAO));

    GLCall(glDrawElements(
        GL_TRIANGLES,      // mode
        mesh.indices_count,// count
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    ));
}

void vertex_attribs_append(VertexAttribs& attribs, int size, GLenum type, GLboolean normalized)
{
    attribs.attribs.push_back({size, type, normalized, (void*) attribs.total_size});
    attribs.total_size += size * size_of_gl_type(type);
}

void vertex_attribs_enable_all(const VertexAttribs& attribs, const Mesh& mesh)
{
    mesh_bind(mesh);
    for (uint32_t i = 0; i < attribs.attribs.size(); ++i)
    {
        const VertexAttribs::VertexAttrib attrib = attribs.attribs[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, attrib.size, attrib.type, attrib.normalized, attribs.total_size, attrib.offset));
    }
}