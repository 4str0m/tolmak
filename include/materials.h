
#pragma once
#include <common.h>
#include <shader.h>
#include <texture.h>
#include <light.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>


struct LightMaterial {
    Shader shader;
    int light_index = 0;
};


inline void use_material(
    LightMaterial& material,
    const glm::mat4& mvp,
    const glm::mat4& model,
    const glm::vec3& eye
)
{
    shader_bind(material.shader);
    shader_set_uniform_mat4(material.shader, "MVP", mvp);
    shader_set_uniform_mat4(material.shader, "M", model);
    shader_set_uniform_3f(material.shader, "EYE", eye);

    const char* point_light_pos_fmt = "point_lights[%u].pos";
    const char* point_light_col_fmt = "point_lights[%u].color";
    char point_light_pos[256];
    char point_light_col[256];
    for(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {
        snprintf(point_light_pos, 256, point_light_pos_fmt, i);
        snprintf(point_light_col, 256, point_light_col_fmt, i);
        shader_set_uniform_3f(material.shader, point_light_pos, point_lights[i].pos);
        shader_set_uniform_3f(material.shader, point_light_col, point_lights[i].color);
    }
    shader_set_uniform_1i(material.shader, "light_index", material.light_index);
}


inline void create_material(LightMaterial& material)
{
    load_shader(material.shader, "/home/benoit/Desktop/cpp/tolmak/resources/shaders/light.glsl");
}


inline void material_imgui(LightMaterial& material)
{
    ImGui::Begin("LightMaterial");
    ImGui::End();
}


struct PhongMaterial {
    Shader shader;
    float uv_scale = 0.f;
    uint32_t diff_tex;
    uint32_t spec_tex;
    uint32_t bump_tex;
    glm::vec3 tint = glm::vec3(0.f);
    float specularity = 0.f;
    float bump_strength = 0.f;
};


inline void use_material(
    PhongMaterial& material,
    const glm::mat4& mvp,
    const glm::mat4& model,
    const glm::vec3& eye
)
{
    shader_bind(material.shader);
    shader_set_uniform_mat4(material.shader, "MVP", mvp);
    shader_set_uniform_mat4(material.shader, "M", model);
    shader_set_uniform_3f(material.shader, "EYE", eye);

    const char* point_light_pos_fmt = "point_lights[%u].pos";
    const char* point_light_col_fmt = "point_lights[%u].color";
    char point_light_pos[256];
    char point_light_col[256];
    for(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {
        snprintf(point_light_pos, 256, point_light_pos_fmt, i);
        snprintf(point_light_col, 256, point_light_col_fmt, i);
        shader_set_uniform_3f(material.shader, point_light_pos, point_lights[i].pos);
        shader_set_uniform_3f(material.shader, point_light_col, point_lights[i].color);
    }
    shader_set_uniform_1f(material.shader, "uv_scale", material.uv_scale);
    shader_set_uniform_1i(material.shader, "diff_tex", 0);
    bind_texture(material.diff_tex, 0);
    shader_set_uniform_1i(material.shader, "spec_tex", 1);
    bind_texture(material.spec_tex, 1);
    shader_set_uniform_1i(material.shader, "bump_tex", 2);
    bind_texture(material.bump_tex, 2);
    shader_set_uniform_3f(material.shader, "tint", material.tint);
    shader_set_uniform_1f(material.shader, "specularity", material.specularity);
    shader_set_uniform_1f(material.shader, "bump_strength", material.bump_strength);
}


inline void create_material(PhongMaterial& material)
{
    load_shader(material.shader, "/home/benoit/Desktop/cpp/tolmak/resources/shaders/phong.glsl");
}


inline void material_imgui(PhongMaterial& material)
{
    ImGui::Begin("PhongMaterial");
    ImGui::SliderFloat("uv_scale", (float*)&material.uv_scale, 0.100000, 10.000000);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("diff_tex");
    ImGui::SameLine();
    ImGui::Image((void*)(intptr_t)textures[material.diff_tex].renderer_id, ImVec2(20, 20));
    ImGui::AlignTextToFramePadding();
    ImGui::Text("spec_tex");
    ImGui::SameLine();
    ImGui::Image((void*)(intptr_t)textures[material.spec_tex].renderer_id, ImVec2(20, 20));
    ImGui::AlignTextToFramePadding();
    ImGui::Text("bump_tex");
    ImGui::SameLine();
    ImGui::Image((void*)(intptr_t)textures[material.bump_tex].renderer_id, ImVec2(20, 20));
    ImGui::ColorEdit3("tint", (float*)&material.tint);
    ImGui::SliderFloat("specularity", (float*)&material.specularity, 0.000000, 1.000000);
    ImGui::SliderFloat("bump_strength", (float*)&material.bump_strength, 0.000000, 1.000000);
    ImGui::End();
}

