#include "my_utils.h"
#include "fat_io_lib/src/fat_filelib.h"
#include "std.h"
#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

void set_led(int value, int l){
	*LEDS = (value<<8) | l;
}

char list_dir(const char* path, file_info_t* files){
    #ifdef FAKE_SDCARD
    for(int i=0; i<FAKE_SDCARD; i++){
        switch(i){
            case 0:
                strncpy(files[i].name, "dir1", MAX_FILENAME_LENGTH);
                files[i].is_dir = 1;
                break;
            default:
                strncpy(files[i].name, "file", MAX_FILENAME_LENGTH);
                files[i].name[4] = '0' + i;
                files[i].is_dir = 0;
        }
    }
    return FAKE_SDCARD;
    #else
    FL_DIR dirstat;
    if(!fl_opendir(path, &dirstat))
        return -1;
    struct fs_dir_ent dirent;
    char n_files = 0;
    while(fl_readdir(&dirstat, &dirent) == 0){
        strncpy(files[n_files].name, dirent.filename, MAX_FILENAME_LENGTH);
        files[n_files++].is_dir = dirent.is_dir;
    }
    fl_closedir(&dirstat);
    return n_files;
    #endif
}

void clprint(const char* str){
    oled_clear(0);
    display_set_cursor(0,0);
    display_set_front_back_color(255, 0);
    printf("%s", str);
    display_refresh();
}

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

void read_audio_file(const char* path){
	clear_audio();
	FL_FILE *f = fl_fopen(path, "rb");
    int n = 0;
    while(f==NULL && n<3){
        printf("RAF: file not found.\n");
        display_refresh();
        f = fl_fopen(path, "rb");
        n++;
    }
	if (f == NULL) {
		// error, no file
		printf("RAF: file not found.\n");
		printf("%s\n", path);
        int k=0;
        while(path[k] != '\0'){
            printf("%d ", path[k++]);
        }
		display_refresh();
		return;
	}
	while(1){
		int* addr = (int*)(*AUDIO);
		int sz = fl_fread(addr, 1, AUDIO_BLOCK_SIZE, f);
		if(sz<AUDIO_BLOCK_SIZE) break; // EOF
		while(addr == (int*)(*AUDIO)); // wait for buffer swap
	}
	clear_audio();
	fl_fclose(f);
}

void strncat(char* dest, const char* src, int n){
    int i = 0;
    while(dest[i] != '\0') i++;
    for(int j=0; i+j<n; j++){
        dest[i+j] = src[j];
        if(src[j] == '\0') return;
    }
    dest[i+n] = '\0';
}