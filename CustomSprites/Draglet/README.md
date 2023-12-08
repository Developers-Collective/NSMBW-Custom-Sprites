# Draglet v1.0.0
*by Synel, model imported by G4L & Zementblock*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- More sprites code by Nin0 (can be found here: https://github.com/N-I-N-0/New-Super-Mario-Lost-Worlds)
- More SFX code by Asu (converted by Synel for Newer 1.3.X) and how to use it (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/More-SFX)


## Kamek
- Add `src/meragon.cpp` in your `src` folder
- Add `meragon.yaml` in your `Kamek` folder
- Reference the `meragon.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `Meragon` for the ProfileID and SpriteID
- Add this address to your `kamek_pal.x`:
```
	doSearchNearPlayer__8dActor_cFP7Point2dP7Point2d = 0x80064790;
```
- If you don't have a `sqrt` function in your `game.h`, add one this one below the `float pow(float x, float y);` one: `float sqrt(float x) { return pow(x, 0.5f); }`
- In `game.h`, add a forward declaration of `dAcPy_c` before the `dActor_c` class if that's not already the case
- In `game.h`, add this method if you don't have it: `dAcPy_c* doSearchNearPlayer(Vec2 *distance, Vec2 *relative_pos);`
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
- In `sfx.h`, add `#include <game.h>` at the top of the file if you don't have it
- Then, add this function if it doesn't exist in the same file (before the `#endif` line):
```cpp
void playSoundDistance(nw4r::snd::SoundHandle handle, Vec3 pos, int id, float volume = 1.0, float pitch = 1.0, float distance = 500.0) {
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	if (cwci == 0) return;

	Vec2 dist = {
		cwci->screenCentreX - pos.x,
		cwci->screenCentreY - pos.y
	};
	float v = max<float>(0.0, (1.0 - (sqrtf(dist.x * dist.x + dist.y * dist.y) / distance)) * 1.0);
	if (v <= 0.0) return;
	else if (v > 1.0) v = 1.0;

	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, id, 1);
	handle.SetVolume(volume * v, 1);
	if (pitch != 1.0) handle.SetPitch(pitch);
}
```
- Compile your code

PS: just in case I forgot to do it myself, these functions have been renamed so revert them to what they were before if you want this to work:
`damagePlayer` is `_vf220`
`playRegularStompSound` is `_vf260`
`playSpinStompSound` is `_vf268`
`playYoshiStompSound` is `_vf278`
`visualDropKill` is `_vf148`
`visualDropKillAndCoinJump` is `_vf14C`


## Game
- Add the arc file into the `SpriteTex` folder
- Add the rwav file into your `Music/sfx` folder if using Newer **OR** `Sound/sfx/new` folder if using NSMBWer


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX by the sprite ID)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
