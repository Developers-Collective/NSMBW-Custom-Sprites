# Camera Scroll Limiter v1.0.1
*by Synel & CLF78*


> [!IMPORTANT]\
> **DO NOT** use the vanilla scroll limiters in combinaison with this one, as the vanilla ones do some weird shenanigans on the used slots in the `BgGmBase` class.


> [!WARNING]\
> Known issues:
> - When the camera tries to zoom out just after the level loaded, the camera can shake weirdly during the zoom.
> - Stacking multiple of these with the same settings with a distance ≤ to the camera view can make the camera not understand which one has the priority.
> 
> These issues may be fixed in the future


## Kamek
- Add `src/limitLineFixes.S` and `src/cameraScrollLimiter.cpp` in your `src` folder
- Add `/cameraScrollLimiter.yaml` in your `Kamek` folder
- Reference the `cameraScrollLimiter.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `CameraScrollLimiter` for the ProfileID and SpriteID
- In `include/game.h`, add this function (under `float floor(float x);`) if it doesn't exist:
```cpp
int sign(float x) { return (int)(x > 0) - (int)(x < 0); }
```
- Still in `include/game.h`, declare these enums in the `BgGmBase` class:
```cpp
enum LimitLineType {
	UNK_0 = 0,
	UD_LEFT = 1,
	UD_RIGHT = 2,
	UNK_3 = 3,
	LR_TOP = 4,
	LR_BOTTOM = 5
};

enum LimitLineFlags {
	LIMIT_SET_1 = 1 /* right/bottom */,
	LIMIT_SET_2 = 2 /* top/left */,
	VERTICAL_SPRITE_2 = 8 /* unused? */,
	TOP = 16,
	BOTTOM = 32,
	LEFT = 64,
	RIGHT = 128,
	NOT_PERMEABLE_1 = 256 /* top/left */,
	NOT_PERMEABLE_2 = 512 /* right/bottom */,
	UNK_1000 = 4096,
	UNK_2000 = 8192,
	UNK_4000 = 16384
};
```
- Still in `include/game.h` and in the `BgGmBase` class, replace the existing `limitLineEntry_s` struct with this one:
```cpp
struct limitLineEntry_s {
	float leftX; /* lineLeft */
	float rightX; /* lineRight (+16) */
	float yLimitTop; /* lineLeft, lineRight */
	float yLimitBottom; /* lineLeft, lineRight */
	float topLimitTriggerPos; /* lineLeft, lineRight */
	float spriteYPos; /* lineLeft, lineRight */
	float topY;
	float bottomY;
	float xLimitLeft;
	float xLimitRight;
	float leftLimitTriggerPos;
	float spriteXPos;
	float zoneTop; /* lineLeft, lineRight */
	float zoneBottom; /* lineLeft, lineRight */
	float zoneLeft; /* lineLeft, lineRight */
	float zoneRight; /* lineLeft, lineRight */
	u16 flags; /* lineLeft, lineRight | LimitLineFlags
	1 = Left Line, 2 = Right Line
	8 = Is UE2 instead of UE

	10 = First nybble is nonzero
	20 = First nybble is zero

	100 = First nybble is nonzero,
		settings 0x1000 (bit 19) is not set

	200 = First nybble is zero,
	settings 0x1000 (bit 19) is not set */
	u8 pad[2];
};
```
- Still in `include/game.h` and in the `BgGmBase` class, add these functions at the end of the class:
```cpp
limitLineEntry_s* getFreeLineLimitSlot(int zoneId) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			if (limitLines[zoneId][i][j].flags == 0) {
				return &limitLines[zoneId][i][j];
			}
		}
	}

	OSReport("No free limit line slots in zone %d\n", zoneId);
	return 0;
}

limitLineEntry_s* addLineLimitButBetter(limitLineEntry_s* limitLineEntry, int zoneId) {
	limitLineEntry_s *entry = getFreeLineLimitSlot(zoneId);
	memcpy(entry, limitLineEntry, sizeof(limitLineEntry_s));
	entry->flags |= LIMIT_SET_1 | LIMIT_SET_2;

	return entry;
}
```

<details>
  <summary>Your `BgGmBase` class should look about like this (click to show)</summary>

```cpp
class BgGmBase : public dBase_c {
public:
	enum LimitLineType {
		UNK_0 = 0,
		UD_LEFT = 1,
		UD_RIGHT = 2,
		UNK_3 = 3,
		LR_TOP = 4,
		LR_BOTTOM = 5
	};

	enum LimitLineFlags {
		LIMIT_SET_1 = 1 /* right/bottom */,
		LIMIT_SET_2 = 2 /* top/left */,
		VERTICAL_SPRITE_2 = 8 /* unused? */,
		TOP = 16,
		BOTTOM = 32,
		LEFT = 64,
		RIGHT = 128,
		NOT_PERMEABLE_1 = 256 /* top/left */,
		NOT_PERMEABLE_2 = 512 /* right/bottom */,
		UNK_1000 = 4096,
		UNK_2000 = 8192,
		UNK_4000 = 16384
	};

	struct something_s {
		u16 x, y;
		int layer, countdown, tile;
	};
	struct limitLineEntry_s {
		float leftX; /* lineLeft */
		float rightX; /* lineRight (+16) */
		float yLimitTop; /* lineLeft, lineRight */
		float yLimitBottom; /* lineLeft, lineRight */
		float topLimitTriggerPos; /* lineLeft, lineRight */
		float spriteYPos; /* lineLeft, lineRight */
		float topY;
		float bottomY;
		float xLimitLeft;
		float xLimitRight;
		float leftLimitTriggerPos;
		float spriteXPos;
		float zoneTop; /* lineLeft, lineRight */
		float zoneBottom; /* lineLeft, lineRight */
		float zoneLeft; /* lineLeft, lineRight */
		float zoneRight; /* lineLeft, lineRight */
		u16 flags; /* lineLeft, lineRight | LimitLineFlags
		1 = Left Line, 2 = Right Line
		8 = Is UE2 instead of UE

		10 = First nybble is nonzero
		20 = First nybble is zero

		100 = First nybble is nonzero,
			settings 0x1000 (bit 19) is not set

		200 = First nybble is zero,
		settings 0x1000 (bit 19) is not set */
		u8 pad[2];
	};
	struct manualZoomEntry_s {
		float x1, x2, y1, y2;
		u8 unkValue6, zoomLevel, firstFlag;
	};
	struct beets_s {
		float _0, _4;
		u8 _8;
	};

	u32 behaviours; //type?

	int somethingCount;
	something_s somethings[256];

	// Limit lines grouped by:
	// 64 zones; 8 groups IDed by mysterious setting; 16 lines per group
	limitLineEntry_s limitLines[64][8][16];

	manualZoomEntry_s manualZooms[64];

	u32 _8F478, _8F47C;

	beets_s beets1[100];
	beets_s beets2[100];
	// TODO, a lot

	u16 *getPointerToTile(int x, int y, int layer, int *pBlockNum = 0, bool unused = false);

	// Note: these tile numbers are kinda weird and involve GetTileFromTileTable
	void placeTile(u16 x, u16 y, int layer, int tile);

	void makeSplash(float x, float y, int type); // 80078410

	limitLineEntry_s* getFreeLineLimitSlot(int zoneId) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 16; j++) {
				if (limitLines[zoneId][i][j].flags == 0) {
					return &limitLines[zoneId][i][j];
				}
			}
		}

		OSReport("No free limit line slots in zone %d\n", zoneId);
		return 0;
	}

	limitLineEntry_s* addLineLimitButBetter(limitLineEntry_s* limitLineEntry, int zoneId) {
		limitLineEntry_s *entry = getFreeLineLimitSlot(zoneId);
		memcpy(entry, limitLineEntry, sizeof(limitLineEntry_s));
		entry->flags |= LIMIT_SET_1 | LIMIT_SET_2;

		return entry;
	}
};
```

</details>

- Compile your code


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX by the sprite ID)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
