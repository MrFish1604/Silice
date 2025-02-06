// @sylefeb 2022
// MIT license, see LICENSE_MIT in Silice repo root
// https://github.com/sylefeb/Silice/

#pragma once

#define DISPLAY_WIDTH 128
#define DISPLAY_PIXELS 16384

void display_clear();
void display_set_cursor(int x,int y);
void display_set_front_back_color(unsigned char f,unsigned char b);
void display_putchar(int c);
void display_refresh();
void dual_putchar(int c);
volatile unsigned char *display_framebuffer();
