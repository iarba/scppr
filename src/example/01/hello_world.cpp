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

int main(int argc, char **argv)
{
  std::string path = std::string(argv[0]);
  std::string directory = path.substr(0, path.find_last_of('/')) + "/../scppr/assets/";
  scppr::scppr *renderer = new scppr::scppr("Hello world", directory);
  scppr::model_t *cube = new scppr::model_t(directory + "cube.obj");
  scppr::object_t *obj = new scppr::object_t();
  obj -> model = cube;
  renderer -> add_object(obj);
  renderer -> add_listener(scppr::scroll_listener, (void *)&process_mouse_scroll);
  while(renderer -> is_open())
  {
    renderer -> poll();
    if(dirty_camera)
    {
      renderer -> set_camera(glm::radians(fov), {0, 0, 0}, 0, 0, 0, scppr::SCPPR_CAMERA_FOV);
      dirty_camera = false;
    }
    renderer -> draw();
  }
  delete obj;
  delete cube;
  return 0;
}
