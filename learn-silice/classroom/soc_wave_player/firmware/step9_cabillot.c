#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

#include "my_utils.h"
#include "music.h"

#include "fat_io_lib/src/fat_filelib.h"

void play_music_callback(char* loop, char* buffer, char* pause);

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
			current_music = (current_music==0 ? current_playlist->size : current_music)-1;
		}
		if(*BUTTONS & BTN_DOWN){
			current_music = (current_music+1)%current_playlist->size;
		}
		if(*BUTTONS & BTN_RIGHT){
			play_music(current_playlist->musics[current_music], play_music_callback);
		}
	}

}

void play_music_callback(char* loop, char* buffer, char* pause){
	switch(*BUTTONS & ~0x1){
		case BTN_LEFT:
			*loop = 0;
			*pause = 0;
			break;
		case BTN_RIGHT:
			if(!*pause) clear_audio();
			*pause = !*pause;
			while(*BUTTONS & BTN_RIGHT); // wait for button release
	}
	if(*pause || !*loop){
		LEDS_OFF();
		return;
	}
	unsigned int value = 0;
	for(int i=0; i<AUDIO_BLOCK_SIZE; i++)
		value += buffer[i] & 0x7F;
	value = value >> 9; // value/(AUDIO_BLOCK_SIZE:=512) we compute the mean of the absolute value of the samples in the buffer
	value = value>48 ? (value-48)>>2 : 0; // if v>48 v = (v-48)*8/32 := (v-48)/4 else v = 0 (this maps values from [48, 80] to [0, 8]) (see musics/test_led_pattern.py)
	for(int l=0; l<=value; l++)
		set_led(255, l);
	for(int l=value+1; l<8; l++)
		set_led(0, l);
}