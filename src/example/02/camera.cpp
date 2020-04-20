#include "example/02/camera.h"

camera_t::camera_t()
{
  recompute_vectors();
}

void camera_t::tick()
{
  position += front * 0.1 * df;
  position += right * 0.1 * dr;
  dirty_camera = true;
}

void camera_t::process_mouse(double xoffset, double yoffset)
{
  yaw   += xoffset * 0.1;
  pitch -= yoffset * 0.1;
  recompute_vectors();
  dirty_camera = true;
}

void camera_t::process_scroll(double offset)
{
  fov -= offset;
  if(fov < 10)
  {
    fov = 10;
  }
  if(fov > 120)
  {
    fov = 120;
  }
  dirty_camera = true;
}

// this is basically what camera does under the hood
void camera_t::recompute_vectors()
{
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(front);
  right = glm::normalize(glm::cross(front, {0, 1, 0}));
  up = glm::normalize(glm::cross(right, front));
}
