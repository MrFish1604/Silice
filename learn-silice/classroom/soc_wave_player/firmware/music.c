#include "music.h"

#include "fat_io_lib/src/fat_filelib.h"
#include "std.h"
#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

#include "my_utils.h"


char list_music(music_info_t* musics){
    FL_DIR dirstat;
    if(!fl_opendir(MUSIC_DIR, &dirstat))
        return -1;
    struct fs_dir_ent dirent;
    char n_files = 0;
    while(fl_readdir(&dirstat, &dirent) == 0){
        if(dirent.is_dir) continue;
        strncpy(musics[n_files].path, MUSIC_DIR, MAX_PATH_LENGTH);
        strncat(musics[n_files].path, dirent.filename, MAX_PATH_LENGTH);
        int i = 0;
        for(; i<MAX_MUSIC_NAME_LENGTH && dirent.filename[i]!='.' && dirent.filename[i]!='\0'; i++)
            musics[n_files].name[i] = dirent.filename[i];
        musics[n_files++].name[i] = '\0';
    }
    fl_closedir(&dirstat);
    return n_files;
}

void play_music(const music_info_t* music, void (*loop_callback)(char*)){
    display_clear();
    display_set_cursor(0,0);
    display_set_front_back_color(255, 0);
    printf("Playing %s\n", music->name);
    display_refresh();

    read_audio_file(music->path, loop_callback);
}