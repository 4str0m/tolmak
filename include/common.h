#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <string>
#include <vector>
#include <unordered_map>

#define PI 3.141592654f
#define HALF_PI 1.570796327f
#define QUARTER_PI 0.785398163f
#define TWO_PI 6.283185307f

#define GLCall(x)\
	do {\
		GL_clear_errors();\
		x;\
		if (GL_log_error(#x, __FILE__, __LINE__)) exit(EXIT_FAILURE);\
	} while(0)

struct MouseState
{
	float x, y;
	float dx, dy;
	bool buttons[3];
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