#pragma once
#include <game.h>

#define ANIMDATA "BG_ext/AnimTiles.bin"
#define ANIMTILE "BG_ext/anim_%02x.bin"
#define ANIMSPECVERSION 1

struct AnimsBinEntry {
    u16 delayOffs;  // pointer to null-terminated u8 array; offset is relative to start of file
    u8 tileNum;     // relative tileset number
    bool reverse;   // reverse animation if true
};

struct AnimsBin {
    u8 version;
    u8 numEntries;
    AnimsBinEntry entries[];
    // delay arrays follow
};
