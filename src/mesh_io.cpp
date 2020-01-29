#include <mesh_io.h>

void load_obj(const char *filename, MeshData& mesh) {
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

    FILE* file = fopen(filename, "r");
    if (!file) {
        std::cout << "Invalid file name: \"" << filename << "\"" << std::endl;
        throw std::runtime_error("Unable to open OBJ file !");
    }
    std::cout << "Loading \"" << filename << "\" .. ";
    std::cout.flush();

    std::vector<glm::vec3>   positions;
    std::vector<glm::vec2>   texcoords;
    std::vector<glm::vec3>   normals;
    std::vector<uint32_t>   indices;
    std::vector<obj_vertex> vertices;
    VertexMap vertexMap;

    char line[256];
    char line_bk[256];
    size_t len = 256;
    while (fgets(line, len, file) != nullptr) {
        memcpy(line_bk, line, 256);
        char* prefix = line;
        char* rest = line;
        while(!isspace(*rest)) ++rest;
        *(rest++) = '\0';

        size_t prefix_length = (size_t)(rest-prefix);
        if (!strncmp(prefix, "v", prefix_length)) {
            glm::vec3 p;
            if (sscanf(rest, "%f %f %f", &p.x, &p.y, &p.z) != 3) {
                std::cout << "Invalid vertex position data: \"" << line_bk << "\"" << std::endl;
                throw std::runtime_error("Could not load obj file");
            }
            positions.push_back(p);
        } else if (!strncmp(prefix, "vt", prefix_length)) {
            glm::vec2 tc;
            if (sscanf(rest, "%f %f", &tc.x, &tc.y) != 2) {
                std::cout << "Invalid texture coordinate data: \"" << line_bk << "\"" << std::endl;
                throw std::runtime_error("Could not load obj file");
            }
            texcoords.push_back(tc);
        } else if (!strncmp(prefix, "vn", prefix_length)) {
            glm::vec3 n;
            if (sscanf(rest, "%f %f %f", &n.x, &n.y, &n.z) != 3) {
                std::cout << "Invalid vertex normals data: \"" << line_bk << "\"" << std::endl;
                throw std::runtime_error("Could not load obj file");
            }
            normals.push_back(n);
        } else if (!strncmp(prefix, "f", prefix_length)) {
            char v1[64], v2[64], v3[64], v4[64];
            int count = 0;
            if ((count = sscanf(rest, "%s %s %s %s", v1, v2, v3, v4)) < 3) {
                std::cout << "Invalid vertex normals data: \"" << line_bk << "\"" << std::endl;
                throw std::runtime_error("Could not load obj file");
            }
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
                    indices.push_back((uint32_t) vertices.size());
                    vertices.push_back(v);
                } else {
                    indices.push_back(it->second);
                }
            }
        }
    }

    mesh.indices.resize(indices.size());
    memcpy(mesh.indices.data(), indices.data(), sizeof(uint32_t)*indices.size());

    mesh.vertices.resize(vertices.size());
    for (uint32_t i = 0; i < vertices.size(); ++i) {
        mesh.vertices[i].pos    = positions.at(vertices[i].p-1);
        mesh.vertices[i].uv     = texcoords.at(vertices[i].uv-1);
        mesh.vertices[i].normal = normals.at(vertices[i].n-1);
    }
    std::cout << " done." << std::endl;
}
