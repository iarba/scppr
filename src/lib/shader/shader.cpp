#include "lib/shader/shader.h"
#include "lib/log.h"
#include <fstream>
#include <iostream>
#include <sstream>

GLuint load_shader(GLenum shader_type, std::string path)
{
  scppr_LOG("creating shader from [" + path + "]");
  GLuint shader = glCreateShader(shader_type);
  scppr_LOG("reading shader code");
  std::ifstream file(path);
  scppr_ASSERT(file.is_open(), "Failed to open file " + path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string shader_text = buffer.str();
  const char *shader_text_c_str = shader_text.c_str();
  scppr_LOG("compiling shader");
  glShaderSource(shader, 1, &shader_text_c_str, NULL);
  glCompileShader(shader);
  GLint result = GL_FALSE;
  int info_length;
  scppr_LOG("checking shader correctness");
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
  if(info_length > 0)
  {
    char *s = new char[1024];
    glGetShaderInfoLog(shader, info_length, NULL, &s[0]);
    scppr_LOG("encountered shader error - " + std::string(s));
    delete[] s;
  }
  return shader;
}

GLuint load_program()
{
  scppr_LOG("creating program");
  GLuint program = glCreateProgram();
  scppr_LOG("creating vertex shader");
  GLuint v_shader = load_shader(GL_VERTEX_SHADER, "../src/lib/shader/simple.vertex_shader.c_");
  scppr_LOG("creating fragment shader");
  GLuint f_shader = load_shader(GL_FRAGMENT_SHADER, "../src/lib/shader/simple.fragment_shader.c_");
  scppr_LOG("attaching shaders");
  glAttachShader(program, v_shader);
  glAttachShader(program, f_shader);
  scppr_LOG("linking program");
  glLinkProgram(program);
  GLint result = GL_FALSE;
  int info_length;
  scppr_LOG("checking program");
  glGetProgramiv(program, GL_LINK_STATUS, &result);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_length);
  if(info_length > 0)
  {
    char *s = new char[1024];
    glGetProgramInfoLog(program, info_length, NULL, &s[0]);
    scppr_LOG("encountered linker error - " + std::string(s));
    delete[] s;
  }

  scppr_LOG("deleting shaders");
  glDetachShader(program, v_shader);
  glDetachShader(program, f_shader);
  glDeleteShader(v_shader);
  glDeleteShader(f_shader);
  return program;
}
