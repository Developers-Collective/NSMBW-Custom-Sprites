# Wonder Flip Block v1.0.0
*by Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- More sprites code by Nin0 and how to use it (can be found here: https://github.com/N-I-N-0/New-Super-Mario-Lost-Worlds)
- More SFX code by Asu and how to use it (can be found here: https://github.com/Asu-chan/NSMBWThePranksterComets)


## Kamek
Before starting to add files, remove all the files related to your flipblock code if have some as it doesn't replace like in newer but it is added as a new actor/sprite slot (the file should be `flipblock.yaml` and `flipblock.cpp`)
  -> If you still want to replace the newer one, then adapt it to make it work with newer

- Add `src/flipblock.cpp` in your `src` folder
- Add `flipblock.yaml` in your `Kamek` folder
- Reference the `flipblock.yaml` in your `NewerProject.yaml` if not already done
- Reference the new sfx like this in the sfx list (replace XXXX by the correct id):
```cpp
"block_round_sway",      // XXXX
```
- Define the following variable (replace XXXX by the correct id):
```cpp
#define SFX_BLOCK_ROUND_SWAY XXXX
```
- Add the code for the "more sprite stuff" using `FlipBlock` both for the ProfileID and SpriteID
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder
- Add the sfx into your `Music/sfx` folder


## Reggie
Before starting to add files, remove all the files related to your flipblock code if have some as it doesn't replace like in newer but it is added as a new actor/sprite slot (there should be a python class in `sprites.py` with its reference and an entry in `spritedata.xml`)
  -> If you still want to replace the newer one, then adapt it to make it work with newer

- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX by the sprite ID)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
