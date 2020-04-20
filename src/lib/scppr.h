#ifndef LIB_SCPPR_H
#define LIB_SCPPR_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace scppr
{

  // flags
  static const uint32_t SCPPR_CAMERA_FOV = 1;
  static const uint32_t SCPPR_CAMERA_EYE = 2;
  static const uint32_t SCPPR_CAMERA_PITCH = 4;
  static const uint32_t SCPPR_CAMERA_ROLL = 8;
  static const uint32_t SCPPR_CAMERA_YAW = 16;

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
    glm::dvec3 position = {0, 0, 0};
    glm::dvec3 rotation = {0, 0, 0};
    glm::dvec2 scale = {1, 1};
    texture_t *texture;
    // do not fiddle with these
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
    void set_camera(double fov, glm::dvec3 eye, double pitch, double roll, double yaw, uint32_t flags);
    GLFWwindow *window;
  private:
    int height = default_width;
    int width = default_height;
    GLuint program;
    double camera_fov;
    glm::dvec3 camera_eye;
    glm::dvec3 camera_point;
    double camera_pitch;
    double camera_roll;
    double camera_yaw;
    glm::dvec3 camera_front;
    glm::dvec3 camera_right;
    glm::dvec3 camera_up;
    GLuint rectangle_vao;
    GLuint rectangle_vbo;
    GLuint rectangle_ebo;
    std::vector<rectangle_t *> rectangles;
  };
}

#endif // LIB_SCPPR_H
