#ifndef MY_UTILS_H
#define MY_UTILS_H

#define MAX_FILES 32
#define MAX_FILENAME_LENGTH 128

#define AUDIO_BLOCK_SIZE 512

#define BTN_ZERO 2
#define BTN_ONE 4
#define BTN_UP 8
#define BTN_DOWN 16
#define BTN_LEFT 32
#define BTN_RIGHT 64

#define LED_OFF(l) set_led(0,l)
#define LED_ON(l) set_led(255,l)
#define LEDS_OFF() set_led(0,0);set_led(0,1);set_led(0,2);set_led(0,3);set_led(0,4);set_led(0,5);set_led(0,6);set_led(0,7)
#define LEDS_ON() set_led(255,0);set_led(255,1);set_led(255,2);set_led(255,3);set_led(255,4);set_led(255,5);set_led(255,6);set_led(255,7)

#define IMAGE_DIR "/imgs/"

// #define FAKE_SDCARD 6

struct file_info{
    char name[MAX_FILENAME_LENGTH];
    char is_dir;
};
typedef struct file_info file_info_t;

void set_led(int value, int l);
char list_dir(const char* path, file_info_t* files);
void clprint(const char* str);
void clear_audio();
void read_audio_file(const char* path, void (*loop_callback)(char*, char*, char*));

void display_image_rgb(const char* path, const unsigned char size);

void strncat(char* dest, const char* src, int n);
char strequ(const char* s1, const char* s2);


#endif