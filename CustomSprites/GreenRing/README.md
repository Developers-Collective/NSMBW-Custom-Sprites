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
- Add `include/greenringreward.h` in your `include` folder
- Add `include/dMultiMng_c.h`, and `include/timekeeper.h` in your `include` folder if you do not have them already
- Add `GreenRing.yaml` in your `Kamek` folder
- Reference the `GreenRing.yaml` in your `NewerProject.yaml` or `NSMBWerProject.yaml` if you're on NSMBWer+
- Reference the new sfx like this in the sfx list (replace XXXX with the correct ids):
```cpp
"SE_OBJ_GREEN_TIMER",      // XXXX
```
- Define the following variable (replace XXXX with the correct id):
```cpp
#define SFX_SE_OBJ_GREEN_TIMER XXXX
```
- Add the code for the "more sprite stuff" using `EN_GREENRING` and `EN_GREENCOINU` for the ProfileID and SpriteID
- Add the code for the "more sprite stuff" using `GreenRingReward` for ONLY the ProfileID

- Open `src/levelspecial.cpp` and add this at the very end of the file:
```cpp
#undef time
```
- If you would like to add the Hammer Suit as an option for an item reward then choose which base you are using and proceed with that guide. If you do not want the Hammer Suit as an option for an item reward then ignore this and proceed on.


<details>
  <summary>Newer 1.3.0</summary>
  <p></p>

- Open `poweruphax.cpp` and add this above `void ThwompHammer(dEn_c *thwomp, ActivePhysics *apThis, ActivePhysics *apOther) {`
```cpp
extern "C" int GetHammerPowerupRelatedValue(int playerID) {
	dAcPy_c player = dAcPy_c::findByID(playerID);
	if (!player)
		return 0;

	int powerup =(int)((u8)player + 0x1090);
	if (powerup != 0 && powerup != 3)
		return 1;
	if (powerup == 3)
		return 2;
	return 0;
}
```
- Open `poweruphax.S` and add this below `.extern returnFromPowerupSoundChange`
```cpp
.extern GetHammerPowerupRelatedValue
```
- In `poweruphax.S` replace
```cpp
.global SetHammerToEnItemDCA
SetHammerToEnItemDCA:
	bl daEnItem_c__GetWhetherPlayerCanGetPowerupOrNot
	cmpwi r3, 1
	bne DontSetHammer
	
	li r0, 5
	sth r0, 0xDCA(r31)
```
- With this
```cpp
.global SetHammerToEnItemDCA
SetHammerToEnItemDCA:
	lwz r3, 0xDB0(r31)
	cmpwi r3, 0
	bne _setHammerDCA

	lwz r3, 0xD90(r31)
	bl GetHammerPowerupRelatedValue
	cmpwi r3, 1
	bne DontSetHammer
```
- Still In `poweruphax.S` replace
```cpp
.global SetHammerToEnItemDCASpecial
SetHammerToEnItemDCASpecial:
	cmplwi r4, 0x19
	b _not19
	li r0, 0xD
	sth r0, 0xDCA(r3)
_not19:
	cmplwi r4, 6
	bnelr
	li r0, 5
	sth r0, 0xDCA(r3)
	blr
```
- With this
```cpp
.global SetHammerToEnItemDCASpecial
SetHammerToEnItemDCASpecial:
	cmplwi r4, 0x19
	bne _checkHammer
	li r0, 0xD
	sth r0, 0xDCA(r3)
	blr

_checkHammer:
	cmplwi r4, 6
	bnelr

	mr r6, r3
	lwz r0, 0xDB0(r3)
	cmpwi r0, 0
	bne _setHammer

	lwz r3, 0xD90(r6)
	bl GetHammerPowerupRelatedValue
	cmpwi r3, 1
	bnelr

_setHammer:
	li r0, 5
	sth r0, 0xDCA(r6)
	blr
```

</details>


<details>
  <summary>NSMBWer+</summary>
  <p></p>

- Open `poweruphax.cpp` and add this above `void ThwompHammer(dEn_c *thwomp, ActivePhysics *apThis, ActivePhysics *apOther) {`
```cpp
extern "C" int GetHammerPowerupRelatedValue(int playerID) {
	dAcPy_c player = dAcPy_c::findByID(playerID);
	if (!player)
		return 0;

	int powerup =(int)((u8)player + 0x1090);
	if (powerup != 0 && powerup != 3)
		return 1;
	if (powerup == 3)
		return 2;
	return 0;
}
```
- In `poweruphax.S` replace
```cpp
bl daEnItem_c__GetWhetherPlayerCanGetPowerupOrNot
```
- With this
```cpp
	lwz r3, 0xDB0(r31)
	cmpwi r3, 0
	bne _setHammerDCA

	lwz r3, 0xD90(r31)
	bl GetHammerPowerupRelatedValue
```
- In `poweruphax.S` after the line
```cpp
.extern returnFromWMSubPlayerDraw
```
- Add This
```cpp
.extern GetHammerPowerupRelatedValue
```
- Still In `poweruphax.S` replace
```cpp
_not19:
	cmplwi r4, 6
	bnelr
	li r0, 5
	sth r0, 0xDCA(r3)
	blr
```
- With this
```cpp
_not19:
	cmplwi r4, 6
	bnelr

        mr r6, r3
        lwz r0, 0xDB0(r3)
        cmpwi r0, 0
        bne _setHammer

        lwz r3, 0xD90(r6)
        bl GetHammerPowerupRelatedValue
        cmpwi r3, 1
        bnelr

_setHammer:
        li r0, 5
        sth r0, 0xDCA(r6)
        blr
```

</details>


- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder or `Object` if you're on NSMBWer+
- Add the sfx into your `Music/sfx` folder or `Sound/stream/sfx` if you're on NSMBWer+
- Add the effects from the `Effects` folder into your `Effect/effect_wnmario.breff` file using [Actually-Working-BREFF-Converter](https://github.com/CLF78/Actually-Working-BREFF-Converter)


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX & YYY with the sprite IDs)
- If you are not adding the Hammer Suit as an option for an item reward, then delete this line in the `Green Ring` sprite data.
```cpp
<entry value="4">Hammer Suit</entry>
```
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX & YYY with the sprite IDs)
	> ⚠️ and don't forget to reference the new class in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
