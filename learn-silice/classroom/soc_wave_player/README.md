# Classroom project: music player

This project is part of the course on '*Conception de Système sur Puce*' taught at TELECOM Nancy.
See the [original README](README_Original.md) for the original project description and the instructions to do it.

> The music files are copyright free and come from [Pixabay](https://pixabay.com/music).

## How to make
```sh
make cabillot # Compile the whole project and upload it to the FPGA
make CABILLOT_FIRMWARE=step9_cabillot.c cabillot # Compile the project with a custom firmware and upload it to the FPGA
make reprog   # Upload the previously compiled project to the FPGA
```

## Features
- [x] Play music files in raw format
  - [x] Play/Pause
  - [x] Next/Previous
  - [x] Returns to the menu
- [x] Playlists
  - [x] Order the musics however you want
  - [x] Play the musics in the order of the playlist
  - [x] Can contain the same music multiple times
  - [x] A music doesn't take more space in the SD card when added to several playlists
  - [x] The 'All musics' playlist is automatically created and contains all the musics found
- [x] Display the associated image of the music being played
  - [x] The image is displayed in color
  - [x] Can't display a grayscale image
  - [x] If no image is found, a text is displayed instead
- [x] LED effect synchronized with the music being played
- [x] Intro music on startup with an animation on the screen
- [x] If a menu is too long for the screen, it can be scrolled

## Behavior
- When the end of a music is reached, the next music in the playlist is played
  - If it's the last music, we go back in the menu and the selection is on the first music
- When the last music is skipped, the first music is played
- When a music is paused, the LEDs are turned off

- In the menu, keeping BTN_UP or BTN_DOWN pressed will continuously scroll the list
- Elsewhere, buttons must be pressed and released to be taken into account (debouncing)

## What did I add

- [cabillot.si](cabillot.si) contains the description of the SOC
  - It includes a unit for pwm and a unit for audio pwm
- [step9_cabillot.c](firmware/step9_cabillot.c) contains the main program for the firmware
- [my_utils.{h, c}](firmware/my_utils.h) is a library that contains my utility functions and macros
- [music.{h, c}](firmware/music.h) is a library to handle the music and playlist files
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
  - The name must be less than 26 characters long

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
- The name must be less than 26 characters long

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

## LED effect algorithm
1. For each audio block read (512 bytes), we compute the average of values (ignoring the bit of sign)
    - For that we compute the sum then we right shift by 9 (since we divide by 512)
    - Let's call it `v`
2. Then we want to map this value from $[48, 80]$ to $[0, 8]$ if it's greater than 48
    - $v := (v - 48)\frac{8-0}{80-48} = (v - 48)/4 = (v - 48) >> 2$
3. We then set the LEDs from 0 to `v` to 255 and the others to 0

```c
unsigned int v = buffer[0];
for(int i=1; i<512; i++) value += buffer[i] & 0x7F;
value = value >> 9;
value = value>48 ? (value - 48) >> 2 : 0;
// Then set the LEDs
```

> I made [this python script](musics/test_led_pattern.py) to find this algorithm.

## Known issues
- A playlist's name can't be **11 characters long** (>=12 doesn't cause any problem)
  - I then made the create_playlist function in [music.h](firmware/music.h) to ignore such files
  - I think it might be an issue with the fat library
    - Maybe because of [fat_access.c](https://github.com/ultraembedded/fat_io_lib/blob/0ef5c2bbc0ab2ff96d970a2149764d8fc377eb33/src/fat_access.c) lines 548, 656, 726 (I didn't test it though)
- There was some music files that crashed the player
  - I didn't find the reason
  - All the files in [data/musics](data/musics) should work
