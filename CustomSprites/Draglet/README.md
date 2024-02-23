# Draglet v1.0.2
*by Synel, model imported by G4L & Zementblock*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0
- [More SFX](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSFX) by Asu (converted by Synel for Newer 1.3.X) and how to use it
- [Sprite Data Utils](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel


## Kamek
- Add `src/meragon.cpp` in your `src` folder
- Add `meragon.yaml` in your `Kamek` folder
- Reference the `meragon.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `Meragon` for the ProfileID and SpriteID
- Add these addresses to your `kamek_pal.x`:
```
	GetParent__7fBase_cFv = 0x801626D0;
	doSearchNearPlayer__8dActor_cFP7Point2dP7Point2d = 0x80064790;
```
- If you don't have a `sqrt` function in your `game.h`, add it **below** the `float pow(float x, float y);` line: `float sqrt(float x) { return pow(x, 0.5f); }`
- In `game.h`, add a forward declaration of `dAcPy_c` (`class dAcPy_c;`) before the `dActor_c` class if that's not already the case
- In `game.h`, add this method in the `dActor_c` class if you don't have it: `dAcPy_c* doSearchNearPlayer(Vec2 *distance, Vec2 *relative_pos);`
- Reference the 3 new sfx like this in the sfx list (replace XXXX, YYYY and ZZZZ by the correct ids):
```cpp
"meragon_fire_start",		// XXXX
"meragon_fire",				// YYYY
"meragon_wing",				// ZZZZ
```
- Define the two following variables (replace XXXX, YYYY and ZZZZ by the correct ids):
```cpp
#define SFX_MERAGON_FIRE_START XXXX
#define SFX_MERAGON_FIRE YYYY
#define SFX_MERAGON_WING ZZZZ
```
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder
- Add the rwav file into your `Music/sfx` folder if using Newer **OR** `Sound/stream/sfx` folder if using NSMBWer


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX by the sprite ID)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
