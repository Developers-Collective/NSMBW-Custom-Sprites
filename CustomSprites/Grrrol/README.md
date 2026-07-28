# Grrrol v1.0.0
*by SilverBuckeye, LucasD10, and TheMarioMan*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0
- [Sprite Data Utils](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel


## Kamek
- Add `src/grrrol.cpp` and `src/grrrolSpawner.cpp` in your `src` folder
- Add `include/UnitType.h`, `include/dMultiMng_c.h`, and `include/timekeeper.h` in your `include` folder if you do not have them already
- Add `grrrol.yaml` and `grrrolSpawner.yaml` in your `Kamek` folder
- Reference the `grrrol.yaml` and `grrrolSpawner.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `Grrrol` and `GrrrolSpawner` for the ProfileID and SpriteID
- Add these addresses to your `kamek_pal.x`:
```cpp
	instance__10dScStage_c = 0x8042a4a8;
	DisplayScoreAddPopup = 0x800B3540;
	AddScore = 0x80060690;
```

- Open `src/levelspecial.cpp` and add this at the very end of the file if it doesn't exist:
```cpp
#undef time
```
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
The next few parts only have to be done for Newer 1.3.X
- In `src/grrrol.cpp`, replace `if (!block || block->isDeleted)` with this:
```cpp
if (!block || block->_B)
```
- In `src/grrrol.cpp`, replace `u8 playerId = blockMain->playerID;` with this:
```cpp
u8 playerId = blockMain->_694;
```
- In `src/grrrol.cpp`, replace `return sColl && sColl->owner && !sColl->owner->isDeleted` with this:
```cpp
return sColl && sColl->owner && !sColl->owner->_B
```
- In `src/grrrol.cpp`, replace `if (!spawner || spawner->isDeleted || spawner->currentLayerID != this->currentLayerID)` with this:
```cpp
if (!spawner || spawner->_B || spawner->currentLayerID != this->currentLayerID)
```
- In `src/grrrolSpawner.cpp`, replace `fBase_c *child = fBase_c::searchById(childID);` with this:
```cpp
child = fBase_c::search(childID)
```
- In `src/grrrolSpawner.cpp`, replace `if (child && !child->isDeleted && child->profileId == ProfileId::Grrrol) {` with this:
```cpp
if (child && !child->_B && child->profileId == ProfileId::Grrrol) {
```

- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder or `Object` if you're on NSMBWer+


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX & YYY with the sprite IDs)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX & YYY with the sprite IDs)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
