#include "lib/scppr.h"
#include "lib/shader/shader.h"
#include "lib/log.h"
#include <glm/gtc/matrix_transform.hpp>
#include "lib/texture/stb_image.h"

bool scppr_initialised = false;

/*        y
 *
 *        A
 *        |
 *     4      1
 *      *---*
 *      |  /|
 *-x <- | 0 | -> x
 *      |/  |
 *      *---*
 *     3      2 ->x
 *        |
 *        V
 *       -y
 *
 */
static const float square_vertices[] = 
{
   0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 1
   0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 2
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 3
  -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // 4
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
  scppr_ASSERT(scppr_initialised, "scppr is not initialised");
  scppr_LOG("creating rectangle buffers");
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  scppr_LOG("populating buffers");
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW); 
  scppr_LOG("defining buffer structure");
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

scppr::rectangle_t::~rectangle_t()
{
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
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

  scppr_LOG("extracting gl context");
  scppr_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "failed to load glad");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scppr_LOG("configuring gl context");
  glfwSwapInterval(1);

  scppr_LOG("creating gl render program");
  program = load_program();

  scppr_LOG("initialising camera");
  set_camera(M_PI / 2, {0, 0, 5}, {0, 0, 0});

  scppr_initialised = true;
  scppr_LOG("scppr initialised successfully");
}

scppr::scppr::~scppr()
{
  scppr_initialised = false;
  glfwDestroyWindow(window);
  glfwTerminate();
}

void scppr::scppr::add_rectangle(rectangle_t *rectangle)
{
  rectangles.push_back(rectangle);
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
  for(auto rectangle : rectangles)
  {
    glBindVertexArray(rectangle -> vao);
    glUniform1i(glGetUniformLocation(program, "tex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rectangle -> texture -> t_id);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
  glfwSwapBuffers(window);
  glfwPollEvents();
}
void scppr::scppr::set_camera(double fov, glm::dvec3 eye, glm::dvec3 point)
{
  this -> camera_fov = fov;
  this -> camera_eye = eye;
  this -> camera_point = point;
}
