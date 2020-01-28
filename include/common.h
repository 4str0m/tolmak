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

#define GLCall(x)\
	do {\
		GL_clear_errors();\
		x;\
		if (GL_log_error(#x, __FILE__, __LINE__)) exit(EXIT_FAILURE);\
	} while(0)

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