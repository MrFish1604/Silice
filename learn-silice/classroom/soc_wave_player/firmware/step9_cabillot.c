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

	memset(display_framebuffer(),0x00,128*128);
	display_refresh();

	display_set_cursor(0,0);
	display_set_front_back_color(255,0);
	printf("init ... ");
	display_refresh();
	LEDS_ON();

	// init sdcard
	sdcard_init();
	// initialise File IO Library
	fl_init();
	// attach media access functions to library
	while (fl_attach_media(sdcard_readsector, sdcard_writesector) != FAT_INIT_OK) {
		// try again, we need this
	}
	printf("done.\n");
	display_refresh();
	LEDS_OFF();

	file_info_t musics[MAX_FILES];
	const int n_musics = list_dir("/", musics);
	if(n_musics == -1){
		clprint("SD card error");
		return;
	}

	int current_music = 0;
	while(1){
		clprint(musics[current_music].name);
		char path[MAX_FILENAME_LENGTH];
		path[0] = '/';
		strncpy(path+1, musics[current_music].name, MAX_FILENAME_LENGTH-1);
		read_audio_file(path);
		current_music = (current_music+1)%n_musics;
	}

}