#pragma once
#include <game.h>

#define PROFDATA "BG_ext/ProfileTiles.bin"
#define PROFSPECVERSION 1

#define PROFILEPAL 0
#define PROFILEKOR 1
#define PROFILECHN 2

struct ProfsBinEntry {
    u8 tileNum;         // relative tile number
    // 1 byte padding
    u16 profileID;
    u8 railColorIndex;  // custom field, byte unused by game
    u8 railTexSrtIndex; // custom field, byte unused by game
    // 2 bytes padding
    float xOffs, yOffs, zOffs;
    float xScale, yScale;
    u32 settings;
};

struct ProfsBin {
    u8 version;
    u8 profileIDVersion;    // 0 = PAL and derivatives, 1 = KOR/TWN, 2 = CHN
    u8 numEntries;          // number of profile overrides
    u8 numColors;           // number of rail vertex color entries
    u8 numScales;           // number of rail material scale entries
    // 3 bytes padding
    ProfsBinEntry entries[];
    // 8-bit rgba color table follows
    // vec2 scale table follows
};
