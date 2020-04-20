#include "scppr.h"
#include "example/02/camera.h"

double last_x = 0;
double last_y = 0;
double last_exists = false;

camera_t camera;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  camera.process_scroll(yoffset);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  if(glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
  {
    return;
  }
  if(!last_exists)
  {
    last_exists = true;
    last_x = xpos;
    last_y = ypos;
    return;
  }
  camera.process_mouse(xpos - last_x, ypos - last_y);
  last_x = xpos;
  last_y = ypos;
}

void click_callback(GLFWwindow *window, int button, int state, int mods)
{
  if(button == GLFW_MOUSE_BUTTON_2)
  {
    if(state == GLFW_PRESS)
    {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if(state == GLFW_RELEASE)
    {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      last_exists = false;
    }
  }
}

void kb_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if(action == GLFW_PRESS)
  {
    if(key == GLFW_KEY_W)
    {
      camera.df += 1;
    }
    if(key == GLFW_KEY_A)
    {
      camera.dr -= 1;
    }
    if(key == GLFW_KEY_S)
    {
      camera.df -= 1;
    }
    if(key == GLFW_KEY_D)
    {
      camera.dr += 1;
    }
  }
  if(action == GLFW_RELEASE)
  {
    if(key == GLFW_KEY_W)
    {
      camera.df -= 1;
    }
    if(key == GLFW_KEY_A)
    {
      camera.dr += 1;
    }
    if(key == GLFW_KEY_S)
    {
      camera.df += 1;
    }
    if(key == GLFW_KEY_D)
    {
      camera.dr -= 1;
    }
  }
}

int main()
{
  scppr::scppr renderer("Scene explorer");
  scppr::texture_t *texture = new scppr::texture_t("../assets/wall.jpg");
  scppr::rectangle_t *rectangle = new scppr::rectangle_t();
                      rectangle -> texture = texture;
                      rectangle -> rotation.x = glm::radians(90.0);
  scppr::cube_t *cube1 = new scppr::cube_t();
                 cube1 -> texture = texture;
                 cube1 -> position.x = -5;
  scppr::cube_t *cube2 = new scppr::cube_t();
                 cube2 -> texture = texture;
                 cube2 -> position.x = 5;
  scppr::light_t *light1 = new scppr::light_t();
                  light1 -> position = {0, 10, -5};
                  light1 -> color = {0, 1, 0};
  scppr::light_t *light2 = new scppr::light_t();
                  light2 -> position = {0, -10, 5};
  renderer.add_rectangle(rectangle);
  renderer.add_cube(cube1);
  renderer.add_cube(cube2);
  renderer.add_light(light1);
  renderer.add_light(light2);
  renderer.add_listener(scppr::scroll_listener, (void *)&scroll_callback);
  renderer.add_listener(scppr::mouse_listener, (void *)&mouse_callback);
  renderer.add_listener(scppr::click_listener, (void *)&click_callback);
  renderer.add_listener(scppr::keyboard_listener, (void *)&kb_callback);
  while(renderer.is_open())
  {
    renderer.poll();
    if(camera.dirty_camera)
    {
      renderer.set_camera(glm::radians(camera.fov), camera.position, glm::radians(camera.pitch), glm::radians(camera.roll), glm::radians(camera.yaw), -1);
      camera.dirty_camera = false;
    }
    camera.tick();
    renderer.draw();
  }
  delete rectangle;
  delete cube1;
  delete cube2;
  delete light1;
  delete light2;
  delete texture;
  return 0;
}
