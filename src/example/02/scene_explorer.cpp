#include "scppr.h"
#include "example/02/camera.h"

double last_x = 0;
double last_y = 0;
double last_exists = false;

camera_t camera;

void scroll_callback(void *p, double xoffset, double yoffset)
{
  camera.process_scroll(yoffset);
}

void mouse_callback(void *p, double xpos, double ypos)
{
  GLFWwindow *window = (GLFWwindow *)p;
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

void click_callback(void *p, int button, int state, int mods)
{
  GLFWwindow *window = (GLFWwindow *)p;
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

void kb_callback(void *p, int key, int scancode, int action, int mods)
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

int main(int argc, char **argv)
{
  std::string path = std::string(argv[0]);
  std::string directory = path.substr(0, path.find_last_of('/')) + "/../scppr/assets/";
  scppr::scppr renderer("Scene explorer", directory);
  scppr::model_t* cube = new scppr::model_t(directory + "cube.obj");
  scppr::material_t mat;
  mat.diffuse = new scppr::texture_t(directory + "container2.png");
  mat.specular = new scppr::texture_t(directory + "container2_specular.png");
  scppr::object_t *cube1 = new scppr::object_t();
                   cube1 -> model = cube;
                   cube1 -> material_overwrite[0] = mat;
  scppr::material_t mat2 = mat;
  mat2.diffuse = NULL;
  scppr::object_t *cube2 = new scppr::object_t();
                   cube2 -> model = cube;
                   cube2 -> position.x = -5;
                   cube2 -> material_overwrite[0] = mat2;
  mat2 = mat;
  mat2.specular = NULL;
  scppr::object_t *cube3 = new scppr::object_t();
                   cube3 -> model = cube;
                   cube3 -> position.x = 5;
                   cube3 -> material_overwrite[0] = mat2;
  scppr::light_t *light1 = new scppr::light_t();
                  light1 -> position = {0, 10, -5};
                  light1 -> color = {0, 1, 0};
  scppr::light_t *light2 = new scppr::light_t();
                  light2 -> position = {0, -10, 5};
  scppr::material_t mat3;
  mat3.diffuse = new scppr::texture_t(directory + "thonk.png");
  scppr::object_t *cube4 = new scppr::object_t();
                   cube4 -> model = cube;
                   cube4 -> position.y = 5;
                   cube4 -> material_overwrite[0] = mat3;
  renderer.add_object(cube1);
  renderer.add_object(cube2);
  renderer.add_object(cube3);
  renderer.add_object(cube4);
  renderer.add_light(light1);
  renderer.add_light(light2);
  renderer.add_listener(scppr::scroll_listener, (void *)&scroll_callback, (void *)renderer.window);
  renderer.add_listener(scppr::mouse_listener, (void *)&mouse_callback, (void *)renderer.window);
  renderer.add_listener(scppr::click_listener, (void *)&click_callback, (void *)renderer.window);
  renderer.add_listener(scppr::keyboard_listener, (void *)&kb_callback, (void *)renderer.window);
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
  delete cube1;
  delete cube2;
  delete cube3;
  delete light1;
  delete light2;
  delete mat.diffuse;
  delete mat.specular;
  delete mat3.diffuse;
  delete cube;
  return 0;
}
