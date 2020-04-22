#include "lib/scppr.h"
#include "lib/shader/shader.h"
#include "lib/log.h"
#include <glm/gtc/matrix_transform.hpp>
#include "lib/texture/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool scppr_initialised = false;

GLint glGetUniformLocation_str(GLint program, std::string location)
{
  return glGetUniformLocation(program, location.c_str());
}

void scppr_error_callback(int error, const char* description)
{
  scppr_LOG(std::string(description));
}

scppr::texture_t::texture_t(std::string path)
{
  scppr_ASSERT(scppr_initialised, "scppr is not initialised");
  int width, height, channels;
  scppr_LOG("attempting to load texture");
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  GLenum format;
  if(channels == 1)
  {
    format = GL_RED;
  }
  if(channels == 3)
  {
    format = GL_RGB;
  }
  if(channels == 4)
  {
    format = GL_RGBA;
  }
  scppr_ASSERT(data, "failed to load texture [" + path + "]");
  scppr_LOG("creating texture buffer");
  glGenTextures(1, &t_id);
  glBindTexture(GL_TEXTURE_2D, t_id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_image_free(data);
}

scppr::texture_t::~texture_t()
{
  // TODO: free texture
  glDeleteBuffers(1, &t_id);
}

scppr::model_t::model_t(std::string path)
{
  scppr_LOG("importing model [" + path + "]");
  Assimp::Importer _importer;
  const aiScene *_scene = _importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_OptimizeMeshes);
  scppr_LOG("checking import");
  scppr_ASSERT(_scene, "assimp failed to load model: " + std::string(_importer.GetErrorString()));

  scppr_LOG("creating model buffers");
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  scppr_LOG("populating buffer with model");
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t), &vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW); 

  scppr_LOG("defining buffer structure for model");
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)(3 * sizeof(float)));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)(5 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  scppr_LOG("unbinding buffer");
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

scppr::model_t::~model_t()
{
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

scppr::object_t::object_t()
{
}

scppr::object_t::~object_t()
{
}

scppr::light_t::light_t()
{
}

scppr::light_t::~light_t()
{
}

scppr::scppr::scppr(std::string name)
{
  scppr_ASSERT(!scppr_initialised, "scppr is not initialised");
  scppr_LOG("creating glfw context");
  scppr_ASSERT(glfwInit(), "failed to initialise glfw context");
  glfwSetErrorCallback(scppr_error_callback);

  scppr_LOG("creating glfw window");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(default_width, default_height, name.c_str(), NULL, NULL);
  scppr_ASSERT(window, "failed to create glfw window");
  glfwMakeContextCurrent(window);

  scppr_LOG("attaching listeners");
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_wrap);
  glfwSetCursorPosCallback(window, mouse_callback_wrap);
  glfwSetScrollCallback(window, scroll_callback_wrap);
  glfwSetMouseButtonCallback(window, click_callback_wrap);
  glfwSetKeyCallback(window, keyboard_callback_wrap);

  scppr_LOG("extracting gl context");
  scppr_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "failed to load glad");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scppr_LOG("configuring gl context");
  glfwSwapInterval(1);

  scppr_LOG("creating gl render program");
  simple_light_program = load_program("simple_light");

  scppr_LOG("initialising camera");
  set_camera(M_PI / 2, { 0.0, 3.0, 0.0},  -M_PI / 2, 0.0, 0.0, SCPPR_CAMERA_FOV | SCPPR_CAMERA_EYE | SCPPR_CAMERA_PITCH | SCPPR_CAMERA_ROLL | SCPPR_CAMERA_YAW);

  scppr_initialised = true;

  scppr_LOG("initialising environment");
  default_texture = new texture_t("../assets/no_texture.png");
  default_specular_texture = new texture_t("../assets/black.jpg");
  default_ambient = new light_t();
  default_ambient -> ambient = {0.15, 0.15, 0.15};
  default_ambient -> color = {0, 0, 0};
  default_ambient -> specular = {0, 0, 0};
  default_ambient -> hidden = true;
  add_light(default_ambient);
  scppr_LOG("scppr initialised successfully");
}

scppr::scppr::~scppr()
{
  delete default_texture;
  delete default_specular_texture;
  delete default_ambient;
  scppr_initialised = false;
  glfwDestroyWindow(window);
  glfwTerminate();
}

void scppr::scppr::add_object(object_t *obj)
{
  objects.insert(obj);
}

void scppr::scppr::remove_object(object_t *obj)
{
  objects.erase(obj);
}

void scppr::scppr::add_light(light_t *light)
{
  lights.insert(light);
}

void scppr::scppr::remove_light(light_t *light)
{
  lights.erase(light);
}

void scppr::scppr::add_listener(listener_t cbt, void *function)
{
  listeners[cbt] = function;
}

void scppr::scppr::remove_listener(listener_t cbt)
{
  listeners.erase(cbt);
}

bool scppr::scppr::is_open()
{
  return !glfwWindowShouldClose(window);
}

void scppr::scppr::poll()
{
  glfwPollEvents();
}

void scppr::scppr::draw()
{
  scppr_LOG("resetting camera for new frame");
  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glm::dmat4 projection = glm::perspective(camera_fov, (double)width / (double)height, 0.1, 100.0);
  glm::dmat4 view = glm::lookAt(camera_eye, (camera_eye + camera_front), camera_up);
  view = glm::rotate(view, camera_roll, camera_front);
  glm::dmat4 vp = projection * view;

  scppr_LOG("running programs");
  GLuint program;

  glUseProgram(simple_light_program);
  program = simple_light_program;

  for(auto obj : objects)
  {
    if(obj -> hidden)
    {
      continue;
    }
    glBindVertexArray(obj -> model -> vao);

    glm::dmat4 model = glm::dmat4(1);
               model = glm::translate(model, glm::dvec3(obj -> position));
               model = glm::rotate(model, obj -> rotation.x, {1, 0, 0});
               model = glm::rotate(model, obj -> rotation.y, {0, 1, 0});
               model = glm::rotate(model, obj -> rotation.z, {0, 0, 1});
               model = glm::scale(model, obj -> scale);

    glm::mat4 f_m = model;
    glUniformMatrix4fv(glGetUniformLocation(program, "m"), 1, GL_FALSE, &f_m[0][0]);
    glm::mat4 f_v = view;
    glUniformMatrix4fv(glGetUniformLocation(program, "v"), 1, GL_FALSE, &f_v[0][0]);
    glm::mat4 f_p = projection;
    glUniformMatrix4fv(glGetUniformLocation(program, "p"), 1, GL_FALSE, &f_p[0][0]);
    glm::mat3 f_nmv = glm::mat3(glm::transpose(glm::inverse(view * model)));
    glUniformMatrix3fv(glGetUniformLocation(program, "nmv"), 1, GL_FALSE, &f_nmv[0][0]);
    int count = 0;
    for(light_t *light : lights)
    {
      if(!light -> active)
      {
        continue;
      }
      glm::vec3 f_lp = view * glm::vec4(light -> position, 1);
      std::string header = "lights[" + std::to_string(count) + "]";
      glUniform3fv(glGetUniformLocation_str(program, header + ".position"), 1, &f_lp[0]);
      glm::vec3 f_la = light -> ambient;
      glUniform3fv(glGetUniformLocation_str(program, header + ".ambient"), 1, &f_la[0]);
      glm::vec3 f_lc = light -> color;
      glUniform3fv(glGetUniformLocation_str(program, header + ".diffuse"), 1, &f_lc[0]);
      glm::vec3 f_ls = light -> specular;
      glUniform3fv(glGetUniformLocation_str(program, header + ".specular"), 1, &f_ls[0]);
      glUniform1f(glGetUniformLocation_str(program, header + ".strength"), light -> strength);
      count++;
    }
    glUniform1i(glGetUniformLocation(program, "light_no"), count);
    glDrawElements(GL_TRIANGLES, obj -> model -> indices.size(), GL_UNSIGNED_INT, 0);

    GLuint t_id = default_texture -> t_id;
    glUniform1i(glGetUniformLocation(program, "material.diffuse"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_id);

    t_id = default_specular_texture -> t_id;
    glUniform1i(glGetUniformLocation(program, "material.specular"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, t_id);

    glUniform1f(glGetUniformLocation(program, "material.shininess"), 32);
  }
  glBindVertexArray(0);

  glfwSwapBuffers(window);
}

void scppr::scppr::set_camera(double fov, glm::dvec3 eye, double pitch, double roll, double yaw, uint32_t flags)
{
  if(flags & SCPPR_CAMERA_FOV)
  {
    this -> camera_fov = fov;
  }
  if(flags & SCPPR_CAMERA_EYE)
  {
    this -> camera_eye = eye;
  }
  if(flags & SCPPR_CAMERA_PITCH)
  {
    this -> camera_pitch = pitch;
  }
  if(flags & SCPPR_CAMERA_ROLL)
  {
    this -> camera_roll = roll;
  }
  if(flags & SCPPR_CAMERA_YAW)
  {
    this -> camera_yaw = yaw;
  }
  camera_front.x = cos(camera_yaw) * cos(camera_pitch);
  camera_front.y = sin(camera_pitch);
  camera_front.z = sin(camera_yaw) * cos(camera_pitch);
  camera_front = glm::normalize(camera_front);
  camera_right = glm::normalize(glm::cross(camera_front, {0, 1, 0}));
  camera_up    = glm::normalize(glm::cross(camera_right, camera_front));
}

void scppr::scppr::framebuffer_size_callback_wrap(GLFWwindow* window, int width, int height)
{
  scppr *that = (scppr *)glfwGetWindowUserPointer(window);
  that -> framebuffer_size_callback(window, width, height);
}

void scppr::scppr::mouse_callback_wrap(GLFWwindow* window, double xpos, double ypos)
{
  scppr *that = (scppr *)glfwGetWindowUserPointer(window);
  that -> mouse_callback(window, xpos, ypos);
}

void scppr::scppr::scroll_callback_wrap(GLFWwindow* window, double xoffset, double yoffset)
{
  scppr *that = (scppr *)glfwGetWindowUserPointer(window);
  that -> scroll_callback(window, xoffset, yoffset);
}

void scppr::scppr::click_callback_wrap(GLFWwindow* window, int button, int state, int mods)
{
  scppr *that = (scppr *)glfwGetWindowUserPointer(window);
  that -> click_callback(window, button, state, mods);
}

void scppr::scppr::keyboard_callback_wrap(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  scppr *that = (scppr *)glfwGetWindowUserPointer(window);
  that -> keyboard_callback(window, key, scancode, action, mods);
}

void scppr::scppr::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void scppr::scppr::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  auto it = listeners.find(mouse_listener);
  if(it != listeners.end())
  {
    void (*cb)(GLFWwindow *, double, double) = (void (*)(GLFWwindow *, double, double))it -> second;
    (*cb)(window, xpos, ypos);
  }
}

void scppr::scppr::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  auto it = listeners.find(scroll_listener);
  if(it != listeners.end())
  {
    void (*cb)(GLFWwindow *, double, double) = (void (*)(GLFWwindow *, double, double))it -> second;
    (*cb)(window, xoffset, yoffset);
  }
}

void scppr::scppr::click_callback(GLFWwindow* window, int button, int state, int mods)
{
  auto it = listeners.find(click_listener);
  if(it != listeners.end())
  {
    void (*cb)(GLFWwindow *, int, int, int) = (void (*)(GLFWwindow *, int, int, int))it -> second;
    (*cb)(window, button, state, mods);
  }
}

void scppr::scppr::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  auto it = listeners.find(keyboard_listener);
  if(it != listeners.end())
  {
    void (*cb)(GLFWwindow *, int, int, int, int) = (void (*)(GLFWwindow *, int, int, int, int))it -> second;
    (*cb)(window, key, scancode, action, mods);
  }
}
