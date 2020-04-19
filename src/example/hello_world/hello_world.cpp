#include "scppr.h"

int main()
{
  scppr::scppr renderer("Hello world");
  scppr::texture_t *texture = new scppr::texture_t("../assets/wall.jpg");
  scppr::rectangle_t *rectangle = new scppr::rectangle_t();
  rectangle -> texture = texture;
  renderer.add_rectangle(rectangle);
  while(renderer.is_open())
  {
    renderer.poll();
    renderer.draw();
  }
  return 0;
}
