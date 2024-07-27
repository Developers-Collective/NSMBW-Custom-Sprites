# Spike Trap v1.0.2
*by Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0
- [More SFX](https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/MoreSFX) by Asu (converted by Synel for Newer 1.3.X) and how to use it
- [Sprite Data Utils](https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel


## Kamek
- Add `src/spikeTrap.cpp` in your `src` folder
- Add `spikeTrap.yaml` in your `Kamek` folder
- Reference the `spikeTrap.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `SpikeTrap` for the ProfileID and SpriteID
- Reference the new sfx like this in the sfx list (replace XXXX by the correct id):
```cpp
"spike_trap",				// XXXX
```
- Define the following variable (replace XXXX by the correct id):
```cpp
#define SFX_SPIKE_TRAP XXXX
```
- Compile your code


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
