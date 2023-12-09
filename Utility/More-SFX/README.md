# Custom SFXs
*by Asu, converted by Synel for Newer 1.3.X*

*Orginal code for 1.2.X can be found here: https://github.com/Asu-chan/NSMBWThePranksterComets*

## Kamek
- Add `include/asm_setup.S` in your `include` folder
- Add the files from `src/k_stdlib/` into your `src/k_stdlib/` folder (if you don't have a `k_stdlib` folder, create one in your `src` folder)
- Add `src/newSfx.cpp` in your `src` folder
- Add `src/newSfx.h` in your `src` folder
- Add `src/newSfx.S` in your `src` folder
- Add `newSfx.yaml` in your `Kamek` folder
- Don't forget to reference `newSfx.yaml` in `NewerProject.yaml` or whatever file you use to compile, **below** `bugfixes.yaml`
- Add these addresses to your `kamek_pal.x`:
```
	NewSFXTable = 0x80450000;
	NewSFXIndexes = 0x80460000;
	doneWithNewSFXHax = 0x80283CE4;
	PlaySoundWithFunctionB4_2 = 0x801951E4;
```
- Compile your code

## Loader
- In the `Loader` folder, you'll find the loader code in `Loader.S`. Edit the file if you want, and then execute the `compile.bat` file. It'll generate a file called `Loader.bin` (for mac and linux users, this bin file is already compiled for the current `Loader.S` file). Replace the `Loader.bin` file from your game files with this new one.

## Riivolution
- Open your riivolution patch and remove these lines:
```
		<!-- Inject Newer loader -->
		<!-- EU --> <memory offset="0x80328478" value="80001800" original="8015BC60" />
		<!-- US --> <memory offset="0x80328130" value="80001800" original="8015BB20" />
		<!-- JP --> <memory offset="0x80327E98" value="80001800" original="8015B930" />
		<!-- KR --> <memory offset="0x80334E60" value="80001800" original="8015C060" />
		<!-- TW --> <memory offset="0x80333218" value="80001800" original="8015C060" />
```
- Still in the patch file, replace `<memory offset="0x80001800" valuefile="Loader.bin" />` with these lines:
```
		<memory offset="0x800046F0" valuefile="Loader.bin" />
		<memory offset="0x8000406C" value="48000684" original="38000000" />
```

## Adding SFX
First, add `original.rwav` into the `Music/sfx/` folder. (And don't edit it, it's here for a reason)
To add SFX into the game, you need to do 2 things:
- Reference a file in `src/music.cpp` in the `SFXNameList` list. If you want to add `aNewSFX.rwav`, your `SFXNameList` will look like this:
```
const char* SFXNameList [] = {
	"original",
	"aNewSFX",		//2000
	NULL
};
```
If you already have the .rwav:
- Add `aNewSFX.rwav` into the `Music/sfx/` folder

*Note: every SFX must be a file ending with .rwav*

If you don't have a .rwav:
- To create a .rwav, you must have a .wav audio file **with only one channel (only mono, not stereo)*.
- Then, open `_TMP.brwsd` from the zip with **brawlcrate** (it comes from the brsar and will only be used to convert files bc of how brawlcrate works, so don't put it in your game files) and in `Audio`, right click on `Audio[0]`, then click on `Replace` and select your .wav file and click on `Okay` when you see the audio popup.
> *Note: If you can't open `_TMP.brwsd`, then use this one: https://github.com/Synell/NSMBW-Custom-Sprites/blob/main/Utility/More-SFX/_TMPFull.brwsd*
- Then, right click on `Audio[0]` and click on `Export`. Name your file when exporting, including the file extension (.rwav).
- You can then close brawlcrate without saving.
- Move your .rwav into the `Music/sfx/` folder.
- That's it!



If there is a compilation or a game problem, tell me, maybe I forgot something somewhere.
