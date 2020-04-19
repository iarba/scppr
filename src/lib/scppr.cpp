#include "lib/scppr.h"
#include "lib/shader/shader.h"
#include "lib/log.h"

static const GLfloat vertices[] = {
   -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

void scppr_error_callback(int error, const char* description)
{
  scppr_LOG(std::string(description));
}

scppr::texture_t::texture_t(std::string path)
{
  this -> path = path;
  // TODO: load texture
}

scppr::texture_t::~texture_t()
{
  // TODO: free texture
}

scppr::scppr::scppr(std::string name)
{
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
  glfwSwapInterval(1);

  // TODO: V
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  // TODO: A

  scppr_LOG("creating gl buffer");
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  scppr_LOG("creating gl render program");
  program = load_program();

  scppr_LOG("scppr initialised successfully");
}

scppr::scppr::~scppr()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}

bool scppr::scppr::is_open()
{
  return !glfwWindowShouldClose(window);
}

void scppr::scppr::draw()
{
  scppr_LOG("drawing");
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnableVertexAttribArray(0);

  scppr_LOG("loading vertices");
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glUseProgram(program);
  glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisableVertexAttribArray(0);

  glfwSwapBuffers(window);
  glfwPollEvents();
}

void scppr::scppr::poll()
{
  glfwPollEvents();
}
