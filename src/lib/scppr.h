#ifndef LIB_SCPPR_H
#define LIB_SCPPR_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace scppr
{

  static int default_width = 800;
  static int default_height = 800;

  class texture_t
  {
  public:
    texture_t(std::string path);
    ~texture_t();
    // do not fiddle with this
    GLuint t_id;
  };

  class rectangle_t
  {
  public:
    rectangle_t();
    ~rectangle_t();
    glm::dvec3 position;
    glm::dvec3 rotation;
    glm::dvec2 scale;
    texture_t *texture;
    // do not fiddle with these
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
  };

  class scppr
  {
  public:
    scppr(std::string name);
    ~scppr();
    void add_rectangle(rectangle_t *rectangle);
    bool is_open();
    void poll();
    void draw();
    void set_camera(double fov, glm::dvec3 eye, glm::dvec3 point);
    GLFWwindow *window;
  private:
    int height = default_width;
    int width = default_height;
    GLuint program;
    double camera_fov;
    glm::dvec3 camera_eye;
    glm::dvec3 camera_point;
    std::vector<rectangle_t *> rectangles;
  };
}

#endif // LIB_SCPPR_H
