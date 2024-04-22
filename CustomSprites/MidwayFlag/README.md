# Better Midway Flag v0.9.8
*by Synel*


## Requirements
- [Check Water](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/CheckWater) by Nin0


## Kamek / Game
- If you already have a `Kamek/src/checkWater.S` file, remove the reference to this file in your `Kamek/midwayFlag.yaml` file
- Copy the files from the `Kamek` folder into your `Kamek` folder (if some files already exist, find a way to merge them)
- Add these lines to your `Kamek/kamek_pal.x`:
```cpp
	midwayFlagOnCreate = 0x807e2130;
	midwayFlagOnExecute = 0x807e2410;
	midwayFlagPowerUp = 0x807e2ca0;
	StateID_Wait__15daChukanPoint_c = 0x80997d98;
	StateID_SeaWait__15daChukanPoint_c = 0x80997e18;
	continueOriginalCheckWater = 0x80075274;
	mGameFlag__7dInfo_c = 0x8042a260;
```
- Don't forget to reference `midwayFlag.yaml` in `NewerProject.yaml` or whatever file you use to compile
- Compile your code

## Reggie Next
- In your `spritedata.xml` file, replace the code of the Midway Flag (188) with the code from `ReggieNext/spritedata.xml` (DON'T REPLACE THE FILE)
- In your `sprites.py` file, replace the code of the Midway Flag (188) with the code from `ReggieNext/sprites.py`(DON'T REPLACE THE FILE)
- Add `import math` at the top of your `sprites.py` file if it doesn't exist
- In your `sprites.py` file, add this line at the top at the file if it doesn't exist: `import math`
- In your `sprites/` folder, add the images from `ReggieNext/sprites/` and replace them if they already exist
- Restart Reggie Next

## Missing features
- Bouncy cloud collision
- Spring block collision
- Maybe more things if I get some ideas

## Special Thanks
- Nin0 for the help <3
- B1 Gaming and Jacopo Plays for finding bugs and fixing minor issues with the reggie patch
- Ryguy for finding the source of the bug where the midway crashes coin battle

*Note: I needed to move settings so it's possible that your previous checkpoints aren't set to the exact same settings they had before.*
