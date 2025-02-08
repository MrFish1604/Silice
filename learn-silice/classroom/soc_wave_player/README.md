# Classroom project: music player

This project is part of the course on '*Conception de Système sur Puce*' taught at TELECOM Nancy.
See the [original README](README_Original.md) for the original project description and the instructions to do it.

> The music files are copyright free and come from [Pixabay](https://pixabay.com/music).

## How to make
```sh
make cabillot # Compile all the project and upload it to the FPGA
make reprog   # Upload the previously compiled project to the FPGA
```

## What did I add

- [cabillot.si](cabillot.si) contains the description of the SOC
  - It includes a unit for pwm and a unit for audio pwm
- [step9_cabillot.c](firmware/step9_cabillot.c) contains the main program for the firmware
- [my_utils.{h, c}](firmware/my_utils.h) is a library that contains my utility functions and macros
- [music.{h, c}](firmware/music.h) is a library that contains to handle the music and playlists files
- [data](data) contains the files (musics, images, playlists) for music player
  - It's content should be copied as is in the root of the SD card

## How to use

- The SD card should be formatted in FAT32
- The SD card must contain **3 folders**
  - 'musics' containing the music files
  - 'imgs' containing the images files
  - 'playlists' containing the playlists files
- There must be a music file named 'music.raw' in the 'musics' folder
  - It will be played as the intro music on startup
  - If no such file is found, an error message will be shown. Press BTN_ONE to discard it.
  - Using the file in [data/musics/music.raw](data/musics/music.raw) is recommended
### Musics files
- They must be in raw format
  - Use `sh encode_music.sh <input_file.{mp3, wav}>; mv music.raw music_name.raw` to convert a music file to raw
- They must be named as `music_name.{whateverYouWant}`
  - The part before the first dot will be used as the music name
### Images files
- They must be in raw format
  - Use `python3 to_raw.py <input_file.{jpg, png, webp, ...}>` to convert an image file to raw
  - One pixel is represented by 3 bytes, one for each color (RGB)
- They must be named as `{associated_music_name}.raw`
  - The part before the first dot is the associated music name

### Playlists files
- A playlist file contains a list of music names separated by a newline
- The filename **is** the playlist name
- A playlist can contain the same music multiple times
- The order of the music in the playlist is the order they will be played in

### Buttons
I named the buttons as follow:
```plaintext
<BTN_ZERO>  <BTN_ONE>
                                   <BTN_UP>
                      <BTN_LEFT>  <BTN_DOWN>  <BTN_RIGHT>
```
- **BTN_LEFT**: Go back to the previous menue
- **BTN_RIGHT**:
  - *In menus:* Select the current playlist or music
  - *While playing a music:* Pause/Resume the music
- **BTN_UP**:
  - *In menus:* Go up in the list
  - *While playing a music:* Play the previous music
- **BTN_DOWN**:
  - *In menus:* Go down in the list
  - *While playing a music:* Play the next music
- **BTN_ONE**: Keep pressed while initialization to skip the intro music
- **BTN_ZERO**: Discard an error message

## Known issues
- A playlist's name can't be **11 characters long**
  - I think it might be an issue with the fat library
    - Maybe because of [fat_access.c](firmware/fat_io_lib/src/fat_access.c) lines 548, 656, 726 (I didn't test it though)
- There was some music files that crashed the player
  - I didn't find the reason
  - All the files in [data/musics](data/musics) should work
