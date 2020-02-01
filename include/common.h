#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdarg>

#define PI 3.141592654f
#define HALF_PI 1.570796327f
#define QUARTER_PI 0.785398163f
#define TWO_PI 6.283185307f

#define MAX_PATH_LENGTH 256

#define GLCall(x)\
    do {\
        GL_clear_errors();\
        x;\
        if (GL_log_error(#x, __FILE__, __LINE__)) exit(EXIT_FAILURE);\
    } while(0)

#define LOG(level, fmt, ...) log(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define ASSERT(cond, fmt, ...)\
do {\
    if (!(cond)) LOG(ERROR, fmt, ##__VA_ARGS__);\
} while(0)

enum LogLevel
{
    INFO,
    SUCCESS,
    WARNING,
    ERROR
};

void log(LogLevel level, const char* file, int line, const char *fmt, ...);

struct MouseState
{
    float x, y;
    float dx, dy;
    int action[GLFW_MOUSE_BUTTON_LAST];
    int mods[GLFW_MOUSE_BUTTON_LAST];
};

struct KeyboardState
{
    int action[GLFW_KEY_LAST];
    int mods[GLFW_KEY_LAST];
};

inline void GL_clear_errors() { while(glGetError()); }

inline bool GL_log_error(const char* func, const char* file, int line)
{
    if (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] " << "0x" << std::hex << error << ": " <<  func <<
            " " << file << ":" << line << std::endl;
        return true;
    }
    return false;
}

inline uint32_t size_of_gl_type(GLenum type)
{
    switch(type)
    {
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return 2;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
            return 4;
        default:
            std::cout << "OPEN_GL type not handled" << std::endl;
            exit(EXIT_FAILURE);
    }
}