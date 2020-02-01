
#pragma once
#include <common.h>
#include <shader.h>
#include <texture.h>
#include <light.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>


struct LightMaterial {
    uint32_t shader_id;
    int light_index = 0;
};


inline void material_use(
    LightMaterial& material,
    const glm::mat4& mvp,
    const glm::mat4& model,
    const glm::vec3& eye
)
{
    shader_bind(material.shader_id);
    shader_set_uniform_mat4(material.shader_id, "MVP", mvp);
    shader_set_uniform_mat4(material.shader_id, "M", model);
    shader_set_uniform_3f(material.shader_id, "EYE", eye);

    const char* point_light_pos_fmt = "point_lights[%u].pos";
    const char* point_light_col_fmt = "point_lights[%u].color";
    char point_light_pos[256];
    char point_light_col[256];
    for(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {
        snprintf(point_light_pos, 256, point_light_pos_fmt, i);
        snprintf(point_light_col, 256, point_light_col_fmt, i);
        shader_set_uniform_3f(material.shader_id, point_light_pos, point_lights[i].pos);
        shader_set_uniform_3f(material.shader_id, point_light_col, point_lights[i].color);
    }
    shader_set_uniform_1i(material.shader_id, "light_index", material.light_index);
}


inline void material_create(LightMaterial& material)
{
    shader_load(&material.shader_id, "../resources/shaders/light.glsl");
}

inline void material_imgui(LightMaterial& material) {}

struct PhongMaterial {
    uint32_t shader_id;
    float uv_scale = 1.000000;
    uint32_t diff_tex;
    uint32_t spec_tex;
    uint32_t bump_tex;
    glm::vec3 tint = glm::vec3(1.000000, 1.000000, 1.000000);
    float specularity = 1.000000;
    float bump_strength = 0.500000;
};


inline void material_use(
    PhongMaterial& material,
    const glm::mat4& mvp,
    const glm::mat4& model,
    const glm::vec3& eye
)
{
    shader_bind(material.shader_id);
    shader_set_uniform_mat4(material.shader_id, "MVP", mvp);
    shader_set_uniform_mat4(material.shader_id, "M", model);
    shader_set_uniform_3f(material.shader_id, "EYE", eye);

    const char* point_light_pos_fmt = "point_lights[%u].pos";
    const char* point_light_col_fmt = "point_lights[%u].color";
    char point_light_pos[256];
    char point_light_col[256];
    for(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {
        snprintf(point_light_pos, 256, point_light_pos_fmt, i);
        snprintf(point_light_col, 256, point_light_col_fmt, i);
        shader_set_uniform_3f(material.shader_id, point_light_pos, point_lights[i].pos);
        shader_set_uniform_3f(material.shader_id, point_light_col, point_lights[i].color);
    }
    shader_set_uniform_1f(material.shader_id, "uv_scale", material.uv_scale);
    shader_set_uniform_1i(material.shader_id, "diff_tex", 0);
    texture_bind(material.diff_tex, 0);
    shader_set_uniform_1i(material.shader_id, "spec_tex", 1);
    texture_bind(material.spec_tex, 1);
    shader_set_uniform_1i(material.shader_id, "bump_tex", 2);
    texture_bind(material.bump_tex, 2);
    shader_set_uniform_3f(material.shader_id, "tint", material.tint);
    shader_set_uniform_1f(material.shader_id, "specularity", material.specularity);
    shader_set_uniform_1f(material.shader_id, "bump_strength", material.bump_strength);
}


inline void material_create(PhongMaterial& material)
{
    shader_load(&material.shader_id, "../resources/shaders/phong.glsl");
}


inline void material_imgui(PhongMaterial& material)
{
    ImGui::Begin("PhongMaterial");
    ImGui::SliderFloat("uv_scale", (float*)&material.uv_scale, 0.100000, 5.000000);
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


struct PlainColorMaterial {
    uint32_t shader_id;
    glm::vec3 tint = glm::vec3(1.000000, 0.000000, 0.000000);
};


inline void material_use(
    PlainColorMaterial& material,
    const glm::mat4& mvp,
    const glm::mat4& model,
    const glm::vec3& eye
)
{
    shader_bind(material.shader_id);
    shader_set_uniform_mat4(material.shader_id, "MVP", mvp);
    shader_set_uniform_mat4(material.shader_id, "M", model);
    shader_set_uniform_3f(material.shader_id, "EYE", eye);

    const char* point_light_pos_fmt = "point_lights[%u].pos";
    const char* point_light_col_fmt = "point_lights[%u].color";
    char point_light_pos[256];
    char point_light_col[256];
    for(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {
        snprintf(point_light_pos, 256, point_light_pos_fmt, i);
        snprintf(point_light_col, 256, point_light_col_fmt, i);
        shader_set_uniform_3f(material.shader_id, point_light_pos, point_lights[i].pos);
        shader_set_uniform_3f(material.shader_id, point_light_col, point_lights[i].color);
    }
    shader_set_uniform_3f(material.shader_id, "tint", material.tint);
}


inline void material_create(PlainColorMaterial& material)
{
    shader_load(&material.shader_id, "../resources/shaders/plain_color.glsl");
}


inline void material_imgui(PlainColorMaterial& material)
{
    ImGui::Begin("PlainColorMaterial");
    ImGui::ColorEdit3("tint", (float*)&material.tint);
    ImGui::End();
}

