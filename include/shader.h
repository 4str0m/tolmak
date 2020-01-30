#pragma once

#include <common.h>

struct Shader
{
    uint32_t program = 0;
    std::unordered_map<std::string, int> uniform_locations;
};

static inline int shader_finds_loc(Shader& shader, const char* uniform_name);

void load_shader(Shader& shader, const char* filename);

inline void shader_bind(Shader& shader)
{
    GLCall(glUseProgram(shader.program));
}


inline void shader_set_uniform_1i(Shader& shader, const char* uniform_name, int v)
{
    int loc = shader_finds_loc(shader, uniform_name); 
    GLCall(glUniform1i(loc, v));
}

inline void shader_set_uniform_1f(Shader& shader, const char* uniform_name, float v)
{
    int loc = shader_finds_loc(shader, uniform_name); 
    GLCall(glUniform1f(loc, v));
}
inline void shader_set_uniform_2f(Shader& shader, const char* uniform_name, const glm::vec2& v)
{
    int loc = shader_finds_loc(shader, uniform_name); 
    GLCall(glUniform2f(loc, v.x, v.y));
}
inline void shader_set_uniform_3f(Shader& shader, const char* uniform_name, const glm::vec3& v)
{
    int loc = shader_finds_loc(shader, uniform_name); 
    GLCall(glUniform3f(loc, v.x, v.y, v.z));
}
inline void shader_set_uniform_4f(Shader& shader, const char* uniform_name, const glm::vec4& v)
{
    int loc = shader_finds_loc(shader, uniform_name); 
    GLCall(glUniform4f(loc, v.x, v.y, v.z, v.w));
}

inline void shader_set_uniform_mat2(Shader& shader, const char* uniform_name, const glm::mat2& v)
{
    int loc = shader_finds_loc(shader, uniform_name);
    GLCall(glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(v)));
}
inline void shader_set_uniform_mat3(Shader& shader, const char* uniform_name, const glm::mat3& v)
{
    int loc = shader_finds_loc(shader, uniform_name);
    GLCall(glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(v)));
}
inline void shader_set_uniform_mat4(Shader& shader, const char* uniform_name, const glm::mat4& v)
{
    int loc = shader_finds_loc(shader, uniform_name);
    GLCall(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(v)));
}

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
