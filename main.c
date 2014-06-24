#include "knob.h"
#include <stdio.h>

int main(int argc, char **argv)
{
  if(get_knob())
  {
    show_knob_endpoints();
    drive_knob();
    while(knob_is_waiting())
    {
    	knob_handle_events();
    }
    getc(stdin);
    close_knob();
  }

  return 0;
}
