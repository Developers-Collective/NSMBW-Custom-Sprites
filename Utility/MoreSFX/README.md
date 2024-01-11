# More SFXs v1.3.1
*by Asu, converted by Synel for Newer 1.3.X*

*Orginal code for 1.2.X can be found here: https://github.com/Asu-chan/NSMBWThePranksterComets*

## Kamek
- Add `include/asm_setup.S` in your `include` folder
- Add `src/newSfx.cpp` in your `src` folder
- If you're **not** using Newer as base (so Vanilla or NSMBWer+), change this line in `src/newSfx.cpp`:
```cpp
snprintf(nameWithSound, 79, "/Sound/new/sfx/%s.rwav", SFXNameList[sfxIndex]);
```
to this:
```cpp
snprintf(nameWithSound, 79, "/Sound/stream/sfx/%s.rwav", SFXNameList[sfxIndex]);
```
- Add `src/newSfx.h` in your `src` folder
- Add `src/newSfx.S` in your `src` folder
- Add `include/asm_sfx.S` in your `include` folder
- Add `newSfx.yaml` in your `Kamek` folder
- Don't forget to reference `newSfx.yaml` in `NewerProject.yaml` or whatever file you use to compile, **below** `bugfixes.yaml`
- In `include/sfx.h`, add `#include <asm_sfx.S>` just **below** the `#include <common.h>` line
- In `include/sfx.h`, add `#include <game.h>` just **below** the `#include <asm_sfx.S>` line
- Then, add this function if it doesn't exist in the same file (before the `#endif` line):
```cpp
void playSoundDistance(nw4r::snd::SoundHandle* handle, Vec3 pos, int id, float volume = 1.0, float pitch = 1.0, float distance = 500.0) {
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	if (cwci == 0) return;

	Vec2 dist = {
		cwci->screenCentreX - pos.x,
		cwci->screenCentreY - pos.y
	};
	float v = max<float>(0.0, (1.0 - (sqrtf(dist.x * dist.x + dist.y * dist.y) / distance)) * 1.0);
	if (v <= 0.0) v = 0.0;
	else if (v > 1.0) v = 1.0;

	PlaySoundWithFunctionB4(SoundRelatedClass, handle, id, 1);
	handle->SetVolume(volume * v, 1);
	if (pitch != 1.0) handle->SetPitch(pitch);
}

void setSoundDistance(nw4r::snd::SoundHandle* handle, Vec3 pos, float volume = 1.0, float pitch = 1.0, float distance = 500.0) {
	if (!handle->Exists()) return;
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	if (cwci == 0) return;

	Vec2 dist = {
		cwci->screenCentreX - pos.x,
		cwci->screenCentreY - pos.y
	};
	float v = max<float>(0.0, (1.0 - (sqrtf(dist.x * dist.x + dist.y * dist.y) / distance)) * 1.0);
	if (v <= 0.0) v = 0.0;
	else if (v > 1.0) v = 1.0;

	handle->SetVolume(volume * v, 1);
	if (pitch != 1.0) handle->SetPitch(pitch);
}
```
- Add these addresses to your `kamek_pal.x`:
```cpp
	NewSFXTable = 0x80450000;
	NewSFXIndexes = 0x80460000;
	doneWithNewSFXHax = 0x80283CE4;
	PlaySoundWithFunctionB4_2 = 0x801951E4;
```
- In `exceptionHandler.cpp`, replace this line:
```cpp
const u32 dlcode = 0x80DFBCC0;
```
with this:
```cpp
const u32 dlcode = 0x80D26040;
```
- Compile your code

## Loader
- In the `Loader` folder, you'll find the loader code in `Loader.S`. Edit the file if you want, and then execute the `compile.bat` file. It'll generate a file called `Loader.bin` (for mac and linux users, this bin file is already compiled for the current `Loader.S` file). Replace the `Loader.bin` file from your game files with this new one.

## Riivolution
- Open your riivolution patch and remove these lines:
```xml
		<!-- Inject Newer loader -->
		<!-- EU --> <memory offset="0x80328478" value="80001800" original="8015BC60" />
		<!-- US --> <memory offset="0x80328130" value="80001800" original="8015BB20" />
		<!-- JP --> <memory offset="0x80327E98" value="80001800" original="8015B930" />
		<!-- KR --> <memory offset="0x80334E60" value="80001800" original="8015C060" />
		<!-- TW --> <memory offset="0x80333218" value="80001800" original="8015C060" />
```
- Still in the patch file, replace `<memory offset="0x80001800" valuefile="Loader.bin" />` with these lines:
```xml
		<memory offset="0x800046F0" valuefile="Loader.bin" />
		<memory offset="0x8000406C" value="48000684" original="38000000" />
```

## Adding SFX
First, add `original.rwav` into the `Music/sfx/` folder. (And don't edit it, it's here for a reason)
Les's say we want to add `aNewSFX.rwav` into the game.

First, reference the filename without the extension in `src/newSfx.cpp` in the `SFXNameList` list and add a comment next to it containing its ID just to prevent behing lost each time we add a new sfx. In our case, the last entry is `original`, with ID `1999` so our SFX will be ID `2000` (1999 + 1). So in our case, our `SFXNameList` will look like this:
```cpp
const char* SFXNameList [] = {
	"original",
	"aNewSFX",		//2000
	NULL
};
```
Now we also need to reference the SFX in `include/asm_sfx.S` by adding this line:
```cpp
#define SFX_ZZZZZZZZZZZ XXXX
```
Where `ZZZZZZZZZZZ` is the name of the SFX (without the extension) and `XXXX` is the ID of the SFX. So in our case, it will look like this:
```cpp
#define SFX_A_NEW_SFX 2000
```
You can give it the name you want, but it's better to keep the same name as the SFX name and to respect the format `SFX_` + `NAME_OF_YOUR_SFX` (with each word of the SFX name in uppercase and separated by `_`).


Now for the game part. If you already have the .rwav:
- Add `aNewSFX.rwav` into the `Music/sfx/` folder

*Note: every SFX must be a file ending with .rwav*

If you don't have a .rwav:
- To create a .rwav, you must have a .wav audio file **with only one channel (only mono, not stereo)*.
- Then, open `_TMP.brwsd` from the zip with **brawlcrate** (it comes from the brsar and will only be used to convert files bc of how brawlcrate works, so don't put it in your game files) and in `Audio`, right click on `Audio[0]`, then click on `Replace` and select your .wav file and click on `Okay` when you see the audio popup.
> *Note: If you can't open `_TMP.brwsd`, then use `_TMPFull.brwsd` instead. It's the same file, but with a lot of audio files. It's bigger, but it works.*
- Then, right click on `Audio[0]` and click on `Export`. Name your file when exporting, including the file extension (.rwav).
- You can then close brawlcrate without saving.
- Move your .rwav into the `Music/sfx/` folder.
- That's it!



If there is a compilation or a game problem, tell me, maybe I forgot something somewhere.
