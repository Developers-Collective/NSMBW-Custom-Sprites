# Goombrat v1.0.0
*by Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- More sprites code by Nin0 (can be found here: https://github.com/N-I-N-0/New-Super-Mario-Lost-Worlds)


## Kamek
- Add `src/goombrat.cpp` in your `src` folder
- Add `goombrat.yaml` in your `Kamek` folder
- Reference the `goombrat.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `Goombrat` for the ProfileID and SpriteID
- Compile your code

PS: just in case I forget to do it myself, these functions have been renamed so revert them to what they were before if you want this to work:
`damagePlayer` is `_vf220`
`playRegularStompSound` is `_vf260`
`playSpinStompSound` is `_vf268`
`playYoshiStompSound` is `_vf278`
`visualDropKill` is `_vf148`
`visualDropKillAndCoinJump` is `_vf14C`


## Game
- Add the arc file into the `SpriteTex` folder


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Add the content of `sprites.py` into your `sprites.py` like every other sprites (replace XXX by the sprite ID)
	> ⚠️ and don't forget to reference the 2 new classes in the list at the end ⚠️
- Add the images in the `sprites` folder into your `sprites` folder
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*
