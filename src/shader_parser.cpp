#include <iostream>
#include <sstream>
#include <string.h>
#include <ctype.h>
#include <cstdlib>
#include <cstdio>


const char* file_header = R"(
#pragma once
#include <common.h>
#include <shader.h>
#include <texture.h>
#include <light.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

)";

inline void parse_shader(const char* shader_file_path)
{
    FILE* shader_file = fopen(shader_file_path, "r");

    if (!shader_file) {
        std::cout << "ERROR: could not load shader file \"" << shader_file_path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* last_slash = strrchr(shader_file_path, '/');
    if (!last_slash)
    {
        std::cout << "ERROR: invalid file path \"" << shader_file_path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    char material_name[128];
    char struct_material_name[128];
    const char * postfix = "_material";
    uint32_t postfix_len = strlen(postfix);
    strncpy(struct_material_name, last_slash+1, 128 - postfix_len);
    char* dot = strrchr(struct_material_name, '.');
    if (!last_slash)
    {
        std::cout << "ERROR: invalid file path \"" << shader_file_path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    *dot = '\0';
    strncpy(material_name, struct_material_name, 128);
    strncpy(dot, postfix, postfix_len);

    bool should_be_upper = true;
    char *c, *t;
    for(c = struct_material_name, t = struct_material_name; *c; ++c, ++t)
    {
        if (should_be_upper) {
            *t = toupper(*c);
            should_be_upper = false;
        } else if(*c == '_') {
            should_be_upper = true;
            t--;
        } else {
            *t = *c;
        }
    }
    *t = '\0';

    enum UniformType
    {
        INT,
        FLOAT, VEC2, VEC3, VEC4,
        MAT2, MAT3, MAT4,
        SAMPLER2D,

        UNIFORM_TYPE_COUNT
    };

    const char* uniform_type_names[UNIFORM_TYPE_COUNT] = {
        "int",
        "float", "vec2", "vec3", "vec4",
        "mat2", "mat3", "mat4",
        "sampler2D"
    };

    const char* set_uniform_prefix = "shader_set_uniform_";
    const char* uniform_type_function_postfix[UNIFORM_TYPE_COUNT] = {
        "1i",
        "1f", "2f", "3f", "4f",
        "mat2", "mat3", "mat4",
        "1i"
    };

    const char* uniform_type_cpp[UNIFORM_TYPE_COUNT] = {
        "int",
        "float", "glm::vec2", "glm::vec3", "glm::vec4",
        "glm::mat2", "glm::mat3", "glm::mat4",
        "uint32_t"
    };

    const char* uniform_imgui[UNIFORM_TYPE_COUNT] = {
        "SliderInt",
        "SliderFloat", "SliderFloat2", "SliderFloat3", "SliderFloat4",
        nullptr, nullptr, nullptr,
        nullptr
    };

    const char* uniform_default_value[UNIFORM_TYPE_COUNT] = {
        " = 0",
        " = 0.f", " = glm::vec2(0.f)", " = glm::vec3(0.f)", " = glm::vec4(0.f)",
        " = glm::mat2(0.f)", " = glm::mat3(0.f)", " = glm::mat4(0.f)",
        ""
    };

    const char* uniform_underlying_type[UNIFORM_TYPE_COUNT] = {
        "int",
        "float", "float", "float", "float", 
        "float", "float", "float",
        ""
    };

    const char* use_material_fmt = R"(
inline void use_material(
    %s& material,
    const glm::mat4& mvp,
    const glm::mat4& model,
    const glm::vec3& eye
)
{
    shader_bind(material.shader);
    shader_set_uniform_mat4(material.shader, "MVP", mvp);
    shader_set_uniform_mat4(material.shader, "M", model);
    shader_set_uniform_3f(material.shader, "EYE", eye);

    const char* point_light_pos_fmt = "point_lights[%%u].pos";
    const char* point_light_col_fmt = "point_lights[%%u].color";
    char point_light_pos[256];
    char point_light_col[256];
    for(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {
        snprintf(point_light_pos, 256, point_light_pos_fmt, i);
        snprintf(point_light_col, 256, point_light_col_fmt, i);
        shader_set_uniform_3f(material.shader, point_light_pos, point_lights[i].pos);
        shader_set_uniform_3f(material.shader, point_light_col, point_lights[i].color);
    })";

    const char* use_material_sampler_fmt = R"(
    shader_set_uniform_1i(material.shader, "%s", %d);
    bind_texture(material.%s, %d);)";

    const char* use_material_variable_fmt = R"(
    shader_set_uniform_%s(material.shader, "%s", material.%s);)";

    const char* create_material_fmt = R"(
inline void create_material(%s& material)
{
    load_shader(material.shader, "%s");
}
)";

    const char* struct_fmt = R"(
struct %s {
    Shader shader;)";
    
    const char* struct_field_fmt = R"(
    %s %s%s;)";

    const char* imgui_fmt = R"(
inline void material_imgui(%s& material)
{
    ImGui::Begin("%s");)";

    const char* imgui_sampler_fmt = R"(
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s");
    ImGui::Image((void*)(intptr_t)textures[material.%s].renderer_id, ImVec2(70, 70));)";

    const char* imgui_variable_fmt = R"(
    ImGui::%s("%s", (%s*)&material.%s, 0, 1);)";

    enum CodeElements
    {
        STRUCT,
        USE_MATERIAL,
        CREATE_MATERIAL,
        IMGUI,
        CODE_ELEMENTS_COUNT
    };

    char* buffers[CODE_ELEMENTS_COUNT] = {0};
    size_t sizes[CODE_ELEMENTS_COUNT];
    FILE* outputs[CODE_ELEMENTS_COUNT] = {0};
    for (int i = 0; i < CODE_ELEMENTS_COUNT; ++i)
    {
        outputs[i] = open_memstream(&buffers[i], &sizes[i]);
        if (!outputs[i])
        {
            std::cout << "ERROR: could not create output buffer." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    fprintf(outputs[CREATE_MATERIAL], create_material_fmt, struct_material_name, shader_file_path);
    fprintf(outputs[STRUCT], struct_fmt, struct_material_name);
    fprintf(outputs[USE_MATERIAL], use_material_fmt, struct_material_name);
    fprintf(outputs[IMGUI], imgui_fmt, struct_material_name, struct_material_name);

    char line[512];
    char line_bk[512] = {0};
    size_t len = 512;
    uint32_t texture_id = 0;
    while (fgets(line, len, shader_file) != nullptr) {

        strncpy(line_bk, line, len);
        // only care about uniforms
        if (strncmp(line, "uniform ", 8))
            continue;

        // Extract uniform name and type
        char* type = line + 8;
        char* next_space = strchr(type, ' ');
        if (!next_space) {
            std::cout << "ERROR: invalid uniform variable: " << line_bk << std::endl;
            exit(EXIT_FAILURE);
        }

        *next_space = '\0';
        char* variable_name = next_space+1;
        char* semi_colon = strchr(variable_name, ';');
        if (!semi_colon) {
            std::cout << "ERROR: uniform variable did not end with semicolon: " << line_bk << std::endl;
            exit(EXIT_FAILURE);
        }
        *semi_colon = '\0';

        if (strchr(variable_name, ' ')) {
            std::cout << "ERROR: invalid uniform name: " << line_bk << std::endl;
            exit(EXIT_FAILURE);
        }

        // Check which type it is, and output right string based on that
        uint32_t type_len = strlen(type);
        bool found_uniform = false;
        for (uint32_t i = 0; i < UNIFORM_TYPE_COUNT; ++i)
        {
            if (!strncmp(type, uniform_type_names[i], type_len))
            {

                fprintf(outputs[STRUCT], struct_field_fmt, uniform_type_cpp[i], variable_name, uniform_default_value[i]);
                if (i == SAMPLER2D) {
                    fprintf(outputs[USE_MATERIAL], use_material_sampler_fmt, variable_name, texture_id, variable_name, texture_id);
                    fprintf(outputs[IMGUI], imgui_sampler_fmt, variable_name, variable_name);
                    texture_id++;
                }
                else {
                    fprintf(outputs[USE_MATERIAL], use_material_variable_fmt, uniform_type_function_postfix[i], variable_name, variable_name);
                    fprintf(outputs[IMGUI], imgui_variable_fmt, uniform_imgui[i], variable_name, uniform_underlying_type[i], variable_name);
                }

                found_uniform = true;
                break;
            }
        }
        if (!found_uniform)
        {
            std::cout << "ERROR: invalid uniform type: " << line_bk << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    fprintf(outputs[IMGUI], "\n}\n");
    fprintf(outputs[STRUCT], "\n};\n");
    fprintf(outputs[USE_MATERIAL], "\n}\n");

    for (int i = 0; i < CODE_ELEMENTS_COUNT; ++i)
    {
        fflush(outputs[i]);
        std::cout << buffers[i] << std::endl;
        fclose(outputs[i]);
        free(buffers[i]);
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <shader_files>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << file_header;

    for (int i = 1; i < argc; ++i) {
        parse_shader(argv[i]);
    }
    return 0;
}