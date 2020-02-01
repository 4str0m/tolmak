#include <shader.h>

#include <sstream>

std::vector<Shader> shaders;

static const char* common_uniforms =
"#version 460\n"
"#define N_POINT_LIGHTS 10u\n"
"uniform mat4 MVP;\n"
"uniform mat4 M;\n"
"uniform vec3 EYE;\n"
"struct PointLight {\n"
"    vec3 pos;\n"
"    vec3 color;\n"
"};\n"
"uniform PointLight point_lights[N_POINT_LIGHTS];\n";

void shader_bind(uint32_t shader_id)
{
    GLCall(glUseProgram(shaders[shader_id].program));
}

bool shader_load(uint32_t* shader_id, const char* file_path)
{
    size_t file_path_len = strlen(file_path);
    if (file_path_len > MAX_PATH_LENGTH)
    {
        std::cout << "Error: file path too long: " << file_path << std::endl;
        exit(EXIT_FAILURE); 
    }

    for (uint32_t i = 0; i < shaders.size(); ++i)
    {
        if (!strncmp(file_path, shaders[i].file_path, MAX_PATH_LENGTH))
        {
            *shader_id = i;
            return true;
        }
    }

    FILE* shader_file = fopen(file_path, "r");

    if (!shader_file) {
        std::cout << "ERROR: could not load shader file." << std::endl;
        return false;
    }

    std::cout << "Loading \"" << file_path << "\" .. ";

    enum State
    {
        NONE = -1,
        VERT,
        FRAG,
        GEOM,
        STATE_COUNT
    };

    GLuint shader_type[STATE_COUNT] = {
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER,
        GL_GEOMETRY_SHADER
    };

    std::stringstream sources[STATE_COUNT];
    for (int i = 0; i < STATE_COUNT; ++i)
        sources[i] << common_uniforms;

    State state = NONE;

    char line[512];
    size_t len = 512;
    bool has_geometry_shader = false;
    while (fgets(line, len, shader_file) != nullptr) {
        if (line[0] == '%')
            continue;

        if      (strstr(line, "#VERTEX#"))       state = VERT;
        else if (strstr(line, "#FRAGMENT#"))     state = FRAG;
        else if (strstr(line, "#GEOMETRY#")) {
            state = GEOM;
            has_geometry_shader = true;
        }

        if (state != NONE)
            sources[state] << line;
    }

    GLuint shader_programs[STATE_COUNT];

    for (int i = 0; i < STATE_COUNT; ++i)
    {
        if (i == GEOM && !has_geometry_shader)
            continue;
        std::string str_source;
        const char* c_str_source;
        str_source = sources[i].str();
        c_str_source = str_source.c_str();
        shader_programs[i] = glCreateShader(shader_type[i]);
        GLCall(glShaderSource(shader_programs[i], 1, &c_str_source, NULL));
        GLCall(glCompileShader(shader_programs[i]));
    }

    GLuint program = glCreateProgram();
    if (!program)
    {
        std::cout << "Error: could not create program." << std::endl;
        return false;
    }
    GLCall(glAttachShader(program, shader_programs[VERT]));
    GLCall(glAttachShader(program, shader_programs[FRAG]));
    if (has_geometry_shader)
        GLCall(glAttachShader(program, shader_programs[GEOM]));
    GLCall(glLinkProgram(program));

    GLCall(glDeleteShader(shader_programs[VERT]));
    GLCall(glDeleteShader(shader_programs[FRAG]));
    if (has_geometry_shader)
        GLCall(glDeleteShader(shader_programs[GEOM]));
    
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
        return false;
    }

    Shader shader;
    shader.program = program;

    memset(shader.file_path, 0, MAX_PATH_LENGTH);
    strncpy(shader.file_path, file_path, file_path_len);

    shaders.emplace_back(shader);
    *shader_id = shaders.size()-1;

    std::cout << " done." << std::endl;
    return true;
}
