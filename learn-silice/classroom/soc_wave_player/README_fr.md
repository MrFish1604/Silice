# Classroom project: music player

> Les fichiers musicaux sont libres de droit et proviennent de [Pixabay](https://pixabay.com/music).

## Comment compiler
```sh
make cabillot # Compile tout le projet et le télécharge sur le FPGA
make CABILLOT_FIRMWARE=step9_cabillot.c cabillot # Compile le projet avec un firmware personnalisé et le télécharge sur le FPGA
make reprog   # Télécharge le projet précédemment compilé sur le FPGA
```

## Fonctionnalités
- [x] Joue des fichiers musicaux au format raw
  - [x] Lecture/Pause
  - [x] Suivant/Précédent
  - [x] Retourner au menu
- [x] Playlists
  - [x] Ordonne les musiques comme on le souhaite
  - [x] Joue les musiques dans l'ordre de la playlist
  - [x] Peut contenir plusieurs fois la même musique
  - [x] Une musique ne prend pas plus de place sur la carte SD lorsqu'elle est ajoutée à plusieurs playlists
  - [x] La playlist 'All musics' est automatiquement créée et contient toutes les musiques trouvées
- [x] Affiche l'image associée à la musique en cours de lecture
  - [x] L'image est affichée en couleur
  - [x] Ne peut pas afficher une image en niveaux de gris
  - [x] Si aucune image n'est trouvée, un texte est affiché à la place
- [x] Effet de lumière avec les LEDs synchronisé avec la musique en cours de lecture
- [x] Musique d'introduction au démarrage avec une animation sur l'écran
- [x] Si un menu est trop long pour l'écran, il peut être défilé

## Comportement
- Lorsque la fin d'une musique est atteinte, la musique suivante de la playlist est jouée
  - Si c'est la dernière musique, on retourne dans le menu et la sélection est sur la première musique
- Lorsque la dernière musique est sautée, la première musique est jouée
- Lorsqu'une musique est en pause, les LEDs sont éteintes

- Dans le menu, maintenir BTN_UP ou BTN_DOWN enfoncé fera défiler la liste en continue
- Ailleurs, les boutons doivent être pressés et relâchés pour être pris en compte (anti-rebond)

## Les fichiers que j'ai ajouté
- [cabillot.si](cabillot.si) contient la description du SOC
  - Il inclut une unité pour le pwm et une unité pour le pwm audio
- [step9_cabillot.c](firmware/step9_cabillot.c) contient le programme principal pour le firmware
- [my_utils.{h, c}](firmware/my_utils.h) est une bibliothèque qui contient mes fonctions utilitaires et macros
- [music.{h, c}](firmware/music.h) est une bibliothèque qui pour gérer les fichiers musicaux et de playlist
- [data](data) contient les fichiers (musiques, images, listes de lecture) pour le lecteur de musique
  - Son contenu doit être copié tel quel à la racine de la carte SD

## Comment utiliser

- La carte SD doit être formatée en FAT32
- La carte SD doit contenir **3 dossiers**
  - 'musics' contenant les fichiers musicaux
  - 'imgs' contenant les fichiers images
  - 'playlists' contenant les fichiers de listes de lecture
- Il doit y avoir un fichier musical nommé 'music.raw' dans le dossier 'musics'
  - Il sera joué comme musique d'introduction au démarrage
  - Si aucun fichier de ce type n'est trouvé, un message d'erreur sera affiché. Appuyez sur BTN_ONE pour le rejeter.
  - Il est recommandé d'utiliser le fichier [data/musics/music.raw](data/musics/music.raw)

### Fichiers musicaux
- Ils doivent être au format raw
  - Utilisez `sh encode_music.sh <input_file.{mp3, wav}>; mv music.raw music_name.raw` pour convertir un fichier musical en raw
- Ils doivent être nommés `music_name.{whateverYouWant}`
  - La partie avant le premier point sera utilisée comme nom de la musique

### Fichiers images
- Ils doivent être au format raw
  - Utilisez `python3 to_raw.py <input_file.{jpg, png, webp, ...}>` pour convertir un fichier image en raw
  - Un pixel est représenté par 3 octets, un pour chaque couleur (RGB)
- Ils doivent être nommés `{associated_music_name}.raw`
  - La partie avant le premier point est le nom de la musique associée

### Fichiers de playlist
- Un fichier de playlist contient une liste de noms de musique séparés par un retour à la ligne
- Le nom de fichier **est** le nom de la playlist
- Une playlist peut contenir la même musique plusieurs fois
- L'ordre des musiques dans la playlist est l'ordre dans lequel elles seront jouées

### Buttons
J'ai nommé les boutons comme suit:
```plaintext
<BTN_ZERO>  <BTN_ONE>
                                   <BTN_UP>
                      <BTN_LEFT>  <BTN_DOWN>  <BTN_RIGHT>
```

- **BTN_LEFT**: Retourne au menu précédent
- **BTN_RIGHT**:
  - *Dans un menu*: Valide la sélection de la playlist ou de la musique
  - *Pendant la lecture d'une musique*: Pause/Reprend la musique
- **BTN_UP**:
  - *Dans un menu*: Monte dans la liste
  - *Pendant la lecture d'une musique*: Joue la musique précédente
- **BTN_DOWN**:
  - *Dans un menu*: Descend dans la liste
  - *Pendant la lecture d'une musique*: Joue la musique suivante
- **BTN_ONE**: Maintenir enfoncé pendant l'initialisation pour sauter la musique d'introduction
- **BTN_ZERO**: Passer un message d'erreur

## Algorithme pour l'effet de lumière des LEDs
1. Pour chaque bloc audio lu (512 octets), nous calculons la moyenne des valeurs (en ignorant le bit de signe)
    - Pour cela, nous calculons la somme qu'on right shift par 9 (puisque nous divisons par 512)
    - Appelons cette valeur `v`
2. Ensuite, on veut mapper `v` de $[48, 80]$ à $[0, 8]$ si elle est supérieure à 48
    - $v := (v - 48)\frac{8-0}{80-48} = (v - 48)/4 = (v - 48) >> 2$
3. Enfin, on allume les LEDs de 0 à `v` et on éteint les autres

```c
unsigned int v = buffer[0];
for(int i=1; i<512; i++) value += buffer[i] & 0x7F;
value = value >> 9;
value = value>48 ? (value - 48) >> 2 : 0;
// Then set the LEDs
```


## Problèmes connus
- Le nom d'une playlist ne doit pas contenir **11 caractères** (>=12 ne cause pas de problème)
  - Je pense que cela peut venir de la fat library
    - Peut-être à cause de [fat_access.c](https://github.com/ultraembedded/fat_io_lib/blob/0ef5c2bbc0ab2ff96d970a2149764d8fc377eb33/src/fat_access.c) lignes 548, 656, 726 (je n'ai pas testé)
- Il y avait des fichiers musicaux qui faisaient planter le lecteur
  - Je n'ai pas trouvé la raison
  - Tous les fichiers dans [data/musics](data/musics) devraient fonctionner
