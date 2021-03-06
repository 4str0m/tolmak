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
#include <unordered_map>

#include <utils/log.h>
#include <utils/array.h>
#include <utils/bucket_allocator.h>

#define PI 3.141592654f
#define HALF_PI 1.570796327f
#define QUARTER_PI 0.785398163f
#define TWO_PI 6.283185307f

#define MAX_PATH_LENGTH 256

#define GLCall(x)\
    do {\
        GL_clear_errors();\
        x;\
        GLenum error = glGetError();\
        ASSERT(!error, "OpenGL error 0x%04x.", error);\
    } while(0)

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
extern MouseState mouse_state;
extern KeyboardState keyboard_state;

inline void GL_clear_errors() { while(glGetError()); }

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
            LOG(ERROR, "OpenGL type not handled.");
            return 0;
    }
}