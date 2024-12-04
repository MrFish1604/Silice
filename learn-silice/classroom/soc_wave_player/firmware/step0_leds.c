// @sylefeb 2022-01-10
// MIT license, see LICENSE_MIT in Silice repo root
// https://github.com/sylefeb/Silice/

// Once Step 0 completed, this firmware produces a back
// and forth "knight rider" pattern on the LEDs

#include "config.h"
#include "std.h"

void set_led(int value, int l){
  *LEDS = (value<<8) | l;
}

void main()
{
  int led  = 0;
  int dir  = 0;
  int intensity = 0;
  while (1) {
    pause(1000000);
    if (intensity == 0 || intensity == 255) {
      dir = 1-dir;
    }
    if (dir) {
      ++ intensity;
    } else {
      -- intensity;
    }
    for(int i=0; i<8; i++)
      set_led(intensity,i);
  }
}
