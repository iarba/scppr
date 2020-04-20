#ifndef SCPPR_LIB_SHADER_SHADER_H
#define SCPPR_LIB_SHADER_SHADER_H

#include "lib/glad.h"
#include <string>

GLuint load_shader(GLenum shader_type, std::string path);

GLuint load_program(std::string choice);

#endif // SCPPR_LIB_SHADER_SHADER_H
