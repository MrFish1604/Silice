#ifndef MY_UTILS_H
#define MY_UTILS_H

#define MAX_FILES 32
#define MAX_FILENAME_LENGTH 64

#define AUDIO_BLOCK_SIZE 512

#define BTN_ZERO 2
#define BTN_ONE 4
#define BTN_UP 8
#define BTN_DOWN 16
#define BTN_LEFT 32
#define BTN_RIGHT 64

// #define FAKE_SDCARD 6

struct file_info{
    char name[MAX_FILENAME_LENGTH];
    char is_dir;
};
typedef struct file_info file_info_t;


char list_dir(const char* path, file_info_t* files);
void clprint(const char* str);
void clear_audio();
void read_audio_file(const char* path);


#endif