#include <iostream>
#include <sstream>
#include <string.h>
#include <ctype.h>
#include <cstdlib>

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
        "Texture"
    };

    std::stringstream struct_ss;
    std::stringstream use_material_ss;
    std::stringstream create_material_signature_ss;
    std::stringstream create_material_body_ss;
    std::stringstream imgui_ss;

    struct_ss << "struct " << struct_material_name << " {\n"
        "\tShader shader;\n";
    use_material_ss << "inline void use_material(" << struct_material_name << "& material, const glm::mat4& mvp,"
                    " const glm::mat4& model, const glm::vec3& eye)\n{\n"
                    "\tshader_bind(material.shader);\n"
                    "\tshader_set_uniform_mat4(material.shader, \"MVP\", mvp);\n"
                    "\tshader_set_uniform_mat4(material.shader, \"M\", model);\n"
                    "\tshader_set_uniform_3f(material.shader, \"EYE\", eye);\n"

                    "\tconst char* point_light_pos_fmt = \"point_lights[%u].pos\";\n"
                    "\tconst char* point_light_col_fmt = \"point_lights[%u].color\";\n"
                    "\tchar point_light_pos[256];\n"
                    "\tchar point_light_col[256];\n"
                    "\tfor(uint32_t i = 0; i < N_POINT_LIGHTS; ++i) {\n"
                    "\t\tsnprintf(point_light_pos, 256, point_light_pos_fmt, i);\n"
                    "\t\tsnprintf(point_light_col, 256, point_light_col_fmt, i);\n"
                    "\t\tshader_set_uniform_3f(material.shader, point_light_pos, point_lights[i].pos);\n"
                    "\t\tshader_set_uniform_3f(material.shader, point_light_col, point_lights[i].color);\n"
                    "\t}\n\n";

    create_material_signature_ss << "inline void create_material(\n\t"
        << struct_material_name << "& material";
    create_material_body_ss << "{\n"
        "\tload_shader(material.shader, \"" << shader_file_path << "\");\n";

    imgui_ss << "inline void material_imgui(" << struct_material_name << "& material)\n{\n"
        "\tImGui::Begin(\"" << struct_material_name << "\");\n";

    char line[512];
    char line_bk[512] = {0};
    size_t len = 512;
    uint32_t texture_id = 0;
    while (fgets(line, len, shader_file) != nullptr) {
        strncpy(line_bk, line, len);
        // only cares about uniforms
        if (strncmp(line, "uniform ", 8))
            continue;
        char* type = line + 8;
        char* next_space = strchr(type, ' ');
        if (!next_space)
        {
            std::cout << "ERROR: invalid uniform variable: " << line_bk << std::endl;
            exit(EXIT_FAILURE);
        }
        *next_space = '\0';
        char* variable_name = next_space+1;
        char* semi_colon = strchr(variable_name, ';');
        if (!semi_colon)
        {
            std::cout << "ERROR: uniform variable did not end with semicolon: " << line_bk << std::endl;
            exit(EXIT_FAILURE);
        }
        *semi_colon = '\0';

        if (strchr(variable_name, ' '))
        {
            std::cout << "ERROR: invalid uniform name: " << line_bk << std::endl;
            exit(EXIT_FAILURE);
        }

        uint32_t type_len = strlen(type);
        for (uint32_t i = 0; i < UNIFORM_TYPE_COUNT; ++i)
        {
            if (!strncmp(type, uniform_type_names[i], type_len))
            {
                struct_ss << "\t" << uniform_type_cpp[i] << " " << variable_name;
                if (i == SAMPLER2D)
                {
                    struct_ss << ";\n";
                    use_material_ss << "\t"
                        << set_uniform_prefix << uniform_type_function_postfix[i]
                        << "(material.shader, \"" << variable_name << "\", " << texture_id << ");\n"
                        << "\tbind_texture(material." << variable_name << ", " << texture_id << ");\n";

                    create_material_signature_ss << ",\n\tconst char* " << variable_name << "_fname";
                    create_material_body_ss << "\tload_texture(material." << variable_name << ", " << variable_name << "_fname);\n";
                    texture_id++;
                }
                else
                {
                    struct_ss << " = " << uniform_type_cpp[i] << "(0);\n";
                    use_material_ss << "\t"
                        << set_uniform_prefix << uniform_type_function_postfix[i]
                        << "(material.shader, \"" << variable_name << "\", material." << variable_name << ");\n";
                }
                if (i > INT && i <= VEC4) {
                    const char* imgui_func = i == VEC3 ? "ImGui::ColorEdit" : "ImGui::SliderFloat";
                    imgui_ss << "\t" << imgui_func;
                    if (i != 1) imgui_ss << i;
                    imgui_ss << "(\"" << variable_name << "\", (float*)&material." << variable_name;
                    if (i == VEC3) imgui_ss << ");\n";
                    else imgui_ss << ", 0.f, 1.f);\n";
                }
                break;
            }
        }
    }
    struct_ss << "};\n";
    use_material_ss << "}\n";
    create_material_signature_ss << ")\n";
    create_material_body_ss << "}\n";
    imgui_ss << "\tImGui::End();\n}\n";

    std::cout << struct_ss.str() << std::endl;
    std::cout << use_material_ss.str() << std::endl;
    std::cout << create_material_signature_ss.str() << create_material_body_ss.str() << std::endl;
    std::cout << imgui_ss.str() << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <shader_files>" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "#pragma once\n"
        "#include <common.h>\n"
        "#include <shader.h>\n" 
        "#include <texture.h>\n"
        "#include <light.h>\n"
        "#include <imgui.h>\n"
        "#include <examples/imgui_impl_glfw.h>\n"
        "#include <examples/imgui_impl_opengl3.h>\n" << std::endl;

    for (int i = 1; i < argc; ++i) {
        parse_shader(argv[i]);
    }
    return 0;
}