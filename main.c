#include "knob.h"

int main(int argc, char **argv)
{
  if(get_knob())
  {
    show_knob_endpoints();
    drive_knob();
    close_knob();
  }

  return 0;
}
