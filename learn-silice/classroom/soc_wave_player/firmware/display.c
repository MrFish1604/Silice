
#include "tama_mini02_font.h"

int cursor_x;
int cursor_y;

unsigned char front_color;
unsigned char back_color;

unsigned char framebuffer[128*128];

static inline void display_set_cursor(int x,int y)
{
  cursor_x = x;
  cursor_y = y;
}

static inline void display_set_front_back_color(unsigned char f,unsigned char b)
{
  front_color = f;
  back_color = b;
}

void display_putchar(int c)
{
  if (c == 10) {
    // next line
    cursor_x = 0;
    cursor_y += 8;
    if (cursor_y >= 128) {
      cursor_y = 0;
    }
    return;
  }
  if (c >= 32) {
    for (int j=0;j<8;j++) {
      for (int i=0;i<5;i++) {
        framebuffer[ 127 - (cursor_y + j) + ((cursor_x+i)<<7) ]
            = (font[c-32][i] & (1<<j)) ? front_color : back_color;
      }
    }
  }
  cursor_x += 5;
  if (cursor_x > 640) {
    cursor_x = 0;
    cursor_y += 8;
    if (cursor_y > 400) {
      cursor_y = 0;
    }
  }
}

static inline void display_refresh()
{
  unsigned char *ptr = framebuffer;
  for (int i=0;i<128*128;i++) {
    unsigned char c = *ptr;
    ++ ptr;
    oled_pix(c,c,c);
    WAIT;
  }
}
