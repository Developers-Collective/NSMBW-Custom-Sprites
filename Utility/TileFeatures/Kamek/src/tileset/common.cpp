#include <game.h>
#include "tileset/grass.h"
#include "tileset/rand.h"

// Defines to turn off individual SPEC parts, turn off as necessary
//#define ANIMCPP
#define GRASSCPP
#define PROFOVERRIDECPP
//#define RANDCPP

// Some file names
extern char UnitHeadDataFmt, UnitDataFmt;

// Universal Tileset Slot Mod - ported from Newer
void FixUnit(u8* data, int slot) {

    // 0xFF = end of file
    while (*data != 0xFF) {

        // Get command
        u8 cmd = *data;

        // If end of row or slope, go to next byte
        if (cmd == 0xFE || (cmd & 0x80) != 0) {
            data++;
            continue;
        }

        // Clear slot data and set correct slot if the tile is not the Pa0 null tile
        if (data[1] != 0 && (data[2] & 3) != 0) {
            data[2] &= 0xFC;
            data[2] |= slot;
        }

        // Go to next tile
        data += 3;
    }
}

void FixTilesetSlot(const char* tileName, int slot) {

    // Allocate some variables on the stack
    size_t hdSize;
    char buffer[40];

    // Get the UnitHeadData
    snprintf(buffer, sizeof(buffer), &UnitHeadDataFmt, tileName);
    HeadDataEntry* hd = (HeadDataEntry*)dResMng_c::instance->res.getRes(tileName, buffer, &hdSize);

    // Get the Unit Data as well
    snprintf(buffer, sizeof(buffer), &UnitDataFmt, tileName);
    u8* unitData = (u8*)dResMng_c::instance->res.getRes(tileName, buffer);

    // If any of them isn't found, bail
    if (hd == NULL || unitData == NULL || hdSize == NULL)
        return;

    // Calculate hd length
    HeadDataEntry* hdEnd = hd + (hdSize / sizeof(HeadDataEntry));

    // Fix each unit by parsing HeadData
    for (HeadDataEntry* currHd = hd; currHd < hdEnd; currHd++)
        FixUnit(unitData + currHd->offset, slot);
}


// Preload the data files so we can reuse them anytime in the level
dBgUnit_c* LoadBins(dBgUnit_c* unit, EGG::Heap* heap, int area, int layer) {

    // Only do this operation in the first layer
    if (layer == 0) {

        // Process each tileset
        for (int slot = 0; slot < 4; slot++) {

            // Get tileset name (this has already been set up so it is safe to call)
            const char* tileName = dBgGlobal_c::instance->getEnvironment(area, slot);

            // If tileset is null, skip
            if (tileName[0] == '\0')
                continue;

            // Universal tileset slot fix
            FixTilesetSlot(tileName, slot);

            // Get the grass file
            #ifdef GRASSCPP
            GrassBin* grass = (GrassBin*)dResMng_c::instance->res.getRes(tileName, GRASSDATA);

            // If the file was found and is valid, create the class
            if (grass != NULL && grass->version == GRASSSPECVERSION && dGrassBinMng_c::instance == NULL)
                dGrassBinMng_c::build(grass, slot);
            #endif

            // Create the random class
            #ifdef RANDCPP
            dRandTileMng_c::build(area);
            #endif
        }
    }

    // Original function call
    return dBgUnit_c::create(unit, heap, area, layer);
}


// Hijack the function end to destroy the data files
int destroyTilesetDataFiles() {
    #ifdef GRASSCPP
    delete dGrassBinMng_c::instance;
    #endif

    #ifdef RANDCPP
    delete dRandTileMng_c::instance;
    #endif

    return 1;
}
