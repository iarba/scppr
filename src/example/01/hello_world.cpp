#include "scppr.h"

double fov = 45;
bool dirty_camera = true;

void process_mouse_scroll(GLFWwindow *_w, double _xoffset, double yoffset)
{
  fov -= yoffset;
  if(fov < 3.0)
  {
    fov = 3.0;
  }
  if(fov > 120)
  {
    fov = 120;
  }
  dirty_camera = true;
}

int main()
{
  scppr::scppr renderer("Hello world");
  scppr::model_t *cube = new scppr::model_t("../assets/cube.obj");
  scppr::object_t *obj = new scppr::object_t();
  obj -> model = cube;
  renderer.add_object(obj);
  renderer.add_listener(scppr::scroll_listener, (void *)&process_mouse_scroll);
  while(renderer.is_open())
  {
    renderer.poll();
    if(dirty_camera)
    {
      renderer.set_camera(glm::radians(fov), {0, 0, 0}, 0, 0, 0, scppr::SCPPR_CAMERA_FOV);
      dirty_camera = false;
    }
    renderer.draw();
  }
  delete obj;
  delete cube;
  return 0;
}
