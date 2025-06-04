#include <game.h>
#include "tileset/grass.h"

// External filenames
extern char objHana, objHanaBrres;
extern char objHanaDaishizen, objHanaDaishizenBrres;

// Externs for ASM calls
extern "C" {
bool AreCustomFlowersLoaded();
int AddFlowerEntry(dBg_c* bg, u16 tileNum, u32 x, u32 y);
void* LoadCustomFlowerBrres();
void* LoadCustomGrassBrres(dRes_c* res, char* originalArc, char* originalName, int style);
}

// Static instance (and a pointer to it for ASM because the compiler is garbage)
dGrassBinMng_c* dGrassBinMng_c::instance;
extern "C" {dGrassBinMng_c** instancePtr = &dGrassBinMng_c::instance;}

// Build function
dGrassBinMng_c* dGrassBinMng_c::build(GrassBin* rawData, u8 slot) {

    // If an instance is already initialized, return it
    if (dGrassBinMng_c::instance != NULL)
        return dGrassBinMng_c::instance;

    // Create class and return it
    return new dGrassBinMng_c(rawData, slot);
}

// Constructor
dGrassBinMng_c::dGrassBinMng_c(GrassBin* rawData, u8 slot) {

    // Set variables
    this->data = rawData;
    this->tileSlot = slot;

    // Set static instance
    dGrassBinMng_c::instance = this;
}

// Destructor
dGrassBinMng_c::~dGrassBinMng_c() {

    // Reset static instance
    dGrassBinMng_c::instance = NULL;
}

// Get flower data from tile
GrassBinEntry* dGrassBinMng_c::getFlowerData(u16 tileNum) {

    // Traverse through all entries
    for (int i = 0; i < this->data->numEntries; i++) {

        // Get entry
        GrassBinEntry* current = &this->data->entries[i];

        // Check if tile number matches, and if so return this entry
        if (tileNum == (current->tileNum | 0x100 * this->tileSlot))
            return current;
    }

    // None found, return NULL
    return NULL;
}

// Helper function
bool AreCustomFlowersLoaded() {
    return (dGrassBinMng_c::instance != NULL);
}

int AddFlowerEntry(dBg_c* bg, u16 tileNum, u32 x, u32 y) {

    // Get the flower data for this tile
    GrassBinEntry* data = dGrassBinMng_c::instance->getFlowerData(tileNum);

    // If none is found, it's just a regular tile, ignore it
    if (data == NULL)
        return tileNum;

    // Set up flower if it was enabled and we can allocate one
    if (data->flowerValue > 0 && bg->flowerCount < 99) {
        FlowerEntry* flower = &bg->flowerEntries[bg->flowerCount];
        flower->x = float(x);
        flower->y = -float(y);
        flower->type = data->flowerValue - 1;
        bg->flowerCount++;
    }

    // Set up grass if it was enabled and we can allocate one
    if (data->grassValue > 0 && bg->grassCount < 99) {
        FlowerEntry* grass = &bg->grassEntries[bg->grassCount];
        grass->x = float(x);
        grass->y = -float(y);
        grass->type = data->grassValue - 1;
        bg->grassCount++;
    }

    // Return null tile so it doesn't get drawn
    return 0;
}

void* LoadCustomFlowerBrres() {

    // Allocate name buffer on the stack
    char buffer[16];

    // Pick correct arc and brres files
    char* arcFile = NULL;
    char* brresFile = NULL;

    switch(dGrassBinMng_c::instance->data->flowerStyle) {
        case 0:
            arcFile = &objHana;
            brresFile = &objHanaBrres;
            break;
        case 1:
            arcFile = &objHanaDaishizen;
            brresFile = &objHanaDaishizenBrres;
            break;
        default:
            arcFile = FLOWERFILE;
            snprintf(buffer, sizeof(buffer), BRRESFMT, dGrassBinMng_c::instance->data->flowerStyle);
            brresFile = buffer;
    }

    // Load it
    return dResMng_c::instance->res.getRes(arcFile, brresFile);
}

void* LoadCustomGrassBrres(dRes_c* res, char* originalArc, char* originalName, int style) {

    // Replicate original call if custom brres is not to be loaded
    if (style < 2 || dGrassBinMng_c::instance->data->grassStyle < 2)
        return res->getRes(originalArc, originalName);

    // Load custom brres otherwise
    char buffer[16];
    snprintf(buffer, sizeof(buffer), BRRESFMT, dGrassBinMng_c::instance->data->grassStyle);
    return res->getRes(GRASSFILE, buffer);
}




// Swap 4th flower texture if found
void* flower4Texture(nw4r::g3d::ResFile* res, const char* originalName) {
    // Get custom texture
    void* tex = res->GetResTex("obj_hana05");

    // Return if not null
    if (tex != NULL)
        return tex;

    // Else return original
    return res->GetResTex(originalName);
}

// Swap 5th flower texture if found
void* flower5Texture(nw4r::g3d::ResFile* res, const char* originalName) {
    // Get custom texture
    void* tex = res->GetResTex("obj_hana04");

    // Return if not null
    if (tex != NULL)
        return tex;

    // Else return original
    return res->GetResTex(originalName);
}
