# Background Anim Player v1.1.1
*by Synel*


## Supported Formats

| Format                | Description           | Supported?            |
| :-------------------: | :-------------------- | :-------------------: |
| **CHR**               | CHR animations are set up by animating bone movement (translation, rotation, or scaling). This is the most commonly used format in NSMBW, underpinning most 3D animation. | ✔️                     |
| **SRT**               | SRT animations are set up by animating texture transforms (translation, rotation, or scaling). This is used in NSMBW for effects such as moving water and clouds. | ✔️                     |
| **PAT**               | PAT animations are set up by animating textures. This is used in NSMBW for effects such as changing power-up outfits and facial expressions. | ✔️                     |
| **VIS**               | VIS animations are set up by animating the visibility of bones. This is used in NSMBW for effects such as the clown car's varying 3D facial expressions. | ✔️                     |
| **CLR**               | CLR animations are set up by animating the contents of certain color registers. These include the registers in lighting channels and the standard & constant registers on every material. This is used in NSMBW for effects such as bosses flashing red when they take damage. | ✔️                     |
| **SHP**               | SHP animation ares set up by animating the morphology of the 3D model. This is genually used in flags. To my knowledge this is not used in NSMBW | ❌                     |
| **SCN**               | SCN animation ares set up by animating the scene objects. This means editing lights, fog and cameras. | ❌                     |


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0


## Kamek
- Add the files from the `src` in your `src` folder
- Add the files from the `include` in your `include` folder
- Add `backgroundAnimPlayer.yaml` in your `Kamek` folder
- Reference the `backgroundAnimPlayer.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `BackgroundAnimCHRPlayer`, `BackgroundAnimSRTPlayer`, `BackgroundAnimPATPlayer`, `BackgroundAnimVISPlayer`, `BackgroundAnimCLRPlayer`, `BackgroundAnimSHPPlayer` and `BackgroundAnimSCNPlayer` for the ProfileIDs and SpriteIDs
- Add this class under the `mHeapAllocator_c` class if it doesn't exist in `include/g3dhax.h`:
```cpp
class dHeapAllocator_c : public mHeapAllocator_c {
    public:
        dHeapAllocator_c();
        ~dHeapAllocator_c();
};
```
- Add these addresses to your `kamek_pal.x` if they don't exist:
```cpp
	setFrameForEntry__Q23m3d8anmClr_cFfi = 0x80166430;
	setUpdateRateForEntry__Q23m3d8anmClr_cFfi = 0x80166450;
	isEntryAnimationDone__Q23m3d8anmClr_cFi = 0x80166460;
	doDelete__9daFarBG_cFv = 0x80116000;
	prepareIndividualBackgroundObject__9daFarBG_cFUsPQ29daFarBG_c20BackgroundModelThingb = 0x80116570;
	UpdateAnim__9daFarBG_cFv = 0x80118f00;
```
- Compile your code


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace AAA, BBB, CCC, DDD and EEE by the sprite IDs)
- Restart reggie to apply the changes


*If I forgot some stuff or if you find a bug, be free to let me know*


## Changelog


### v1.1.0

#### Hotfix
- Fixed BG IDs > 0x8000 causing a game crash due to some negative values when signed


### v1.0.0

Official Release
