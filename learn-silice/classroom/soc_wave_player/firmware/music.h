#ifndef MUSIC_H
#define MUSIC_H

#define MUSIC_DIR "/musics/"
#define PLAYLIST_DIR "/playlists/"

#define MAX_PATH_LENGTH 128
#define MAX_MUSIC_NAME_LENGTH 64
#define MAX_MUSICS 32

#define MAX_FILE_SIZE 512

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

#endif