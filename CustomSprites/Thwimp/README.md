# Thwimp v1.2.3 <img src="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/blob/main/CustomSprites/Thwimp/Reggie/sprites/Koton_0.png" width="32" align="center" />
*Made by **ReaZ0n23**, 3DModel by **B1 Gaming**, Proxy pushing by wakanameko*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.2.X or 1.3X
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0


## Kamek
- Add `src/koton.cpp` in your `src` folder
- Add `koton.yaml` in your `Kamek` folder
- Reference the `koton.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `koton` for the ProfileID and SpriteID
- Add these addresses to your kamek_pal.x (or kamek_base.x):
```
	DisplayScoreAddPopup = 0x800B3540;
	AddScore = 0x80060690;
```
- Compile your code


## Game
- Add the arc file into the `SpriteTex` folder


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX by the sprite ID)
	> ⚠️ and don't forget to reference a new classes in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


## Change Log
 - v1.0.0  
   First release
 - v1.1.0  
   Added the Jumping Distance option. So you can change the jump distance with Reggie!.
 - v1.2.0  
   Supported the Newer1.30 (CW compiler).
 - v1.2.1  
   Fixed an issue where koton would slide sideways when the jump height was 0.  
   Changed codes that check Reggie settings to be enabled only in debug mode.  
 - v1.2.2  
   Deleted the line "#inlude <wakanalib.h>"
 - v1.2.3  
   Replaced Japanese comments with English comments has same meaning.  
  
*If I forgot some stuff or if you find a bug, be free to let me know*
