# Green Ring v1.0.0
*by SilverBuckeye, TheMarioMan, & LucasD10*

*Credits to NSMLW Team for some of the code and spritedata. Thank you MandyIGuess & Ryguy for the help*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Knowledge about how to add GFX to the game
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0
- [Sprite Data Utils](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel
- [More SFX](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSFX) by Asu (converted by Synel for Newer 1.3.X) and how to use it


## Kamek
- Add `src/GreenRingManager.cpp` in your `src` folder
- Add `include/greenringreward.h`, `include/dMultiMng_c.h`, and `include/timekeeper.h` in your `include` folder
- Add `GreenRing.yaml` in your `Kamek` folder
- Reference the `GreenRing.yaml` in your `NewerProject.yaml` or `NSMBWerProject.yaml` if you're on NSMBWer+
- Reference the new sfx like this in the sfx list (replace XXXX with the correct ids):
```cpp
"SE_OBJ_GREEN_TIMER",      // XXXX
```
- Define the following variable (replace XXXX with the correct ids):
```cpp
#define SFX_SE_OBJ_GREEN_TIMER XXXX
```
- Add the code for the "more sprite stuff" using `EN_GREENRING` and `EN_GREENCOINU` for the ProfileID and SpriteID
- Add the code for the "more sprite stuff" using `GreenRingReward` for ONLY the ProfileID

- Open `src/levelspecial.cpp` and add this at the very end of the file:
```cpp
#undef time
```
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder or `Object` if you're on NSMBWer+
- Add the sfx into your `Music/sfx` folder or `Sound/stream/sfx` if you're on NSMBWer+
- Add the effects from the `Effects` folder into your `Effect/effect_wnmario.breff` file using [Actually-Working-BREFF-Converter](https://github.com/CLF78/Actually-Working-BREFF-Converter)


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX & YYY with the sprite IDs)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX & YYY with the sprite IDs)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


## Hammer Suit
- If you want to add the Hammer Suit as an option for an Item Reward, go to `Adding Hammer Suit` and choose the base you are using
- Add `src/poweruphax.h`, `src/poweruphax.S`, and `src/poweruphax.cpp` in your `src` folder and replace the files or adjust those files
- Go to spritedata.xml and find the `Green Ring` sprite. Then add `<entry value="4">Hammer Suit</entry>` underneath `<entry value="3">Penguin Suit</entry>`
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
