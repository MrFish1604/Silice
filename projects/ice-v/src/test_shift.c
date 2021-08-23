#include "config.h"

inline unsigned int cpu_id() 
{
   unsigned int cycles;
   asm volatile ("rdcycle %0" : "=r"(cycles));
   return cycles&1;
}

void main() 
{

  volatile int i;
  if (cpu_id())  {
    for (i = 0; i < 5 ; i+=1) {    
      *LEDS = 1<<i;
    }
  } else {
    for (i = 0; i < 5 ; i+=1) {    
      *LEDS = 16>>i;
    }
  }

  while (1) { }

}