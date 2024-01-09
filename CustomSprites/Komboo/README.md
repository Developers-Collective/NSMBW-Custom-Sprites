# Komboo v1.0.4
*by Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- More sprites code by AboodXD & Nin0 (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites)
- SpriteDataUtils by RedStoneMatt & Synel (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils)
- CheckWater by Nin0 (can be found here: https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/CheckWater)


## Kamek
- If you already have a `Kamek/src/checkWater.S` file, remove the reference to this file in your `Kamek/konbu.yaml` file
- Add `src/konbu.h`, `src/konbu.cpp`, `src/konbuSpawner.cpp` in your `src` folder
- Add `konbu.yaml` in your `Kamek` folder
- Reference the `konbu.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `Konbu` and `KonbuSpawner` for the ProfileIDs and SpriteIDs
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX & YYY by the corresponding sprite IDs)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX & YYY by the corresponding sprite IDs)
	> ⚠️ and don't forget to reference the 2 new classes in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
