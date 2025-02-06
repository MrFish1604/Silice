#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

#include "my_utils.h"
#include "music.h"

#include "fat_io_lib/src/fat_filelib.h"

#define PADDING_CHAR ' '

#define PRINT_CENTERED_WITH_PADDING(str) const int len = strlen(str);\
        if(len >= MAX_MUSIC_NAME_LENGTH) printf("%s\n", str);\
        else{\
            const int n_padding = (MAX_MUSIC_NAME_LENGTH - len) >> 1;\
            padding[n_padding] = '\0';\
            printf("%s", padding);\
            printf("%s", str);\
            if(n_padding & 1) printf("%c", PADDING_CHAR);\
            printf("%s\n", padding);\
            padding[n_padding] = PADDING_CHAR;\
        }

void play_music_callback(char* loop, char* buffer, char* pause, char* output_signal);

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

	printf("Loading musics ...");
	display_refresh();
	music_info_t musics[MAX_MUSICS];
	const int n_musics = list_music(musics);
	check_image(musics, n_musics);
	playlist_t playlists[MAX_FILES];
	playlists->size = n_musics;
	strcpy(playlists->name, "All musics");
	for(int i=0; i<n_musics; i++) playlists->musics[i] = musics + i;
	if(n_musics==0){
		clprint("No music found");
		return;
	}
	else if(n_musics == -1){
		clprint("SD card error");
		return;
	}

	const int n_playlists = list_playlist(playlists+1, musics, n_musics) + 1;
	if(n_playlists<=0){
	    clprint("SD_CARD ERROR");
	}

	int selected_playlist = 0;
	playlist_t* current_playlist = playlists;

	printf("done\n");
	display_refresh();

	char music_menu_loop = 0;
	char padding[MAX_MUSIC_NAME_LENGTH+1];
	padding[MAX_MUSIC_NAME_LENGTH] = '\0';
	memset(padding, PADDING_CHAR, MAX_MUSIC_NAME_LENGTH);

	int pulse_pl = 0;
	display_clear();
	while(1){
	    display_set_cursor(0,0);
		display_set_front_back_color((pulse_pl+127)&255,pulse_pl);
		pulse_pl += 7;
		PRINT_CENTERED_WITH_PADDING("Playlists");
		for(int i=0; i<n_playlists; i++){
            display_set_front_back_color(i==selected_playlist ? 0 : 255, i==selected_playlist ? 255 : 0);
            printf("%d> %s", i, playlists[i].name);
            int n_padding = MAX_MUSIC_NAME_LENGTH - strlen(playlists[i].name) - 4 - (i>=10) - (playlists[i].size>=10); // playlist_t.size <= 32 so we can't have more than 2 digits
            padding[n_padding] = '\0';
            printf("%s%d\n", padding, playlists[i].size);
            padding[n_padding] = PADDING_CHAR;
        }
		display_refresh();
		if(*BUTTONS & BTN_UP){
		    selected_playlist = (selected_playlist==0 ? n_playlists : selected_playlist)-1;
		}
		if(*BUTTONS & BTN_DOWN){
            selected_playlist = selected_playlist==n_playlists-1 ? 0 : selected_playlist+1;
        }
		if(*BUTTONS & BTN_RIGHT){
		    current_playlist = playlists + selected_playlist;
            music_menu_loop = 1;
            display_clear();
            while(*BUTTONS & BTN_RIGHT); // wait for button release
        }


	int pulse = 0;
	int current_music = 1;
	while(music_menu_loop){
		display_set_cursor(0,0);
		display_set_front_back_color((pulse+127)&255,pulse);
		pulse += 7;
		// printf("%s\n", current_playlist->name);
		PRINT_CENTERED_WITH_PADDING(current_playlist->name);

		for(int i=0; i<current_playlist->size; i++){
			display_set_front_back_color(i==current_music ? 0 : 255, i==current_music ? 255 : 0);
			printf("%d> %s\n", i, current_playlist->musics[i]->name);
		}
		display_refresh();

		if(*BUTTONS & BTN_UP){
			current_music = (current_music==0 ? current_playlist->size : current_music)-1;
		}
		if(*BUTTONS & BTN_DOWN){
			current_music = current_music==current_playlist->size-1 ? 0 : current_music+1;
		}
		if(*BUTTONS & BTN_RIGHT){
			int signal = play_music(current_playlist->musics[current_music], play_music_callback);
			while(signal){
				if(signal == 1){
				    if(current_music == current_playlist->size-1){
						current_music = 0;
						break;
					}
					current_music++;
				}
			    else if(signal == 2){
					current_music = (current_music==current_playlist->size-1 ? 0 : current_music+1);
				}
				else if(signal == 3){
					current_music = (current_music==0 ? current_playlist->size : current_music) - 1;
				}
				signal = play_music(current_playlist->musics[current_music], play_music_callback);
			}
			display_clear();
			while(*BUTTONS & BTN_RIGHT);
			while(*BUTTONS & BTN_LEFT); // wait for buttons release
		}
		if(*BUTTONS & BTN_LEFT){
		    music_menu_loop = 0;
			display_clear();
			while(*BUTTONS & BTN_LEFT); // wait for button release
		}
	}
	}

}

void play_music_callback(char* loop, char* buffer, char* pause, char* output_signal){
	switch(*BUTTONS & ~0x1){
		case BTN_LEFT:
			*loop = 0;
			*pause = 0;
			*output_signal = 0;
			break;
		case BTN_RIGHT:
			if(!*pause) clear_audio();
			*pause = !*pause;
			while(*BUTTONS & BTN_RIGHT); // wait for button release
			break;
		case BTN_DOWN:
			*loop = 0;
			*pause = 0;
			*output_signal = 2;
			break;
		case BTN_UP:
			*loop = 0;
			*pause = 0;
			*output_signal = 3;
			break;

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
