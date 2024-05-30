# Dry Bones Remake & Para Bones v1.0.0
*by Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0
- [More SFX](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSFX) by Asu (converted by Synel for Newer 1.3.X) and how to use it
- [Sprite Data Utils](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel


## Kamek
- Add `src/dryBones.cpp` and `src/paraBones.cpp` in your `src` folder
- Add `dryBones.yaml` in your `Kamek` folder
- Reference the `dryBones.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `DryBones` and `ParaBones` for the ProfileID and SpriteID
- Reference the new sfx like this in the sfx list (replace the XXXN by the correct ids):
```cpp
// Dry Bones
"karon/break_down",					// XXX1
"karon/break",						// XXX2
"karon/down",						// XXX3
"karon/recover_sign",				// XXX4
"karon/recover",					// XXX5
"karon/find",						// XXX6
"karon/attack_hit",					// XXX7
"karon/die",						// XXX8
```
- Define the following variable (replace XXXX by the correct id):
```cpp
// Dry Bones
#define SFX_KARON_BREAK_DOWN XXX1
#define SFX_KARON_BREAK XXX2
#define SFX_KARON_DOWN XXX3
#define SFX_KARON_RECOVER_SIGN XXX4
#define SFX_KARON_RECOVER XXX5
#define SFX_KARON_FIND XXX6
#define SFX_KARON_ATTACK_HIT XXX7
#define SFX_KARON_DIE XXX8
```
- Add this function in the `dAcPy_c` class in `include/game.h` if it doesn't exist yet (replace `dAcPy_c` by the correct class name if needed):
```cpp
static dAcPy_c *findNearestPlayer(Vec2 pos, float dist = 0) {
	dAcPy_c *nearest = 0;
	float nearestDist = -1;

	for (int i = 0; i < 4; i++) {
		dAcPy_c *player = dAcPy_c::findByID(i);
		if (!player) continue;

		float d = sqrtf((player->pos.x - pos.x) * (player->pos.x - pos.x) + (player->pos.y - pos.y) * (player->pos.y - pos.y));
		if ((d < dist || dist <= 0) && (nearestDist == -1 || d < nearestDist)) {
			nearestDist = d;
			nearest = player;
		}
	}

	return nearest;
}
```
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder
- Add the folder from the `sfx` folder into your `Music/sfx` folder if using Newer **OR** `Sound/sfx/new` folder if using NSMBWer


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX and YYY by the sprite IDs)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX and YYY by the sprite IDs)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the image folder in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
