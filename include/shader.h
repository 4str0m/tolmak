#pragma once

#include <common.h>

struct Shader
{
    uint32_t program = 0;
    std::unordered_map<std::string, int> uniform_locations;
};

extern std::vector<Shader> shaders;

static inline int shader_finds_loc(Shader& shader, const char* uniform_name);

void load_shader(Shader& shader, const char* filename);

inline void shader_bind(Shader& shader)
{
    GLCall(glUseProgram(shader.program));
}

#define SHADER_SET_UNIFORM(type, postfix1, postfix2, ...)\
inline void shader_set_uniform_##postfix1(Shader& shader, const char* uniform_name, const type& v) {\
    int loc = shader_finds_loc(shader, uniform_name);\
    GLCall(glUniform##postfix2(loc, __VA_ARGS__));\
}

SHADER_SET_UNIFORM(int, 1i, 1i, v)
SHADER_SET_UNIFORM(float, 1f, 1f, v)
SHADER_SET_UNIFORM(glm::vec2, 2f, 2f, v.x, v.y)
SHADER_SET_UNIFORM(glm::vec3, 3f, 3f, v.x, v.y, v.z)
SHADER_SET_UNIFORM(glm::vec4, 4f, 4f, v.x, v.y, v.z, v.w)
SHADER_SET_UNIFORM(glm::mat2, mat2, Matrix2fv, 1, GL_FALSE, glm::value_ptr(v))
SHADER_SET_UNIFORM(glm::mat3, mat3, Matrix3fv, 1, GL_FALSE, glm::value_ptr(v))
SHADER_SET_UNIFORM(glm::mat4, mat4, Matrix4fv, 1, GL_FALSE, glm::value_ptr(v))

static inline int shader_finds_loc(Shader& shader, const char* uniform_name)
{
    auto found = shader.uniform_locations.find(uniform_name);
    int loc;
    if (found == shader.uniform_locations.end())
    {
        loc = glGetUniformLocation(shader.program, uniform_name);
        if (loc == -1)
            std::cout << "Warning: uniform not found \"" << uniform_name << "\"." << std::endl;
        shader.uniform_locations[uniform_name] = loc;
    }
    else
    {
        loc = shader.uniform_locations[uniform_name];
    }
    return loc;
}
