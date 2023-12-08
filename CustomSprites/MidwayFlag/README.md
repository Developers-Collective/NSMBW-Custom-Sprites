# Better Midway Flag v0.9.4 (by Synel)

## Game / Code Installation
- Copy the files from the `Kamek` folder into your `Kamek` folder (if some files already exist, find a way to merge them lol)
- Add these lines to your `Kamek/kamek_pal.x`:
	```
	midwayFlagOnCreate = 0x807e2130;
	midwayFlagOnExecute = 0x807e2410;
	midwayFlagPowerUp = 0x807e2ca0;
	StateID_Wait__15daChukanPoint_c = 0x80997d98;
	StateID_SeaWait__15daChukanPoint_c = 0x80997e18;
	continueOriginalCheckWater = 0x80075274;
	```
	*Note: check that some of these addresses dont't already exist in your .x (to avoid duplicates, otherwise your game will crash)*
- Don't forget to reference `midwayFlag.yaml` in `NewerProject.yaml` or whatever file you use to compile

## Reggie Next Installation
- In your `<your_reggie_next_location>/reggiedata/patches/<your_mod_name>/spritedata.xml` file, replace the code of the Midway Flag (188) with the code from `ReggieNext/spritedata.xml` (DON'T REPLACE THE FILE)
- In your `<your_reggie_next_location>/reggiedata/patches/<your_mod_name>/sprites.py` file, replace the code of the Midway Flag (188) with the code from `ReggieNext/sprites.py`(DON'T REPLACE THE FILE)
- In your `<your_reggie_next_location>/reggiedata/patches/<your_mod_name>/sprites.py` file, add this line at the top at the file if it doesn't exist: `import math`
- In your `<your_reggie_next_location>/reggiedata/patches/<your_mod_name>/sprites/` folder, add the images from `ReggieNext/sprites/` and replace them if they already exist
- Restart Reggie Next

## Missing functionalities
- Bouncy cloud collision
- Spring block collision
- Maybe more things if I get some ideas

## Special Thanks
- Nin0 for the help <3
- B1 Gaming and Jacopo Plays for finding bugs and fixing minor issues with the reggie patch

*Note: I needed to move settings so it's possible that your previous checkpoints aren't set to the exact same settings they had before.*
