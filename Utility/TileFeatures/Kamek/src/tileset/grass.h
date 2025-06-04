#pragma once
#include <game.h>

#define GRASSDATA "BG_ext/PlantTiles.bin"
#define GRASSSPECVERSION 1
#define FLOWERFILE "obj_hana_custom"
#define GRASSFILE "obj_kusa_custom"
#define BRRESFMT "g3d/t%02d.brres"

struct GrassBinEntry {
    u8 tileNum;        // relative tileset number
    u8 flowerValue:4;  // 0 = none, 1 = pink, 2 = yellow, 3 = blue, 4-5 = custom, any other value = invalid
    u8 grassValue:4;   // 0 = none, 1 = left edge, 2-4 = middles, 5 = right edge, any other value = invalid
};

struct GrassBin {
    u8 version;
    u8 numEntries;
    u8 flowerStyle;  // 0 = nohara, 1 = daishizen, 2+ = custom
    u8 grassStyle;   // 0 = nohara, 1 = nohara again, 2+ = custom
    GrassBinEntry entries[];
};

// Helper class for extra data that needs to be saved
class dGrassBinMng_c {
    public:
        dGrassBinMng_c(GrassBin* rawData, u8 slot);
        ~dGrassBinMng_c();

        GrassBinEntry* getFlowerData(u16 tileNum);

        GrassBin* data;
        u8 tileSlot;

        static dGrassBinMng_c* instance;
        static dGrassBinMng_c* build(GrassBin* rawData, u8 slot);
};
