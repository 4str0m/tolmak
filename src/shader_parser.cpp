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


enum UniformType
{
    INT,
    FLOAT, VEC2, VEC3, VEC4,
    MAT2, MAT3, MAT4,
    SAMPLER2D,

    UNIFORM_TYPE_COUNT
};

#define IS_FLOAT(x) ((x) == FLOAT || (x) == VEC2 || (x) == VEC3 || (x) == VEC4)

struct UniformTemplate
{
    const char* opengl_type;
    const char* cpp_type;
    const char* cpp_default_value;
    const char* opengl_function_postfix;
    const char* imgui_function;
    const char* underlying_type;
};

const UniformTemplate uniform_templates[UNIFORM_TYPE_COUNT] = {
    {"int", "int", " = 0", "1i", "SliderInt", "int"},
    
    {"float", "float", " = 0.f", "1f", "SliderFloat", "float"},
    {"vec2", "glm::vec2", " = glm::vec2(0.f)", "2f", "SliderFloat2", "float"},
    {"vec3", "glm::vec3", " = glm::vec3(0.f)", "3f", "SliderFloat3", "float"},
    {"vec4", "glm::vec4", " = glm::vec4(0.f)", "4f", "SliderFloat4", "float"},

    {"mat2", "glm::mat2", " = glm::mat2(0.f)", "mat2", nullptr, "float"},
    {"mat3", "glm::mat3", " = glm::mat3(0.f)", "mat3", nullptr, "float"},
    {"mat4", "glm::mat4", " = glm::mat4(0.f)", "mat4", nullptr, "float"},
    
    {"sampler2D", "uint32_t", "", "1i", nullptr, ""}
};

const char* material_use_fmt = R"(
inline void material_use(
    %s& material,
    const glm::mat4& mvp,
    const glm::mat4& model,
    const glm::vec3& eye
)
{
    uint32_t ui = 0;
    shader_bind(material.shader_id);
    shader_set_uniform_mat4(material.shader_id, material.uniform_locations[ui++], mvp);
    shader_set_uniform_mat4(material.shader_id, material.uniform_locations[ui++], model);
    shader_set_uniform_3f(material.shader_id, material.uniform_locations[ui++], eye);

    for(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {
        shader_set_uniform_3f(material.shader_id, material.uniform_locations[ui++], point_lights[i].pos);
        shader_set_uniform_3f(material.shader_id, material.uniform_locations[ui++], point_lights[i].color);
        shader_set_uniform_1f(material.shader_id, material.uniform_locations[ui++], point_lights[i].intensity);
    })";


const char* use_material_sampler_fmt = R"(
    shader_set_uniform_1i(material.shader_id, material.uniform_locations[ui++], %d);
    texture_bind(material.%s, %d);)";

const char* use_material_variable_fmt = R"(
    shader_set_uniform_%s(material.shader_id, material.uniform_locations[ui++], material.%s);)";

const char* material_create_fmt = R"(
inline void material_create(%s& material)
{
    shader_load(&material.shader_id, "%s");

    shader_bind(material.shader_id);
    material.uniform_locations.push_back(shader_find_uniform_location(material.shader_id, "MVP"));
    material.uniform_locations.push_back(shader_find_uniform_location(material.shader_id, "M"));
    material.uniform_locations.push_back(shader_find_uniform_location(material.shader_id, "EYE"));

    const char* point_light_pos_fmt = "point_lights[%%u].pos";
    const char* point_light_col_fmt = "point_lights[%%u].color";
    const char* point_light_intensity_fmt = "point_lights[%%u].intensity";
    char point_light_pos[256];
    char point_light_col[256];
    char point_light_intensity[256];
    for(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {
        snprintf(point_light_pos, 256, point_light_pos_fmt, i);
        snprintf(point_light_col, 256, point_light_col_fmt, i);
        snprintf(point_light_intensity, 256, point_light_intensity_fmt, i);
        material.uniform_locations.push_back(shader_find_uniform_location(material.shader_id, point_light_pos));
        material.uniform_locations.push_back(shader_find_uniform_location(material.shader_id, point_light_col));
        material.uniform_locations.push_back(shader_find_uniform_location(material.shader_id, point_light_intensity));
    })";

const char* material_create_variable_fmt = R"(
    material.uniform_locations.push_back(shader_find_uniform_location(material.shader_id, "%s"));)";

const char* struct_fmt = R"(
struct %s {
    uint32_t shader_id;
    Array<int> uniform_locations;
    )";
    
const char* struct_field_fmt = R"(
    %s %s%s;)";
const char* struct_color_fmt = R"(
    glm::vec3 %s = glm::vec3(%f, %f, %f);)";
const char* struct_float_fmt = R"(
    float %s = %f;)";

const char* imgui_fmt = R"(
inline void material_imgui(%s& material)
{
    ImGui::Begin("%s");)";

const char* imgui_sampler_fmt = R"(
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s");
    ImGui::SameLine();
    ImGui::Image((void*)(intptr_t)textures[material.%s].renderer_id, ImVec2(20, 20));)";

const char* imgui_float_variable_fmt = R"(
    ImGui::%s("%s", (%s*)&material.%s, %f, %f);)";
const char* imgui_int_variable_fmt = R"(
    ImGui::%s("%s", (%s*)&material.%s, %d, %d);)";

const char* imgui_color3_variable_fmt = R"(
    ImGui::ColorEdit3("%s", (float*)&material.%s);)";

struct UniformCustomParameter
{
    enum Type
    {
        NONE = -1,
        HIDE,
        RANGE,
        COLOR
    };

    union Data
    {
        struct {
            float min, max, start;
        } range_boundaries;
        struct {
            float r, g, b;
        } color;
    };

    Type type;
    Data data;
};

UniformCustomParameter custom_param = { UniformCustomParameter::Type::NONE };
bool has_custom_param = false;

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

    fprintf(outputs[CREATE_MATERIAL], material_create_fmt, struct_material_name, shader_file_path);
    fprintf(outputs[STRUCT], struct_fmt, struct_material_name);
    fprintf(outputs[USE_MATERIAL], material_use_fmt, struct_material_name);
    fprintf(outputs[IMGUI], imgui_fmt, struct_material_name, struct_material_name);

    bool has_non_trivial_imgui = false;

    char line[512];
    char line_bk[512] = {0};
    size_t len = 512;
    uint32_t texture_id = 0;
    while (fgets(line, len, shader_file) != nullptr) {

        strncpy(line_bk, line, len);

        // if it's a custom command
        if (line[0] == '%') {
            char* rest = line + 1;
            if (!strncmp(rest, "Range", 5)) {
                float min, max, start;
                if (3 != sscanf(rest, "Range(%f, %f, %f)", &min, &max, &start)) {
                    std::cout << "ERROR: bad formatting of custom uniform parameter (Range): " << line_bk << std::endl;
                    exit(EXIT_FAILURE);
                }
                has_custom_param = true;
                custom_param.type = UniformCustomParameter::Type::RANGE;
                custom_param.data.range_boundaries.min = min;
                custom_param.data.range_boundaries.max = max;
                custom_param.data.range_boundaries.start = start;
            } else if (!strncmp(rest, "Color", 5)) {
                float r, g, b;
                if (3 != sscanf(rest, "Color(%f, %f, %f)", &r, &g, &b)) {
                    std::cout << "ERROR: bad formatting of custom uniform parameter (Color): " << line_bk << std::endl;
                    exit(EXIT_FAILURE);
                }
                has_custom_param = true;
                custom_param.type = UniformCustomParameter::Type::COLOR;
                custom_param.data.color.r = r;
                custom_param.data.color.g = g;
                custom_param.data.color.b = b;
            } else if (!strncmp(rest, "Hide()", 6)) {
                if (rest[6] != '\n') {
                    std::cout << "ERROR: bad formatting of custom uniform parameter (Hide): " << line_bk << std::endl;
                    exit(EXIT_FAILURE);
                }
                has_custom_param = true;
                custom_param.type = UniformCustomParameter::Type::HIDE;
            }
            continue;
        }

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
            const UniformTemplate& uniform_template = uniform_templates[i];
            if (!strncmp(type, uniform_template.opengl_type, type_len))
            {
                if (has_custom_param) {
                    if (custom_param.type == UniformCustomParameter::Type::COLOR && i != VEC3) {
                        std::cout << "ERROR: invalid combination of uniform type and custom param: " << line_bk << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    else if (custom_param.type == UniformCustomParameter::Type::RANGE &&
                        !IS_FLOAT(i)) {
                        std::cout << "ERROR: invalid combination of uniform type and custom param: " << line_bk << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }

                if (has_custom_param && custom_param.type == UniformCustomParameter::Type::COLOR)
                    fprintf(outputs[STRUCT], struct_color_fmt, variable_name, custom_param.data.color.r, custom_param.data.color.g, custom_param.data.color.b);
                else if (has_custom_param && custom_param.type == UniformCustomParameter::Type::RANGE)
                    fprintf(outputs[STRUCT], struct_float_fmt, variable_name, custom_param.data.range_boundaries.start);
                else
                    fprintf(outputs[STRUCT], struct_field_fmt, uniform_template.cpp_type, variable_name, uniform_template.cpp_default_value);

                fprintf(outputs[CREATE_MATERIAL], material_create_variable_fmt, variable_name);
                if (i == SAMPLER2D) {
                    fprintf(outputs[USE_MATERIAL], use_material_sampler_fmt, texture_id, variable_name, texture_id);
                    fprintf(outputs[IMGUI], imgui_sampler_fmt, variable_name, variable_name);
                    texture_id++;
                }
                else {
                    fprintf(outputs[USE_MATERIAL], use_material_variable_fmt, uniform_template.opengl_function_postfix, variable_name);
    
                    if (uniform_template.imgui_function && !(has_custom_param && custom_param.type == UniformCustomParameter::Type::HIDE)) {
                        if (i == INT) {
                            fprintf(outputs[IMGUI], imgui_int_variable_fmt,
                                uniform_template.imgui_function,
                                variable_name,
                                uniform_template.underlying_type,
                                variable_name,
                                0, 1
                            );
                            has_non_trivial_imgui = true;
                       } else if (i == VEC3 && has_custom_param && custom_param.type == UniformCustomParameter::Type::COLOR) {
                            fprintf(outputs[IMGUI], imgui_color3_variable_fmt,
                                variable_name,
                                variable_name
                            );
                            has_non_trivial_imgui = true;
                        } else if (IS_FLOAT(i)) {
                            float min = 0.f, max = 1.f;
                            if (has_custom_param && custom_param.type == UniformCustomParameter::Type::RANGE) {
                                min = custom_param.data.range_boundaries.min;
                                max = custom_param.data.range_boundaries.max;
                            }
                            fprintf(outputs[IMGUI], imgui_float_variable_fmt,
                                uniform_template.imgui_function,
                                variable_name,
                                uniform_template.underlying_type,
                                variable_name,
                                min, max
                            );
                            has_non_trivial_imgui = true;
                        }
                    }
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
        has_custom_param = false;
    }
    fprintf(outputs[IMGUI], "\n    ImGui::End();\n}\n");
    fprintf(outputs[STRUCT], "\n};\n");
    fprintf(outputs[USE_MATERIAL], "\n}\n");
    fprintf(outputs[CREATE_MATERIAL], "\n}\n");

    for (int i = 0; i < CODE_ELEMENTS_COUNT; ++i)
    {
        fflush(outputs[i]);
        if (i != IMGUI || has_non_trivial_imgui)
            std::cout << buffers[i] << std::endl;
        else
            std::cout << "inline void material_imgui(" << struct_material_name << "& material) {}" << std::endl;
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
    for (int i = 1; i < argc; ++i)
        parse_shader(argv[i]);

    return 0;
}