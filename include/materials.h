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
	int light_index = int(0);
};

inline void use_material(LightMaterial& material, const glm::mat4& mvp, const glm::mat4& model, const glm::vec3& eye)
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

inline void create_material(
	LightMaterial& material)
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
	float uv_scale = float(0);
	Texture diff_tex;
	Texture spec_tex;
	Texture bump_tex;
	glm::vec3 tint = glm::vec3(0);
	float specularity = float(0);
	float bump_strength = float(0);
};

inline void use_material(PhongMaterial& material, const glm::mat4& mvp, const glm::mat4& model, const glm::vec3& eye)
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

inline void create_material(
	PhongMaterial& material,
	const char* diff_tex_fname,
	const char* spec_tex_fname,
	const char* bump_tex_fname)
{
	load_shader(material.shader, "/home/benoit/Desktop/cpp/tolmak/resources/shaders/phong.glsl");
	load_texture(material.diff_tex, diff_tex_fname);
	load_texture(material.spec_tex, spec_tex_fname);
	load_texture(material.bump_tex, bump_tex_fname);
}

inline void material_imgui(PhongMaterial& material)
{
	ImGui::Begin("PhongMaterial");
	ImGui::SliderFloat("uv_scale", (float*)&material.uv_scale, 0.f, 1.f);
	ImGui::ColorEdit3("tint", (float*)&material.tint);
	ImGui::SliderFloat("specularity", (float*)&material.specularity, 0.f, 1.f);
	ImGui::SliderFloat("bump_strength", (float*)&material.bump_strength, 0.f, 1.f);
	ImGui::End();
}

