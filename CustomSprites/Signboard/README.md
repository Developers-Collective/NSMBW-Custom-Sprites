# New Signboard v1.1.0
*by Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [Sprite Data Utils](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel


## Kamek
- Add `src/signboard.cpp` in your `src` folder
- Add `signboard.yaml` in your `Kamek` folder
- Reference the `signboard.yaml` in your `NewerProject.yaml`
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder
- Credit G4L for porting the NSMBU Signboard model to NSMBW


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites
- Add the content of `sprites.py` into your `sprites.py` like every other sprites
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add `import math` at the top of your `sprites.py` file if it doesn't exist
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
