#include <game.h>
#include "tileset/rand.h"

// Extern for ASM call
extern "C" {
bool DoRandTile(dBgUnit_c* unit, BGRender* render);
}

// Static instance
dRandTileMng_c* dRandTileMng_c::instance;

// Build function
dRandTileMng_c* dRandTileMng_c::build(int area) {

    // If an instance is already initialized, return it
    if (dRandTileMng_c::instance != NULL)
        return dRandTileMng_c::instance;

    // Create class and return it
    return new dRandTileMng_c(area);
}

// Constructor
dRandTileMng_c::dRandTileMng_c(int area) {

    // Process every tileset
    for (int slot = 0; slot < 4; slot++) {

        // Get tileset name
        char* tilesetName = dBgGlobal_c::instance->getEnvironment(area, slot);

        // If tileset name is valid, get data
        RandTileBin* data = NULL;
        if (tilesetName[0] != '\0')
            data = (RandTileBin*)dResMng_c::instance->res.getRes(tilesetName, RANDDATA);

        // Set it
        this->randData[slot] = data;
    }

    // Set static instance
    dRandTileMng_c::instance = this;
}

// Destructor
dRandTileMng_c::~dRandTileMng_c() {

    // Reset static instance
    dRandTileMng_c::instance = NULL;
}

// Get entry from data given a tile
RandTileBinEntry* GetTileFromData(RandTileBin* data, u8 tileNum) {

    // Process every entry
    for (int i = 0; i < data->numEntries; i++) {
        RandTileBinEntry* currEntry = &data->entries[i];

        // If tile matches, return the entry
        if (currEntry->tileNum == tileNum)
            return currEntry;
    }

    // No entry found, return none
    return NULL;
}

// Main random function
bool DoRandTile(dBgUnit_c* unit, BGRender* render) {

    // Get tile and slot
    u16 tileNum = render->currTile & 0x3FF;
    int slot = tileNum >> 8;

    // Get random data for slot
    RandTileBin* data = dRandTileMng_c::instance->randData[slot];

    // If no data is found, fall back to Nintendo's code
    if (data == NULL || data->version != RANDSPECVERSION)
        return false;

    // Find the entry for this tile
    RandTileBinEntry* entry = GetTileFromData(data, tileNum & 0xFF);

    // If no tile is found, still skip Nintendo's code and render original tile
    if (entry == NULL)
        return true;

    // Initialize pointer
    u16* tilePtr = NULL;

    // Handle special tiles first
    switch(entry->specialType) {
        case SpecialType::VDoubleBottom:
            // Get pointer to top tile
            tilePtr = unit->getTileP(render->currX*16, (render->currY-1) * 16, NULL, false);

            // If pointer is not null, set the new tile
            if (tilePtr != NULL)
                render->currTile = (*tilePtr + 16) & 0xFF;
            return true;

        case SpecialType::HDoubleRight:
            // Get pointer to left tile
            tilePtr = unit->getTileP((render->currX-1) * 16, render->currY*16, NULL, false);

            // If pointer is not null, set the new tile
            if (tilePtr != NULL)
                render->currTile = (*tilePtr + 1) & 0xFF;
            return true;
        default:
            break;
    }

    // Get a pointer to the random choices list
    u16* tileArray = (u16*)((u32)data + entry->tileArrayOffs);

    // Handle regular tiles with the game's original functions (setting slot to 0 in order to insert it manually later)
    switch(entry->normalType) {
        case NormalType::Horizontal:
            unit->generateHorizontalTile(render, tileArray, entry->arrayLen, 0);
            break;
        case NormalType::Vertical:
            unit->generateVerticalTile(render, tileArray, entry->arrayLen, 0);
            break;
        case NormalType::Both:
            unit->generateTile(render, tileArray, entry->arrayLen, 0);
            break;
        default:
            return true;
    }

    // Reload tile and add slot to it
    render->currTile |= slot * 0x100;
    return true;
}
