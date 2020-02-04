#include <mesh.h>

Array<Mesh> meshes;

void mesh_bind(uint32_t mesh_id)
{
    const Mesh& mesh = meshes[mesh_id];
    GLCall(glBindVertexArray(mesh.VAO));
}

void mesh_create(uint32_t *mesh_id, const char *file_path)
{
    size_t file_path_len = strlen(file_path);
    ASSERT(file_path_len < MAX_PATH_LENGTH, "file path too long: \"%s\".", file_path);

    for (uint32_t i = 0; i < meshes.size(); ++i) {
        if (!strncmp(file_path, meshes[i].file_path, MAX_PATH_LENGTH)) {
            *mesh_id = i;
            return;
        }
    }
    MeshData mesh_data;
    obj_load(mesh_data, file_path);

    Mesh mesh;
    GLCall(glGenVertexArrays(1, &mesh.VAO));
    GLCall(glGenBuffers(1, &mesh.VBO));
    GLCall(glBindVertexArray(mesh.VAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() * sizeof(MeshData::Vertex), mesh_data.vertices.data(), GL_STATIC_DRAW));

    GLCall(glGenBuffers(1, &mesh.EBO));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.indices.size() * sizeof(uint32_t), mesh_data.indices.data(), GL_STATIC_DRAW));

    mesh.indices_count = mesh_data.indices.size();
    memset(mesh.file_path, 0, MAX_PATH_LENGTH);
    strncpy(mesh.file_path, file_path, file_path_len);

    meshes.append(mesh);
    *mesh_id = meshes.size()-1;
}

void mesh_draw(uint32_t mesh_id)
{
    const Mesh& mesh = meshes[mesh_id];
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
    attribs.attribs.append({size, type, normalized, (void*) attribs.total_size});
    attribs.total_size += size * size_of_gl_type(type);
}

void vertex_attribs_enable_all(const VertexAttribs& attribs, uint32_t mesh_id)
{
    mesh_bind(mesh_id);
    for (uint32_t i = 0; i < attribs.attribs.size(); ++i)
    {
        const VertexAttribs::VertexAttrib attrib = attribs.attribs[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, attrib.size, attrib.type, attrib.normalized, attribs.total_size, attrib.offset));
    }
}