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

#define MAX_LISTED_ITEMS 12

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

static int init_music_step = 0;

void play_music_callback(char* loop, char* buffer, char* pause, char* output_signal, char* run_callback);
void init_music_callback(char* loop, char* buffer, char* pause, char* output_signal, char* run_callback);

void main()
{
	clear_audio();
	LEDS_OFF();
	// install putchar handler for printf
	f_putchar = display_putchar;

	oled_init();
	oled_fullscreen();

	memset((void*)display_framebuffer(),0x00, DISPLAY_PIXELS);
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
	printf("done\n");
	printf("Loading playlists ...");
	display_refresh();
	playlist_t playlists[MAX_FILES]; // playlists[0] is the "All musics" playlist
	playlists->size = n_musics;
	strcpy(playlists->name, "All musics");
	for(int i=0; i<n_musics; i++) playlists->musics[i] = musics + i;
	memset(playlists->musics+n_musics, (int)NULL, MAX_FILES - playlists->size);
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
	display_clear();
	display_refresh();
	init_music_step = 0;
	if(!(*BUTTONS & BTN_ONE)){
	   read_audio_file(MUSIC_DIR INIT_MUSIC_FILENAME, init_music_callback);
	   for(;init_music_step<128*128; init_music_step++){
	       oled_pix(0, 0, 0);
	   	oled_wait();
	   }
	}

	char music_menu_loop = 0;
	char padding[MAX_MUSIC_NAME_LENGTH+1];
	padding[MAX_MUSIC_NAME_LENGTH] = '\0';
	memset(padding, PADDING_CHAR, MAX_MUSIC_NAME_LENGTH);

	int pulse_pl = 0;
	int menu_start_pl_item = 0;
	display_clear();
	while(1){
	    display_set_cursor(0,0);
		display_set_front_back_color((pulse_pl+127)&255,pulse_pl);
		pulse_pl += 7;
		PRINT_CENTERED_WITH_PADDING("Playlists");
		int n_print_pl;
		if(menu_start_pl_item>0){
            display_set_front_back_color(255, 0);
            printf("...\n");
        }
		for(n_print_pl=menu_start_pl_item; n_print_pl<n_playlists && n_print_pl-menu_start_pl_item<MAX_LISTED_ITEMS; n_print_pl++){
            display_set_front_back_color(n_print_pl==selected_playlist ? 0 : 255, n_print_pl==selected_playlist ? 255 : 0);
            printf("%d> %s", n_print_pl, playlists[n_print_pl].name);
            int n_padding = MAX_MUSIC_NAME_LENGTH - strlen(playlists[n_print_pl].name) - 4 - (n_print_pl>=10) - (playlists[n_print_pl].size>=10); // playlist_t.size <= 32 so we can't have more than 2 digits
            padding[n_padding] = '\0';
            printf("%s%d\n", padding, playlists[n_print_pl].size);
            padding[n_padding] = PADDING_CHAR;
        }
		if(n_print_pl<n_playlists){
		    display_set_front_back_color(255, 0);
            printf("...\n");
		}

		display_refresh();
		if(*BUTTONS & BTN_UP){
		    selected_playlist = (selected_playlist==0 ? n_playlists : selected_playlist)-1;
			if((selected_playlist+1)%MAX_LISTED_ITEMS==0){
                menu_start_pl_item = selected_playlist - MAX_LISTED_ITEMS + 1;
                display_clear();
                display_set_cursor(0, 0);
            }
			else if(selected_playlist == n_playlists-1){
                menu_start_pl_item = n_playlists - n_playlists%MAX_LISTED_ITEMS;
                display_clear();
                display_set_cursor(0, 0);
            }
		}
		if(*BUTTONS & BTN_DOWN){
            selected_playlist = selected_playlist==n_playlists-1 ? 0 : selected_playlist+1;
            if(selected_playlist%MAX_LISTED_ITEMS==0){
                menu_start_pl_item = selected_playlist;
                display_clear();
                display_set_cursor(0, 0);
            }
        }
		if(*BUTTONS & BTN_RIGHT){
		    current_playlist = playlists + selected_playlist;
            music_menu_loop = 1;
            display_clear();
            while(*BUTTONS & BTN_RIGHT); // wait for button release
        }


	int pulse = 0;
	int current_music = 0;
	int menu_start_item = 0;
	while(music_menu_loop){
		display_set_cursor(0,0);
		display_set_front_back_color((pulse+127)&255,pulse);
		pulse += 7;
		// printf("%s\n", current_playlist->name);
		PRINT_CENTERED_WITH_PADDING(current_playlist->name);
		int n_printed_items;
		if(menu_start_item>0){
            display_set_front_back_color(255, 0);
            printf("...\n");
        }
		for(n_printed_items=menu_start_item; n_printed_items<current_playlist->size && n_printed_items-menu_start_item<MAX_LISTED_ITEMS; n_printed_items++){
			display_set_front_back_color(n_printed_items==current_music ? 0 : 255, n_printed_items==current_music ? 255 : 0);
			printf("%d> %s\n", n_printed_items, current_playlist->musics[n_printed_items]->name);
		}
		if(n_printed_items<current_playlist->size){
            display_set_front_back_color(255, 0);
            printf("...\n");
        }
		display_refresh();

		if(*BUTTONS & BTN_UP){
			current_music = (current_music==0 ? current_playlist->size : current_music)-1;
			if((current_music+1)%MAX_LISTED_ITEMS==0){
			    menu_start_item = current_music - MAX_LISTED_ITEMS + 1;
                display_clear();
                display_set_cursor(0, 0);
            }
            else if(current_music == current_playlist->size-1){
                menu_start_item = current_playlist->size - current_playlist->size%MAX_LISTED_ITEMS;
                display_clear();
                display_set_cursor(0, 0);
            }
		}
		if(*BUTTONS & BTN_DOWN){
			current_music = current_music==current_playlist->size-1 ? 0 : current_music+1;
			if(current_music%MAX_LISTED_ITEMS==0){
			    menu_start_item = current_music;
			    display_clear();
				display_set_cursor(0, 0);
			}
		}
		if(*BUTTONS & BTN_RIGHT){
			int signal = play_music(current_playlist->musics[current_music], play_music_callback);
			while(signal){
				if(signal == 1){
				    if(current_music == current_playlist->size-1){
						current_music = 0;
						menu_start_item = 0;
						break;
					}
					current_music++;
				}
			    else if(signal == 2){
					current_music = (current_music==current_playlist->size-1 ? 0 : current_music+1);
					menu_start_item = MAX_LISTED_ITEMS*(current_music/MAX_LISTED_ITEMS);
				}
				else if(signal == 3){
					current_music = (current_music==0 ? current_playlist->size : current_music) - 1;
					menu_start_item = MAX_LISTED_ITEMS*(current_music/MAX_LISTED_ITEMS);
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

void play_music_callback(char* loop, char* buffer, char* pause, char* output_signal, char* run_callback){
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

#define STEPS 512
void init_music_callback(char* loop, char* buffer, char* pause, char* output_signal, char* run_callback){
    if(init_music_step >= DISPLAY_PIXELS || !*run_callback) return;
    for(int i=0; i<STEPS; i++){
        int r = (init_music_step + i)/DISPLAY_WIDTH;
        int c = (init_music_step + i)%DISPLAY_WIDTH;
        int v = 255*((c>=33 && c<43 || c>=83 && c<93) || (c>=43 && c<83 && ( r>=10 && r<20 || r>=40 && r<50 || r>=70 && r<80 || r>=100 && r<110)));
        oled_pix(v, v, v);
        oled_wait();
    }
    init_music_step += STEPS;
    *run_callback = 0;
}
