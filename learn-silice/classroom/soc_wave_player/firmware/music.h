#ifndef MUSIC_H
#define MUSIC_H

#define MUSIC_DIR "/musics/"
#define PLAYLIST_DIR "/playlists/"

#define MAX_PATH_LENGTH 128
#define MAX_MUSIC_NAME_LENGTH 25 // Based on the width of the OLED screen
#define MAX_MUSICS 32

#define PL_NAME_SIZE_THAT_CREATES_A_BUG 11 // I don't know why but fl_open returns NULL if the filename is 11 characters long. (>=12 works btw) \_(O.O))_/

struct music_info{
    char has_img;
    char name[MAX_MUSIC_NAME_LENGTH+1];
    char path[MAX_PATH_LENGTH+1]; // Never print this. I don't know why but fl_open returns NULL if it was printed before. \_(O.O))_/
};
typedef struct music_info music_info_t;

struct playlist{
    char name[MAX_MUSIC_NAME_LENGTH+1];
    music_info_t* musics[MAX_MUSICS];
    unsigned char size;
};
typedef struct playlist playlist_t;

char list_music(music_info_t* musics);
char check_image(music_info_t* musics, int n_musics);

char play_music(const music_info_t* music, void (*loop_callback)(char*, char*, char*, char*));

char create_playlist(const char* path, playlist_t* playlist, music_info_t* musics, int n_musics);
char list_playlist(playlist_t* playlists, music_info_t* musics, int n_musics);

#endif
