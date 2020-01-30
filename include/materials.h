#pragma once
#include <common.h>
#include <shader.h>
#include <texture.h>
#include <light.h>

struct PhongMaterial {
	Shader shader;
	Texture diff_tex;
	Texture spec_tex;
	Texture bump_tex;
	glm::vec3 tint= glm::vec3(0.f);
	float specularity= float(0.f);
	float bump_strength= float(0.f);
};

inline void use_material(PhongMaterial& material, const glm::mat4& mvp, const glm::mat4& model, const glm::vec3& eye, const PointLight& point_light) {
	shader_bind(material.shader);
	shader_set_uniform_mat4(material.shader, "MVP", mvp);
	shader_set_uniform_mat4(material.shader, "M", model);
	shader_set_uniform_3f(material.shader, "EYE", eye);
	shader_set_uniform_3f(material.shader, "point_light.pos", point_light.pos);
	shader_set_uniform_3f(material.shader, "point_light.color", point_light.color);

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

inline void create_phong_material(
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

inline void phong_material_imgui(PhongMaterial& material)
{
	ImGui::Begin("PhongMaterial");
	ImGui::ColorEdit3("tint", (float*)&material.tint);
	ImGui::SliderFloat("specularity", (float*)&material.specularity, 0.f, 1.f);
	ImGui::SliderFloat("bump_strength", (float*)&material.bump_strength, 0.f, 1.f);
	ImGui::End();
}

