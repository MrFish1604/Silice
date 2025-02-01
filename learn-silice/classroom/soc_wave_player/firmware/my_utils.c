#include "my_utils.h"
#include "fat_io_lib/src/fat_filelib.h"
#include "std.h"
#include "config.h"
#include "sdcard.h"
#include "std.h"
#include "oled.h"
#include "display.h"
#include "printf.h"

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