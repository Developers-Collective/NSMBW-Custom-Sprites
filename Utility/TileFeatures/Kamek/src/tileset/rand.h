#pragma once
#include <game.h>

#define RANDDATA "BG_ext/RandTiles.bin"
#define RANDSPECVERSION 1

struct NormalType {
    enum Value {
        None,
        Horizontal,
        Vertical,
        Both,
    };
};

struct SpecialType {
    enum Value {
        None,
        VDoubleTop,
        VDoubleBottom,
        HDoubleLeft,
        HDoubleRight,
    };
};

struct RandTileBinEntry {
    u8 tileNum;         // tile to be randomized, relative
    u8 normalType;      // see enum
    u8 specialType;     // see enum
    u8 arrayLen;        // length of tile list, due to the possibility of tiles being 00
    u32 tileArrayOffs;  // offset to tile list in u16 format, relative to start of file
};

struct RandTileBin {
    u8 version;
    u8 numEntries;
    // 2 bytes padding
    RandTileBinEntry entries[];
    // u16 random tile arrays follow
};

class dRandTileMng_c {
    public:
        dRandTileMng_c(int area);
        ~dRandTileMng_c();

        RandTileBin* randData[4];

        static dRandTileMng_c* instance;
        static dRandTileMng_c* build(int area);
};
