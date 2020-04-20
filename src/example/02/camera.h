#ifndef EXAMPLE_02_CAMERA_H
#define EXAMPLE_02_CAMERA_H

#include <glm/glm.hpp>

// here we use euler angles

class camera_t
{
public:
  camera_t();
  void tick();
  double fov = 45;
  glm::dvec3 position = {0, 0, 3};
  double pitch = 0;
  double roll = 0;
  double yaw = -90;
  double df = 0;
  double dr = 0;
  bool dirty_camera = true;
  void process_mouse(double xoffset, double yoffset);
  void process_scroll(double offset);
private:
  void recompute_vectors();
  glm::dvec3 front;
  glm::dvec3 up;
  glm::dvec3 right;
};

#endif // EXAMPLE_02_CAMERA_H
