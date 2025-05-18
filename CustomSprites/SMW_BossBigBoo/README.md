# BossBigBoo (SMW) v1.0
*by ReaZ0n23*

## Demo
https://youtu.be/VCa9HPNgNM8&t=278 (Jump to YouTube)  

## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.2.X (changes may have to be made for Newer 1.3X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0

## Kamek
- Add `src/smw_BossBigBoo.cpp` in your `src` folder
- Add `smw_BossBigBoo.yaml` in your `Kamek` folder
- Reference the `smw_BossBigBoo.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `SMW_BOSSBIGBOO` for the ProfileID and SpriteID
- Add these addresses to your kamek_base.x (or kamek_pal.x):
```
	m_goalType = 0x8042a4dc;
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
- v1.0　　
> Known issues: If you were using Newer1.30 (CW compiler), cannot kill child boos when defeated this sprite. So If you want to use this in the environment, plz delete the line 683 (sa->Delete(1);	// FIX ME).
And the style option is not completed.  

  
  
*If I forgot some stuff or if you find a bug, be free to let me know*
