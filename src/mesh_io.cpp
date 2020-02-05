#include <mesh_io.h>

void obj_load(MeshData& mesh, const char *file_path) {
    /// Vertex indices used by the OBJ format
    struct obj_vertex {
        uint32_t p = (uint32_t) -1;
        uint32_t n = (uint32_t) -1;
        uint32_t uv = (uint32_t) -1;

        inline obj_vertex() { }

        inline obj_vertex(const char *str) {
            // TODO: handle various cases (no uvs, no normals, etc...)
            if (sscanf(str, "%u/%u/%u", &p, &uv, &n) != 3) {
                std::cout << "Invalid vertex data: \"" << str << "\"" << std::endl;
                throw std::runtime_error("Could not load obj file");
            }
        }

        inline bool operator==(const obj_vertex &v) const {
            return v.p == p && v.n == n && v.uv == uv;
        }
    };

    /// Hash function for obj_vertex
    struct obj_vertexHash : std::unary_function<obj_vertex, size_t> {
        std::size_t operator()(const obj_vertex &v) const {
            size_t hash = std::hash<uint32_t>()(v.p);
            hash = hash * 37 + std::hash<uint32_t>()(v.uv);
            hash = hash * 37 + std::hash<uint32_t>()(v.n);
            return hash;
        }
    };

    typedef std::unordered_map<obj_vertex, uint32_t, obj_vertexHash> VertexMap;

    FILE* file = fopen(file_path, "r");
    if (!file) {
        LOG(ERROR, "invalid file path: \"%s\".", file_path);
    }

    Array<glm::vec3>   positions;
    Array<glm::vec2>   texcoords;
    Array<glm::vec3>   normals;
    Array<uint32_t>   indices;
    Array<obj_vertex> vertices;
    VertexMap vertexMap;

    char line[256];
    char line_bk[256];
    int line_number = 0;
    size_t len = 256;
    while (fgets(line, len, file) != nullptr) {
        line_number++;
        memcpy(line_bk, line, 256);
        char* prefix = line;
        char* rest = line;
        while(!isspace(*rest)) ++rest;
        *(rest++) = '\0';

        size_t prefix_length = (size_t)(rest-prefix);
        if (!strncmp(prefix, "v", prefix_length)) {
            glm::vec3 p;
            if (sscanf(rest, "%f %f %f", &p.x, &p.y, &p.z) != 3)
                LOG(ERROR, "invalid vertex position: \"%s\" [%s:%d].", line_bk, file_path, line_number);
            positions.append(p);
        } else if (!strncmp(prefix, "vt", prefix_length)) {
            glm::vec2 tc;
            if (sscanf(rest, "%f %f", &tc.x, &tc.y) != 2)
                LOG(ERROR, "invalid texture coordinate: \"%s\" [%s:%d].", line_bk, file_path, line_number);
            texcoords.append(tc);
        } else if (!strncmp(prefix, "vn", prefix_length)) {
            glm::vec3 n;
            if (sscanf(rest, "%f %f %f", &n.x, &n.y, &n.z) != 3)
                LOG(ERROR, "invalid vertex normal: \"%s\" [%s:%d].", line_bk, file_path, line_number);
            normals.append(n);
        } else if (!strncmp(prefix, "f", prefix_length)) {
            char v1[64], v2[64], v3[64], v4[64];
            int count = 0;
            if ((count = sscanf(rest, "%s %s %s %s", v1, v2, v3, v4)) < 3)
                LOG(ERROR, "invalid face indices: \"%s\" [%s:%d].", line_bk, file_path, line_number);

            obj_vertex tri[6];
            int nVertices = 3;

            tri[0] = obj_vertex(v1);
            tri[1] = obj_vertex(v2);
            tri[2] = obj_vertex(v3);

            if (count == 4) {
                /* This is a quad, split into two triangles */
                tri[3] = obj_vertex(v4);
                tri[4] = tri[0];
                tri[5] = tri[2];
                nVertices = 6;
            }
            /* Convert to an indexed vertex list */
            for (int i=0; i<nVertices; ++i) {
                const obj_vertex &v = tri[i];
                VertexMap::const_iterator it = vertexMap.find(v);
                if (it == vertexMap.end()) {
                    vertexMap[v] = (uint32_t) vertices.size();
                    indices.append((uint32_t) vertices.size());
                    vertices.append(v);
                } else {
                    indices.append(it->second);
                }
            }
        }
    }

    mesh.indices.resize(indices.size());
    memcpy(mesh.indices.data(), indices.data(), sizeof(uint32_t)*indices.size());

    mesh.vertices.resize(vertices.size());
    for (uint32_t i = 0; i < vertices.size(); ++i) {
        mesh.vertices[i].pos    = positions[vertices[i].p-1];
        mesh.vertices[i].uv     = texcoords[vertices[i].uv-1];
        mesh.vertices[i].normal = normals[vertices[i].n-1];
    }

    // compute tangent and bitangent vactor for each triangle
    for (uint32_t i = 0; i < mesh.indices.size(); i+=3)
    {
        uint32_t i0 = mesh.indices[i+0];
        uint32_t i1 = mesh.indices[i+1];
        uint32_t i2 = mesh.indices[i+2];

        MeshData::Vertex& v0 = mesh.vertices[i0];
        MeshData::Vertex& v1 = mesh.vertices[i1];
        MeshData::Vertex& v2 = mesh.vertices[i2];

        glm::vec2 delta_uv0 = v1.uv - v0.uv;
        glm::vec2 delta_uv1 = v2.uv - v0.uv;

        glm::vec3 edge_0 = v1.pos - v0.pos;
        glm::vec3 edge_1 = v2.pos - v0.pos;

        float det = 1 / (delta_uv0.x*delta_uv1.y - delta_uv0.y*delta_uv1.x);

        glm::vec3 tangent   = det * ( edge_0 * delta_uv1.y - edge_1 * delta_uv0.y);
        glm::vec3 bitangent = det * (-edge_0 * delta_uv1.x + edge_1 * delta_uv0.x);

        v0.tangent += tangent;
        v0.bitangent += bitangent;
        v1.tangent += tangent;
        v1.bitangent += bitangent;
        v2.tangent += tangent;
        v2.bitangent += bitangent;
    }
    LOG(SUCCESS, "finished loading obj file: \"%s\".", file_path);
}
