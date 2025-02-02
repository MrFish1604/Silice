#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

#include "my_utils.h"
#include "music.h"

#include "fat_io_lib/src/fat_filelib.h"

void play_music_callback(char* loop);

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
	playlist_t all_musics;
	all_musics.size = n_musics;
	strcpy(all_musics.name, "All musics");
	for(int i=0; i<n_musics; i++) all_musics.musics[i] = musics + i;
	if(n_musics==0){
		clprint("No music found");
		return;
	}
	else if(n_musics == -1){
		clprint("SD card error");
		return;
	}

	playlist_t* current_playlist = &all_musics;

	int pulse = 0;

	int current_music = 1;
	while(1){
		display_set_cursor(0,0);
		display_set_front_back_color(0, 255);
		display_clear();
		printf("%s\n", current_playlist->name);
		
		for(int i=0; i<current_playlist->size; i++){
			display_set_front_back_color(i==current_music ? 0 : 255, i==current_music ? 255 : 0);
			printf("%d> %s\n", i, current_playlist->musics[i]->name);
		}
		display_refresh();

		if(*BUTTONS & BTN_UP){
			current_music--;
		}
		if(*BUTTONS & BTN_DOWN){
			current_music++;
		}
		if(*BUTTONS & BTN_RIGHT){
			play_music(current_playlist->musics[current_music], play_music_callback);
		}
	}

}

void play_music_callback(char* loop){
	if(*BUTTONS & BTN_ZERO){
		*loop = 0;// set loop to 0 to stop the main loop in read_audio_file
		set_led(255, 1);
	}
}