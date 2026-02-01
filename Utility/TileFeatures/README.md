# Tile Features v1.0.1
*by CLF78 & Nin0*

## Explanation
This feature allows to easily add new behaviour overwrites for tiles. Specifically, it allows to replace tiles of custom tilesets with custom flowers and bushes or spawn actors.
<br><br>

## License
These files are released under Mozilla Public License 2.0, following the license of the [NSMASR-v2 repo](https://github.com/CLF78/NSMASR-v2/).
<br><br>

## Kamek
Open `include/g3dhax.h` and add this:
```cpp
	class bmdl_c : public scnLeaf_c {
		public:
			virtual ~bmdl_c();
			virtual int returnUnknown();
			virtual void free();
			virtual void setAnm(void* /*banm_c&*/ anm);
			virtual void play();

			void* /* banm_c* */ anim;
	};

	class smdl_c : public bmdl_c {
		public:
			virtual ~smdl_c();
	};
```
Place above block right after the line stating:
```cpp
	class proc_c : public scnLeaf_c {
		public:
			bool setup(void *allocator = 0, u32 *size = 0);

			int returnUnknown() { return 2; }
			virtual void drawOpa() = 0;
			virtual void drawXlu() = 0;
	};
```

In the same file, also add right below this code (inside the class):
```cpp
		class ResFile {
			public:
				void * /*ResFileData*/ data;

				ResFile(void *p = NULL) {
					data = p;
				}

				void Init();
				bool CheckRevision() const;

				void *GetResMdl(const char *str) const;
				void *GetResAnmChr(const char *str) const;
				void *GetResAnmVis(const char *str) const;
				void *GetResAnmClr(const char *str) const;
				void *GetResAnmTexPat(const char *str) const;
				void *GetResAnmTexSrt(const char *str) const;
```
the following few lines:
```cpp
				void * /*ResTex*/ GetResTex(const char *str) const;
				void * /*ResTex*/ GetResTex(const ResName n) const;
				void * /*ResTex*/ GetResTex(u32 idx) const;
```



Open `include/game.h` and add these definitions at the end of the file (before the `#endif` line):
```cpp
struct BGBuffer {
    u16 tiles[0x100];
};

class dBgUnit_c {
    public:
        void* vtable;
        BGBuffer* allocatedBlocks[257];
        u8 usedIDs[2048];
        u16 nextID;
        // 2 bytes padding
        u32 pa0_id, pa1_id, pa2_id, pa3_id; // only set for Nintendo tilesets and used for randomization
        u32 layerId, areaId;
        EGG::FrmHeap* heap;
        bool isNot2CastleA1;
        // 3 bytes padding

        u16* getTileP(u32 x, u32 y, u32* block, bool placeTile); // last param is unused

        void generateTile(BGRender* render, u16* tileArray, int arrayLength, int slot);
        void generateHorizontalTile(BGRender* render, u16* tileArray, int arrayLength, int slot);
        void generateVerticalTile(BGRender* render, u16* tileArray, int arrayLength, int slot);

        static dBgUnit_c* create(dBgUnit_c* unit, EGG::Heap* heap, int area, int layer);
};

class dBgTexMng_c {
    public:
        void setupAnimTile(int slot, u16 tileNum, char* name, u8* pFrameTimes, bool reverse);
};

typedef struct {
    u16 offset;
    u8 width;
    u8 height;
} HeadDataEntry;

class dBgGlobal_c {
    public:
        void* vtable;
        u8* bgDatas[4][3];
        dBgUnit_c* units[4][3];
        HeadDataEntry* unitHeadDatas[4][4];
        u8* unitDatas[4][4];
        char tilesetNames[4][4][32];
        EGG::FrmHeap* heaps[4][4];
        u32 _324;

        char* getEnvironment(int area, int tileSlot);

        static dBgGlobal_c* instance;
};


class resCallback_c {
    public:
        void* vtable;
};

class dRes_c {
    public:
        void* array; // actually a subclass but i can't be arsed
        u16 count;
        // 2 bytes padding
        resCallback_c* callback;

        void* getRes(const char* arcName, const char* subfileName) const;
        void* getRes(const char* arcName, const char* subfileName, size_t* length) const;
};
 

class dResMng_c {
    public:
        void* vtable;
        dRes_c res;
        resCallback_c callback;
 

        static dResMng_c* instance;
};
 

class dBgActorManager_c {
    public:
        class BgObj_c {
            public:
                u16 uniqueId;
                u16 x, y;
                u8 layer;
                // 1 byte padding
                u32 profileId;
        };

        struct BgObjName_t {
            u32 tileNum;
            u16 profileId;
            u8 railColorIndex; // custom field, byte unused by game
            u8 railTexSrtIndex; // custom field, byte unused by game
            float xOffs, yOffs, zOffs;
            float xScale, yScale;
            u32 settings;
        };

        void* vtable;
        mHeapAllocator_c allocator;
        VEC3 screenBL;
        VEC3 screenTR;
        BgObj_c* objects;
        int count;
        int type;

        static BgObjName_t* bgObjNameList; // unofficial name

};


class daUnitRail_c : public dActor_c {
    public:
        dHeapAllocator_c allocator;
        m3d::smdl_c mdl;
        bool insertedSuccessfully;
        // 3 bytes padding
};

struct FlowerEntry {
    float x, y;
    u8 type;
    // 3 bytes padding
};

class dBg_c : public dBase_c {
    public:
        u8 unk[0x8F408]; // TODO: document this properly
        u32 grassCount;
        u32 flowerCount;
        FlowerEntry grassEntries[100];
        FlowerEntry flowerEntries[100];
        // TODO: the rest of the class
};
```

Further, if it exists, replace the part looking like this:
```cpp
namespace EGG {
	class Heap {
		public:
			static void* becomeCurrentHeap(void* newHeap);
	};
}
```
with one that instead looks like this:
```cpp
namespace EGG {
	class Heap {
		public:
			static void* becomeCurrentHeap(void* newHeap);
	};

	class FrmHeap : public Heap {};
}
```
If no such code part exists, put the entire above block right above the line:
```cpp
struct BGBuffer {
```

Next, open `src/linegod.cpp` and remove this code block from it:
```cpp
struct dBgActorManager_c {
	u32 vtable;		// 0x00
	u8 _04[0x34];	// 0x04
	BgActor *array;	// 0x38
	u32 count;		// 0x3C
	u32 type;		// 0x40
};
```
and additionally, change this line:
```cpp
BgActor *ac = &dBgActorManager->array[i];
```
to this:
```cpp
BgActor *ac = (BgActor*)&dBgActorManager->objects[i];
```


Open `src/levelspecial.cpp` and add this at the very end of the file:
```cpp
#undef time
```

It should something look like this:
```cpp
	self->lastEvState = newEvState;
}

#undef time
```


Add these addresses to your `kamek_pal.x`:
```cpp
	returnTo8007E3AC = 0x8007E3AC;
	returnTo8007E274 = 0x8007E274;
	returnTo808B2A44 = 0x808B2A44;
	returnTo80077F8C = 0x80077F8C;
	UnitHeadDataFmt = 0x803100BC;
	instance__9dResMng_c = 0x8042A318;
	getRes__6dRes_cCFPCcPCcPUl = 0x800DF2D0;
	UnitDataFmt = 0x803100D0;
	getRes__6dRes_cCFPCcPCc = 0x800DF270;
	instance__11dBgGlobal_c = 0x8042A0D0;
	getEnvironment__11dBgGlobal_cFii = 0x800813F0;
	create__9dBgUnit_cFP9dBgUnit_cPQ23EGG4Heapii = 0x80083560;
	Rail_ = 0x8030FFF4;
	RailWhite = 0x80310000;
	RailDaishizen = 0x80310010;
	RailMinigame = 0x80310020;
	OriginalOverrides = 0x8030FFE0;
	bgObjNameList__17dBgActorManager_c = 0x8042A0BC;
	RailColors = 0x8097DE48;
	RailScales = 0x8097DE70;
	objHana = 0x80972690;
	objHanaBrres = 0x8097267C;
	objHanaDaishizen = 0x809726BC;
	objHanaDaishizenBrres = 0x8097269C;
	GetResTex__Q34nw4r3g3d7ResFileCFPCc = 0x8023a060;
	isEntryAnimationDone__Q23m3d11anmTexPat_cFi = 0x80167060;
	instance__10dScStage_c = 0x8042a4a8;
	ContinueFromLoadingCRSIN = 0x8091EC78;
```

<br>

- Copy `tileset.yaml` into the `Kamek` folder
- Further, copy the `tileset` folder with all its files into the `Kamek\src` folder, so you have a `Kamek\src\tileset` folder with multiple `.cpp` and `.h` files inside.
- Also copy `loading.S` (or adjust your `loading.S` if you already have one) into `Kamek\src`
- Copy `loading.yaml` into the `Kamek` folder if you did not have a `loading.S` file before this already
- Copy `timekeeper.h` and `dMultiMng_c.h` into the `Kamek\include` folder
- Add `tileset.yaml` and eventually `loading.yaml` in your main `.yaml` file, e.g. `NewerProject.yaml`
- Remove `tilesetfixer.yaml` from your main `.yaml` file.

<br><br>

## Game files
Copy the files in the `Riivolution Patch` folder into your riivolution patch so they are patched into the games `Object` folder. In Newer this is the folder `NewerSMBW\SpriteTex` and in NSMBWer+ `NSMBWer+\Object\` for example.

<br><br>

## Editing Tilesets
To use the new features introduced by this code, you can use the newest source version of [Puzzle Next](https://github.com/Developers-Collective/Puzzle-Next/tree/master) to directly edit the new subfile types inside tilesets. Alternatively you can use the scripts that can be found in the [NSMASR-v2 repo](https://github.com/CLF78/NSMASR-v2/tree/master/tools/tilesettools).

Using Puzzle Next to add More Flowers and Grasses: Open your tileset, then go to PlantTiles tab and click Add Entry. Your Tilenum is the tile that your flower or bush will replace. For Example: Tilenum 0 is the first tile in the tileset which is in the top left corner. Tilenum 15 would be the end of the row in the top right corner. The first tile on row 2 would be Tilenum 16. The FlowerType will be the color of the Flower. There are only 3 colors per flower. For Example: 1=Color 1, 2=Color 2, and 3=Color 3. The GrassType represents which part of the grass the tile is. For Example: 1=Start Piece, 5=End Piece. FlowerFile is which flower model will be loaded. For Example: 0=Overworld, 1=Jungle, and 2=Custom Flower. GrassType is the type of grass that will be loaded. For Example: 0-1=Normal Grass, 2=Custom Grass. Once you are finished, just save your tileset and your done!

<br><br>

## Add New Flowers & Bushes
Open `obj_hana_custom.arc` for flowers and `obj_kusa_custom.arc` for bushes with e.g. BrawlCrate. Inside you find at least one `.brres` file starting at index 02. The indices 00 and 01 reference the original flowers and bushes of the game. If you want to add new entries, add e.g. a subfile `t03.brres` inside these `.arc` files. The `.bin` file in the tilesets allows up to 256 different values, so up to 254 custom flowers or bushes. Long before reaching this number, you will likely hit memory issues however as all custom flowers and bushes will be always loaded inside levels, so adding too many of them may cause issues for your project.
