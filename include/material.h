#pragma once

#include <common.h>
#include <shader.h> 
#include <texture.h>
#include <light.h>

struct PhongMaterial
{
    Shader shader;
    Texture diffuse;
    Texture specular;
    Texture bump;
};

inline void create_phong_material(PhongMaterial& material, const char* diffuse_fname, const char* specular_fname, const char* bump_fname)
{
    load_shader(material.shader, "../resources/shaders/phong.glsl");
    load_texture(material.diffuse, diffuse_fname);
    load_texture(material.specular, specular_fname);
    load_texture(material.bump, bump_fname);
}

inline void use_material(PhongMaterial& material, const glm::mat4& mvp, const glm::mat4& model, const glm::vec3& eye, const PointLight& point_light)
{
    shader_bind(material.shader);
    shader_set_uniform_mat4(material.shader, "MVP", mvp);
    shader_set_uniform_mat4(material.shader, "M", model);
    shader_set_uniform_3f(material.shader, "EYE", eye);

    shader_set_uniform_3f(material.shader, "point_light.pos", point_light.pos);
    shader_set_uniform_3f(material.shader, "point_light.color", point_light.color);
    shader_set_uniform_1i(material.shader, "diff_tex", 0);
    shader_set_uniform_1i(material.shader, "spec_tex", 1);
    shader_set_uniform_1i(material.shader, "bump_tex", 2);
    
    bind_texture(material.diffuse, 0);
    bind_texture(material.specular, 1);
    bind_texture(material.bump, 2);
}