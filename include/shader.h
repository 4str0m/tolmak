#pragma once

#include <common.h>

struct Shader
{
    char file_path[MAX_PATH_LENGTH];
    uint32_t program = 0;
    // std::unordered_map<std::string, int> uniform_locations;
};

extern Array<Shader> shaders;

void shaders_terminate();

bool shader_load(uint32_t* shader_id, const char* file_name);
void shader_bind(uint32_t shader_id);

#define SHADER_SET_UNIFORM(type, postfix1, postfix2, ...)\
inline void shader_set_uniform_##postfix1(uint32_t shader_id, int loc, const type& v)\
{\
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

inline int shader_find_uniform_location(uint32_t shader_id, const char* uniform_name)
{
    Shader& shader = shaders[shader_id];
    // auto found = shader.uniform_locations.find(uniform_name);
    // int loc;
    // if (found == shader.uniform_locations.end())
    // {
        int loc = glGetUniformLocation(shader.program, uniform_name);
        if (loc == -1)
            LOG(WARNING, "uniforn not found: %s [%s]", uniform_name, shaders[shader_id].file_path);
        return loc;
        // shader.uniform_locations[uniform_name] = loc;
    // }
    // else
    //     loc = shader.uniform_locations[uniform_name];
    // return loc;
}
