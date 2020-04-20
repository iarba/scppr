#ifndef LIB_SCPPR_H
#define LIB_SCPPR_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <set>
#include <map>

namespace scppr
{

  // flags
  static const uint32_t SCPPR_CAMERA_FOV = 1;
  static const uint32_t SCPPR_CAMERA_EYE = 2;
  static const uint32_t SCPPR_CAMERA_PITCH = 4;
  static const uint32_t SCPPR_CAMERA_ROLL = 8;
  static const uint32_t SCPPR_CAMERA_YAW = 16;

  // enums
  enum listener_t
  {
    mouse_listener,
    scroll_listener,
    click_listener,
    keyboard_listener
  };

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
    void remove_rectangle(rectangle_t *rectangle);
    void add_listener(listener_t cbt, void *function);
    void remove_listener(listener_t cbt);
    bool is_open();
    void poll();
    void draw();
    void set_camera(double fov, glm::dvec3 eye, double pitch, double roll, double yaw, uint32_t flags);
    GLFWwindow *window;
  private:
    static void framebuffer_size_callback_wrap(GLFWwindow* window, int width, int height);
    static void mouse_callback_wrap(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback_wrap(GLFWwindow* window, double xoffset, double yoffset);
    static void click_callback_wrap(GLFWwindow* window, int button, int state, int mods);
    static void keyboard_callback_wrap(GLFWwindow* window, int key, int scancode, int action, int mods);
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void click_callback(GLFWwindow* window, int button, int state, int mods);
    void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
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
    std::set<rectangle_t *> rectangles;
    std::map<listener_t, void *> listeners;
  };
}

#endif // LIB_SCPPR_H
