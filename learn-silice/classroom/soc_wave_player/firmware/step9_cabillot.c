#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

#include "my_utils.h"

#include "fat_io_lib/src/fat_filelib.h"

void main()
{
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

	file_info_t musics[MAX_FILES];
	const int n_musics = list_dir(MUSIC_DIR, musics);
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
		clprint(musics[current_music].name);
		char path[MAX_FILENAME_LENGTH+1] = MUSIC_DIR;
		strncat(path, musics[current_music].name, MAX_FILENAME_LENGTH);
		// clprint(path);
		// while(1);
		// strncpy(path+1, musics[current_music].name, MAX_FILENAME_LENGTH-1);
		read_audio_file(path);
		current_music = (current_music+1)%n_musics;
	}

}