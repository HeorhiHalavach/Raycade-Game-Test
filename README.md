# Nowa gra na Raycade

Szablon projektu gry na silniku [Raycade](https://github.com/seba9989/Raycade).

## Kompilacja

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/gra
```

Z [devenv](https://devenv.sh/): `build`, `run`, `fmt`, `clean`, `edit`.

CMake pobiera silnik z GitHuba (gałąź `main`). Żeby pracować na lokalnej kopii silnika:

```sh
cmake -S . -B build -DFETCHCONTENT_SOURCE_DIR_RAYCADE=$HOME/Code/Raycade
# albo w devenv:
RAYCADE_DIR=~/Code/Raycade build
```

## Struktura

```
src/main.cpp        konfiguracja silnika i start
src/scenes/         sceny (makro SCENE) - jeden plik na scenę
assets/             zasoby kopiowane obok pliku wykonywalnego (font.ttf z polskimi znakami)
```

Nowe pliki `.cpp` w `src/` są dołączane automatycznie (wystarczy ponownie uruchomić `build`).
Sceny muszą leżeć w programie, nie w bibliotece - inaczej linker pominie ich rejestrację.
