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
        musics[n_files].name[i] = '\0';
        musics[n_files++].has_img = 0;
    }
    fl_closedir(&dirstat);
    return n_files;
}

char check_image(music_info_t* musics, int n_musics){
    FL_DIR dirstat;
    if(!fl_opendir(IMAGE_DIR, &dirstat))
        return -1;
    struct fs_dir_ent dirent;
    char n_files = 0;
    char img_names[MAX_MUSICS][MAX_MUSIC_NAME_LENGTH];
    while(fl_readdir(&dirstat, &dirent) == 0){
        if(dirent.is_dir) continue;
        int i=0;
        for(; i<MAX_MUSIC_NAME_LENGTH && dirent.filename[i]!='.' && dirent.filename[i]!='\0'; i++)
            img_names[n_files][i] = dirent.filename[i];
        img_names[n_files++][i] = '\0';
    }
    fl_closedir(&dirstat);
    for(int i=0; i<n_musics; i++){
        // display_clear();
        // display_set_cursor(0,0);
        // printf("%s\n", musics[i].name);
        for(int j=0; j<n_files; j++){
            // printf("%s\n", img_names[j]);
            if(strequ(musics[i].name, img_names[j])){
                musics[i].has_img = 1;
                printf("ok\n");
                break;
            }
            // display_refresh();
            // while(!(*BUTTONS & BTN_ZERO));
            // while(*BUTTONS & BTN_ZERO);
        }
    }
    return n_files;
}

char play_music(const music_info_t* music, void (*loop_callback)(char*, char*, char*)){
    if(music->has_img){
        char path[MAX_PATH_LENGTH] = IMAGE_DIR;
        strncat(path, music->name, MAX_PATH_LENGTH);
        strncat(path, ".raw", MAX_PATH_LENGTH);
        display_image_rgb(path, 128);
    }
    else{
        display_clear();
        display_set_cursor(0,0);
        display_set_front_back_color(255, 0);
        printf("   Playing %s\n", music->name);
        printf("\nNo image found\n");
        display_refresh();
    }

    return read_audio_file(music->path, loop_callback);
}