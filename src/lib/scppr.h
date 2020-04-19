#ifndef LIB_SCPPR_H
#define LIB_SCPPR_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

namespace scppr
{

  static int default_width = 800;
  static int default_height = 600;

  class texture_t
  {
  public:
    texture_t(std::string path);
    ~texture_t();
    std::string path;
  };

  class rectangle_t
  {
  public:
    glm::dvec3 position;
    glm::dvec3 rotation;
    glm::dvec2 scale;
    texture_t *t;
  };

  class scppr
  {
  public:
    scppr(std::string name);
    ~scppr();
    bool is_open();
    void poll();
    void draw();
    GLFWwindow *window;
  private:
    GLuint program;
    GLuint vertex_buffer;
    GLint mvp_location;
  };
}

#endif // LIB_SCPPR_H
