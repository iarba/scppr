#ifndef LIB_SCPPR_H
#define LIB_SCPPR_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <set>
#include <map>
#include <vector>

namespace scppr
{

  // flags
  static const uint32_t SCPPR_CAMERA_FOV = 1;
  static const uint32_t SCPPR_CAMERA_EYE = 2;
  static const uint32_t SCPPR_CAMERA_PITCH = 4;
  static const uint32_t SCPPR_CAMERA_ROLL = 8;
  static const uint32_t SCPPR_CAMERA_YAW = 16;
  extern std::string _assets_path;

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

  struct vertex_t
  {
    glm::vec3 position;
    glm::vec2 texture_coord;
    glm::vec3 normal;
  };

  class texture_t
  {
  public:
    texture_t(std::string path);
    ~texture_t();
    // do not fiddle with this
    GLuint t_id;
  };

  class material_t
  {
  public:
    texture_t *diffuse = NULL;
    texture_t *specular = NULL;
  };

  class mesh_t
  {
    public:
    mesh_t(std::vector<vertex_t> vertices, std::vector<GLuint> indices);
    ~mesh_t();
    material_t material;
    // do not fiddle with this
    std::vector<vertex_t> vertices;
    std::vector<GLuint> indices;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
  };

  class model_t
  {
  public:
    model_t(std::string path);
    ~model_t();
    // do not fiddle with this
    std::vector<mesh_t *> meshes;
    std::vector<material_t> materials;
  };

  class object_t
  {
  public:
    object_t();
    ~object_t();
    glm::dvec3 position = {0, 0, 0};
    glm::dvec3 rotation = {0, 0, 0};
    glm::dvec3 scale = {1, 1, 1};
    bool hidden = false;
    bool active = true;
    model_t *model = NULL;
    std::map<int, material_t> material_overwrite;
  };

  class light_t
  {
  public:
    light_t();
    ~light_t();
    glm::dvec3 position = {0, 0, 0};
    glm::dvec3 ambient = {0.2, 0.2, 0.2};
    glm::dvec3 color = {1, 1, 1}; // diffuse
    glm::dvec3 specular = {1, 1, 1};
    double strength = 1000000;
    bool hidden = true;
    bool active = true;
  };

  class scppr
  {
  public:
    scppr(std::string name, std::string assets_path);
    ~scppr();
    void add_object(object_t *obj);
    void remove_object(object_t *obj);
    void add_light(light_t *light);
    void remove_light(light_t *light);
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
    GLuint simple_light_program;
    GLuint light_program;
    double camera_fov;
    glm::dvec3 camera_eye;
    glm::dvec3 camera_point;
    double camera_pitch;
    double camera_roll;
    double camera_yaw;
    glm::dvec3 camera_front;
    glm::dvec3 camera_right;
    glm::dvec3 camera_up;
    std::set<object_t *> objects;
    std::set<light_t *> lights;
    std::map<listener_t, void *> listeners;
    material_t default_material;
    light_t *default_ambient;
  };
}

#endif // LIB_SCPPR_H
