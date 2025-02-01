#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

#include "my_utils.h"

#include "fat_io_lib/src/fat_filelib.h"

void clear_audio()
{
  // wait for a buffer swap (sync)
  int *addr = (int*)(*AUDIO);
  while (addr == (int*)(*AUDIO)) { }
  // go ahead
  for (int b=0 ; b<2 ; ++b) {
    // read directly in hardware buffer
    addr = (int*)(*AUDIO);
    // clear buffer
    memset(addr,0,512);
    // wait for buffer swap
    while (addr == (int*)(*AUDIO)) { }
  }
}

void set_led(int value, int l){
  *LEDS = (value<<8) | l;
}
#define LED_OFF(l) set_led(0,l)
#define LED_ON(l) set_led(255,l)
#define LEDS_OFF() set_led(0,0);set_led(0,1);set_led(0,2);set_led(0,3);set_led(0,4);set_led(0,5);set_led(0,6);set_led(0,7)
#define LEDS_ON() set_led(255,0);set_led(255,1);set_led(255,2);set_led(255,3);set_led(255,4);set_led(255,5);set_led(255,6);set_led(255,7)

void main()
{
    LEDS_OFF();
    // install putchar handler for printf
    f_putchar = display_putchar;

    oled_init();
    oled_fullscreen();
    oled_clear(0);

    clprint("init ... ");

    LED_ON(0);
    #ifndef FAKE_SDCARD
      sdcard_init();
      fl_init(); // initialise File IO Library
    #endif
    LED_OFF(0);

    #ifndef FAKE_SDCARD
      while (fl_attach_media(sdcard_readsector, sdcard_writesector) != FAT_INIT_OK) {
          LED_ON(1);
      }
    #endif
    printf("done.\n");
    display_refresh();

    display_set_cursor(0,0);
    display_set_front_back_color(0,255);
    printf("    ===== files =====    \n\n");
    display_refresh();
    display_set_front_back_color(255,0);

    file_info_t files[MAX_FILES];
    char n_files = list_dir("/", files);
    if(n_files < 0){
        LED_ON(7);
        clprint("error listing dir");
        return;
    }

    int selected = 0;
    int pulse = 0;

    while(1){
      display_set_cursor(0,0);
      // pulsing header
      display_set_front_back_color((pulse+127)&255,pulse);
      pulse += 7;
      printf("    ===== files =====    \n\n");
      for (char i = 0; i < n_files; ++i) {
        if (i == selected) { // highlight selected
          display_set_front_back_color(0,255);
        } else {
          display_set_front_back_color(255,0);
        }
        if(files[i].is_dir){
            printf("%d> (dir) %s/\n",i,files[i].name);
        } else {
            printf("%d> %s\n",i,files[i].name);
        }
      }
      // printf("N = %d\n", n_files);c
      display_refresh();
      switch(*BUTTONS & ~1){
        case BTN_UP:
          selected--;
          break;
        case BTN_DOWN:
            selected++;
            break;
        case BTN_ZERO:
            char s[MAX_FILENAME_LENGTH];
            s[0] = '/';
            strncpy(s+1, files[selected].name, MAX_FILENAME_LENGTH-1);
            n_files = list_dir(s, files);
            if(n_files <= 0){
                LED_ON(7);
                clprint("error listing dir");
                while(1);
            }
            selected = 0;
            break;
      }

      // --- display binary representation of buttons (for test) ---
      char bin[19];
      bin[18] = '\0';
      for(int i=1; i<=8; i++){
        bin[17-i] = (*BUTTONS) >> (i-1) & 1 ? '1' : '0';
      }
      display_set_cursor(0,120);
      display_set_front_back_color(255,0);
      printf("%s", bin);
      // -----------------------------------------------------------
      // wrap around
      if (selected < 0) {
        selected = n_files - 1;
      }
      if (selected >= n_files) {
        selected = 0;
      }
    }
    // FL_UNLOCK(&_fs);
    display_refresh();
}