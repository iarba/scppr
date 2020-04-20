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
  scppr::texture_t *texture = new scppr::texture_t("../assets/wall.jpg");
  scppr::rectangle_t *rectangle = new scppr::rectangle_t();
  rectangle -> texture = texture;
  renderer.add_rectangle(rectangle);
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
  delete rectangle;
  delete texture;
  return 0;
}
