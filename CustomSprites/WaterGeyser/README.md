# Water Geyser v1.0.1
*by Synel & LucasD10*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0
- [Sprite Data Utils](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel
- [More SFX](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSFX) by Asu (converted by Synel for Newer 1.3.X) and how to use it


## Kamek
- Add `src/waterGeyser.cpp` and `src/sandPillar.h` in your `src` folder
- Add `waterGeyser.yaml` in your `Kamek` folder
- Reference the `waterGeyser.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `WaterGeyser` for the ProfileID and SpriteID

- In `include/game.h`, under
```cpp
class daPlBase_c : public dStageActor_c {
	public:
```
add this enum if it doesn't exist:
```cpp
enum FootNoteEnum {
	FOOTNOTE_ROCK = 0,
	FOOTNOTE_SNOW = 1,
	FOOTNOTE_SAND = 2,
	FOOTNOTE_ROCK_2 = 3,
	FOOTNOTE_DIRT = 4,
	FOOTNOTE_WATER = 5,
	FOOTNOTE_CLOUD = 6,
	FOOTNOTE_BLOWSAND = 7,
	FOOTNOTE_MANTA = 8,
	FOOTNOTE_SAND_2 = 9,
	FOOTNOTE_ROCK_3 = 10,
	FOOTNOTE_LEAF = 11,
	FOOTNOTE_ROCK_4 = 12
};
```
- Still in this file, still in the `daPlBase_c` class, replace `u8 data2[0x1418 - 0x1008];` with
```cpp
u8 data2[0x111c - 0x1008];
FootNoteEnum footnote_type;
u8 data7[0x1418 - 0x1120];
```
(You may need to adjust these values if you edited this class before or if you're using NSMBWer+)
- Still in this file, still in the `daPlBase_c` class, add
```cpp
bool setFunsui();
```
under the `bool testFlag(int flag);` line
- Still in `include/game.h`, under
```cpp
namespace mEf {
	class effect_c : public EGG::Effect {
		public:
			~effect_c();

			void clear();

			virtual bool probablyCreateWithName(const char *name, u32 unk);
			virtual bool spawn(const char *name, u32 unk, const Vec *pos=0, const S16Vec *rot=0, const Vec *scale=0);
			virtual bool spawnWithMatrix(const char *name, u32 unk, const Mtx *mtx);

			// these two deal with mEf::effectCB_c and crap. absolutely no idea.
			virtual bool _vfA8(/* tons and tons of params */);
			virtual bool _vfAC(/* a slightly smaller amount of params */);

			virtual bool _vfB0(const Vec *pos=0, const S16Vec *rot=0, const Vec *scale=0);
			virtual bool _vfB4(const Mtx *mtx);
	};
```
add
```cpp
class levelEffect_c : public effect_c {
	mEf::levelEffect_c * prev;
	mEf::levelEffect_c * next;
	bool set;
	bool isActive;
	ushort padMaybe;
	uint _120;
	uint _124;
};
```
if it doesn't exist yet
- Add these addresses to your `kamek_pal.x` if they don't exist:
```cpp
	left__13ActivePhysicsFv = 0x8008c550;
	right__13ActivePhysicsFv = 0x8008c530;
	FUN_808027f0 = 0x808027f0;
	setFunsui__10daPlBase_cFv = 0x8004a210;
	removeFromList__13BasicColliderFv = 0x800DAFC0;
	top__13ActivePhysicsFv = 0x8008c4d0;
	FUN_80802d30 = 0x80802d30;
	FUN_8008fba0 = 0x8008fba0;
	FUN_808025b0 = 0x808025b0;
```
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX by the sprite ID)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
