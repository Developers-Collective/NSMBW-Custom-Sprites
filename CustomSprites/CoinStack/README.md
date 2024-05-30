# Coin Stack v1.0.0
*by Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0
- [Sprite Data Utils](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel


## Kamek
- Add `src/coinStack.cpp` in your `src` folder
- Add `coinStack.yaml` in your `Kamek` folder
- Reference the `coinStack.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `coinStack` for the ProfileID and SpriteID
- Add these lines to your `Kamek/kamek_pal.x`:
```cpp
	daPyMng_c_incCoin = 0x80060250;
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
