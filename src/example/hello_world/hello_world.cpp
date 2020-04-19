#include "scppr.h"

int main()
{
  scppr::scppr r("Hello world");
  while(r.is_open())
  {
    r.poll();
    r.draw();
  }
  return 0;
}
