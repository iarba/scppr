#include "lib/scppr.h"
#include "lib/shader/shader.h"
#include "lib/log.h"
#include <glm/gtc/matrix_transform.hpp>
#include "lib/texture/stb_image.h"

bool scppr_initialised = false;

/*        x
 *
 *        A
 *        |
 *     1     0
 *      *---*
 *      |\  |
 *-z <- | 0 | -> z
 *      |  \|
 *      *---*
 *     2     3
 *        |
 *        V
 *       -x
 *
 */
static const float square_vertices[] = 
{
   0.5f,  0.0f,  0.5f, 1.0f, 1.0f, // 0
   0.5f,  0.0f, -0.5f, 1.0f, 0.0f, // 1
  -0.5f,  0.0f, -0.5f, 0.0f, 0.0f, // 2
  -0.5f,  0.0f,  0.5f, 0.0f, 1.0f  // 3
};

unsigned int square_indices[] = {
  0, 1, 3,
  1, 2, 3
};

void scppr_error_callback(int error, const char* description)
{
  scppr_LOG(std::string(description));
}

scppr::texture_t::texture_t(std::string path)
{
  scppr_ASSERT(scppr_initialised, "scppr is not initialised");
  int width, height, nrChannels;
  scppr_LOG("attempting to load texture");
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  scppr_ASSERT(data, "failed to load texture [" + path + "]");
  scppr_LOG("creating texture buffer");
  glGenTextures(1, &t_id);
  glBindTexture(GL_TEXTURE_2D, t_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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

scppr::rectangle_t::rectangle_t()
{
}

scppr::rectangle_t::~rectangle_t()
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

  scppr_LOG("creating rectangle buffers");
  glGenVertexArrays(1, &rectangle_vao);
  glGenBuffers(1, &rectangle_vbo);
  glGenBuffers(1, &rectangle_ebo);

  scppr_LOG("populating buffers");
  glBindVertexArray(rectangle_vao);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangle_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW); 

  scppr_LOG("defining buffer structure");
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  scppr_LOG("creating gl render program");
  program = load_program("simple");

  scppr_LOG("initialising camera");
  set_camera(M_PI / 2, { 0.0, 3.0, 0.0},  -M_PI / 2, 0.0, 0.0, SCPPR_CAMERA_FOV | SCPPR_CAMERA_EYE | SCPPR_CAMERA_PITCH | SCPPR_CAMERA_ROLL | SCPPR_CAMERA_YAW);

  scppr_initialised = true;
  scppr_LOG("scppr initialised successfully");
}

scppr::scppr::~scppr()
{
  glDeleteVertexArrays(1, &rectangle_vao);
  glDeleteBuffers(1, &rectangle_vbo);
  glDeleteBuffers(1, &rectangle_ebo);
  scppr_initialised = false;
  glfwDestroyWindow(window);
  glfwTerminate();
}

void scppr::scppr::add_rectangle(rectangle_t *rectangle)
{
  rectangles.insert(rectangle);
}

void scppr::scppr::remove_rectangle(rectangle_t *rectangle)
{
  rectangles.erase(rectangle);
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

  scppr_LOG("running program");
  glUseProgram(program);
  glm::dmat4 projection = glm::perspective(camera_fov, (double)width / (double)height, 0.1, 100.0);
  glm::dmat4 view = glm::lookAt(camera_eye, (camera_eye + camera_front), camera_up);
  view = glm::rotate(view, camera_roll, camera_front);
  glm::dmat4 vp = projection * view;
  for(auto rectangle : rectangles)
  {
    glm::dmat4 model = glm::dmat4(1);
              model = glm::translate(model, glm::dvec3(rectangle -> position));
              model = glm::rotate(model, rectangle -> rotation.x, {1, 0, 0});
              model = glm::rotate(model, rectangle -> rotation.y, {0, 1, 0});
              model = glm::rotate(model, rectangle -> rotation.z, {0, 0, 1});
              model = glm::scale(model, glm::dvec3(rectangle -> scale, 1));
    glm::mat4 mvp = vp * model;

    glBindVertexArray(rectangle_vao);
    glUniform1i(glGetUniformLocation(program, "tex"), 0);
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rectangle -> texture -> t_id);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
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
