# Electricity Block v1.0.0
*by Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Knowledge about how to add GFX to the game
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- More sprites code by AboodXD & Nin0 (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites)
- More SFX code by Asu (converted by Synel for Newer 1.3.X) and how to use it (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSFX)
- SpriteDataUtils by RedStoneMatt and Synel (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils)
- Rotating Active Physics code by Synel (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/RotatingActivePhysics)
- Light & Light Path code by Synel (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/Light)

## Kamek
- Add `src/blockElectricity.cpp` in your `src` folder
- Add `blockElectricity.yaml` in your `Kamek` folder
- Reference the `blockElectricity.yaml` in your `NewerProject.yaml` file
- Reference the new sfx like this in the sfx list (replace XXXX by the correct id):
```cpp
"block_electricity",      // XXXX
```
- Define the following variable (replace XXXX by the correct id):
```cpp
#define SFX_BLOCK_ELECTRICITY XXXX
```
- Add the code for the "more sprite stuff" using `BlockElectricity` both for the ProfileID and SpriteID
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder
- Add the rwav file into your `Music/sfx` folder if using Newer **OR** `Sound/sfx/new` folder if using NSMBWer
- Add the effects from the `Effects/.breff` folder into your `Effect/effect_wnmario.breff` file
- Add the effect textures from the `Effects/.breft` folder into your `Effect/effect_wnmario.breft` file (IA8)

*Note that only yellow and blue effects are currently in the files. If you want to add more, you'll have to add the following BREFF effects (no need for more textures):*
- `block_electricity_prepareXX`
- `block_electricity_sparkXX`
- `block_electricity_thunderXX`

*Replace XX with the color ID so `00` for color = `0`, `01` for color = `1`, etc., `15` for color = `15`.*

- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX by the sprite ID)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
