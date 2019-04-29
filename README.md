### SMS Plus
This is a modernized, cleaned up, basic, standalone port of
Charles MacDonald's SMS Plus. The goal of this project is to be useful
to homebrewers or emulator developers who need a simple reference
Sega Master System/Game Gear emulator for testing.

Currently runs on Linux, macOS, BSD, and Windows.

#### Compiling
You will require GLFW3, and libepoxy to build.
```
make
```
Options may be set in the .ini file. An example .ini file is included:
```
cp shell/smsplus.ini.default smsplus.ini
```
```
Usage: ./smsplus [FILE]

Controls:

  Player 1 Up       Up
  Player 1 Down     Down
  Player 1 Left     Left
  Player 1 Right    Right
  Player 1 B1       Z
  Player 1 B2       A

  Player 2 Up       I
  Player 2 Down     K
  Player 2 Left     J
  Player 2 Right    L
  Player 2 B1       ;
  Player 2 B2       '

  Reset             Tab
  Pause             \
  Start             Enter

  Screenshot        F9
  Exit              Escape
```
