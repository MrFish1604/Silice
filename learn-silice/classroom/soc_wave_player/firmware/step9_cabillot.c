#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

#include "my_utils.h"
#include "music.h"

#include "fat_io_lib/src/fat_filelib.h"

void main()
{
	clear_audio();
	LEDS_OFF();
	// install putchar handler for printf
	f_putchar = display_putchar;

	oled_init();
	oled_fullscreen();

	memset((void*)display_framebuffer(),0x00,128*128);
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

	music_info_t musics[MAX_MUSICS];
	const int n_musics = list_music(musics);
	if(n_musics==0){
		clprint("No music found");
		return;
	}
	else if(n_musics == -1){
		clprint("SD card error");
		return;
	}

	int current_music = 0;
	while(1){
		// oled_clear(0);
		// display_set_cursor(0,0);
		// display_set_front_back_color(255, 0);
		// printf("Playing %d> %s\n", current_music, musics[current_music].name);
		// // printf("%s\n", musics[current_music].path); // Never print this !!!
		// display_refresh();
		// read_audio_file(musics[current_music].path);
		play_music(musics + current_music);
		current_music = (current_music+1)%n_musics;
	}

}