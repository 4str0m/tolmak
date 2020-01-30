#include <shader.h>

#include <sstream>

void load_shader(Shader& shader, const char* filename)
{
    FILE* shader_file = fopen(filename, "r");

    if (!shader_file) {
        std::cout << "ERROR: could not load shader file." << std::endl;
        exit(EXIT_FAILURE);
    }
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

    State state = NONE;

    char line[512];
    size_t len = 512;
    bool has_geometry_shader = false;
    while (fgets(line, len, shader_file) != nullptr) {
        if      (strstr(line, "#VERTEX#"))         state = VERT;
        else if (strstr(line, "#FRAGMENT#"))     state = FRAG;
        else if (strstr(line, "#GEOMETRY#")) {
            state = GEOM;
            has_geometry_shader = true;
        }

        if (state != NONE)
            sources[state] << line;
    }

    GLuint shaders[STATE_COUNT];

    for (int i = 0; i < STATE_COUNT; ++i)
    {
        if (i == GEOM && !has_geometry_shader)
            continue;
        std::string str_source;
        const char* c_str_source;
        str_source = sources[i].str();
        c_str_source = str_source.c_str();
        shaders[i] = glCreateShader(shader_type[i]);
        GLCall(glShaderSource(shaders[i], 1, &c_str_source, NULL));
        GLCall(glCompileShader(shaders[i]));
    }

    GLuint program = glCreateProgram();
    GLCall(glAttachShader(program, shaders[VERT]));
    GLCall(glAttachShader(program, shaders[FRAG]));
    if (has_geometry_shader)
        GLCall(glAttachShader(program, shaders[GEOM]));
    GLCall(glLinkProgram(program));

    GLCall(glDeleteShader(shaders[VERT]));
    GLCall(glDeleteShader(shaders[FRAG]));
    if (has_geometry_shader)
        GLCall(glDeleteShader(shaders[GEOM]));
    
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
        exit(EXIT_FAILURE);
    }

    shader.program = program;
}
