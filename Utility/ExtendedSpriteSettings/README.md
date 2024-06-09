# Extended Sprite Settings v1.1.1
*by Nin0 & Synel*


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) by AboodXD & Nin0
- [Sprite Data Utils](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils) by RedStoneMatt and Synel


## Kamek
- Add `src/betterActorSpawner.cpp` and `src/extendedSettingTempBlockRemover.cpp` in your `src` folder
- Add `include/rect.h` in your `include` folder
- Add `/extendedSpriteSettings.yaml` in your `Kamek` folder
- Reference the `extendedSpriteSettings.yaml` in your `NewerProject.yaml`
- Add the code for the "more sprite stuff" using `BetterActorSpawner` for the ProfileID and SpriteID
- Add the code for the "more sprite stuff" using `ExtendedSettingTempBlockRemover` for the ProfileID only (so not SpriteID)
- In `include/dCourse.h`, add the include on the top of the file with the other includes (above `class dCourse_c` if none):
```cpp
#include <rect.h>
```
- Still in `include/dCourse.h`, in the `dCourse_c` class, remove the `unk_s` struct and add these structs:
```cpp
struct spriteSettings_s {
	u16 unk1;
	u16 unk2;
	u16 unk3;
	u16 unk4;
	u32 amountOfSettings;
	u32 offsets[];
};

struct spriteSetting_s {
	u32 amountOf4Bytes;
	u32 settings[];
};
```
- Still in `include/dCourse.h`, in the `dCourse_c` class, replace the `unk_s *unk;` line by this one:
```cpp
spriteSettings_s *spriteSettings;
```
- At the top of `include/game.h`, near the other includes, add these if they are not already there:
```cpp
#include <dCourse.h>
#include <profileid.h>
```
- Still in `include/game.h`, remove the `mRect` and `mRect16` classes, they should look like these:
```cpp
class mRect {
	public:
		float x, y, width, height;
};

class mRect16 {
	public:
		short x, y, width, height;
};
```
- Still in `include/game.h`, add this above the `dStageActor_c` class:
```cpp
inline int GetAreaNum(); // Forward declaration

namespace ExtendedSettingTempBlocks
{
	const u8 TimeBeforeDeleting = 2; // 2 frames = 1/30th of a second
	const u8 SettingsSize = 16; // If you change this value, you must also change the size of the UsedSettings bitmask
	u16 UsedSettings;
	dCourse_c::spriteSetting_s* ExtendedSettings[SettingsSize];

	u8 getNextUnusedSlot() {
		for (int i = 0; i < SettingsSize; i++) {
			if ((1 << i) & UsedSettings) continue;
			return i;
		}

		return -1;
	}

	bool isSlotUsed(u8 slot) {
		if (slot >= SettingsSize) return false;
		return (1 << slot) & UsedSettings;
	}

	dCourse_c::spriteSetting_s* getUsedSlot(u8 slot) {
		if (slot >= SettingsSize) {
			OSReport("Invalid slot %d\n", slot);
			return 0;
		}
		return ExtendedSettings[slot];
	}

	dCourse_c::spriteSetting_s* setSlot(u8 slot, u32 settingBlocks[]) {
		if (slot >= SettingsSize) {
			OSReport("Invalid slot %d\n", slot);
			return 0;
		}

		if (isSlotUsed(slot)) {
			OSReport("Slot %d is already in use\n", slot);
			return 0;
		}

		u32 size = sizeof(settingBlocks);

		dCourse_c::spriteSetting_s* data = (dCourse_c::spriteSetting_s*)new u8[size + 4];
		data->amountOf4Bytes = size;
		memcpy(data->settings, settingBlocks, size);

		ExtendedSettings[slot] = data;
		UsedSettings |= 1 << slot;

		CreateActor(
			(Actors)ProfileId::ExtendedSettingTempBlockRemover,
			(u32)slot,
			&(Vec){ClassWithCameraInfo::instance->screenCentreX, ClassWithCameraInfo::instance->screenCentreY, 0},
			0,
			0
		);

		return ExtendedSettings[slot];
	}

	void clearSlot(u8 slot) {
		if (slot >= SettingsSize) {
			OSReport("Invalid slot %d\n", slot);
			return;
		}

		if (!isSlotUsed(slot)) {
			OSReport("Slot %d is not in use\n", slot);
			return;
		}

		delete ExtendedSettings[slot];
		ExtendedSettings[slot] = 0;
		UsedSettings &= ~(1 << slot);
	}
} // namespace ExtendedSettingTempBlocks
```
- Still in `include/game.h`, only if located after the `ExtendedSettingTempBlocks` namespace, find the `ClassWithCameraInfo` class and move it above the previously added code (so above `namespace ExtendedSettingTempBlocks`)
- Still in `include/game.h`, add this in the `dStageActor_c` class:
```cpp
private:
	dCourse_c::spriteSetting_s* getSpriteSettings() {
		dCourse_c::spriteSetting_s *spriteSettings = 0;

		if (this->settings >= 0x80000000) {
			spriteSettings = (dCourse_c::spriteSetting_s*)(this->settings);
		}

		else {
			dCourse_c *area = dCourseFull_c::instance->get(GetAreaNum());
			if (this->settings >= area->spriteSettings->amountOfSettings) return 0;

			u32 addr = area->spriteSettings->offsets[this->settings];
			spriteSettings = (dCourse_c::spriteSetting_s*)((u32)&area->spriteSettings->unk1 + addr);
		}

		return spriteSettings;
	}

public:
	/**
		* @brief Get the amount of block settings for this actor
		* 
		* @return u32 The amount of block settings for this actor
		*/
	u32 getBlockSettingCount() {
		dCourse_c::spriteSetting_s *spriteSettings = this->getSpriteSettings();
		return spriteSettings->amountOf4Bytes;
	}

	/**
		* @brief Get the Block Settings object
		* 
		* @param blockID The ID of the block to get settings for (Block 1 in reggie is block ID 0, block 2 is block ID 1, etc.)
		* @return u32 The settings for the specified block ID, 0 if the block ID is invalid
		*/
	u32 getBlockSettings(u32 blockID) {
		dCourse_c::spriteSetting_s *spriteSettings = this->getSpriteSettings();
		if (blockID >= spriteSettings->amountOf4Bytes) return 0;

		return spriteSettings->settings[blockID];
	}


	static dStageActor_c *createExtended(u16 type, u32 settingBlocks[], VEC3 *pos, S16Vec *rot, u8 layer) {
		dCourse_c *area = dCourseFull_c::instance->get(GetAreaNum());

		u8 nextSlot = ExtendedSettingTempBlocks::getNextUnusedSlot();
		if (nextSlot == -1) {
			OSReport("No more slots for temp extended settings\n");
			return 0;
		}

		u32 settings = (u32)ExtendedSettingTempBlocks::setSlot(nextSlot, settingBlocks);

		dStageActor_c *actor = dStageActor_c::create(
			type,
			settings,
			pos,
			rot,
			layer
		);

		return actor;
	}
	static dStageActor_c *createExtendedChild(u16 type, dStageActor_c *parent, u32 settingBlocks[], VEC3 *pos, S16Vec *rot, u8 layer) {
		dCourse_c *area = dCourseFull_c::instance->get(GetAreaNum());

		u8 nextSlot = ExtendedSettingTempBlocks::getNextUnusedSlot();
		if (nextSlot == -1) {
			OSReport("No more slots for temp extended settings\n");
			return 0;
		}

		dStageActor_c *actor = dStageActor_c::createChild(
			type,
			parent,
			(u32)ExtendedSettingTempBlocks::setSlot(nextSlot, settingBlocks),
			pos,
			rot,
			layer
		);

		return actor;
	}

	static dStageActor_c *createExtended(Actors type, u32 settingBlocks[], VEC3 *pos, S16Vec *rot, u8 layer) {
		return dStageActor_c::createExtended((u16)type, settingBlocks, pos, rot, layer);
	}
	static dStageActor_c *createExtendedChild(Actors type, dStageActor_c *parent, u32 settingBlocks[], VEC3 *pos, S16Vec *rot, u8 layer) {
		return dStageActor_c::createExtendedChild((u16)type, parent, settingBlocks, pos, rot, layer);
	}
```

- Add these lines to your `Kamek/kamek_pal.x`:
```cpp
	CreateActor__F6ActorsUiP7Point3dPvc = 0x80064610;
```
- Compile your code

*PS: You won't be able tell if it'll work before having a sprite that uses it.*


## Reggie
- Add the content of `spritedata.xml` into your `spritedata.xml` like every other sprites (replace XXX by the sprite ID)
- Restart reggie to apply the changes


## How to Use the Extended Sprite Settings

### Reggie

> [!NOTE]\
> For now, reggie's repo owners didn't approve the pull request, so you won't be able to use this feature with the official version of Reggie. However, you can use this [custom version of Reggie](https://github.com/Synell/Reggie-Next/tree/sprite-settings) to use this feature while waiting for the official version to be updated.

> [!WARNING]\
> Opening old levels with the new version of Reggie will work fine. However, levels created or saved with the new version of Reggie will not work with old versions of Reggie if they contain extended sprites. If they don't contain extended sprites, they should work fine with old versions of Reggie.

#### XML

In the `sprite` tag, set the `extended` attribute to `true` to enable the extended settings (e.g. `<sprite id="XXX" extended="true">`)

> [!NOTE]\
> The max number of blocks is **very high (4,294,967,296 in theory)** so we can say it's unlimited (even if the wii will crash way before you could even reach this number in real situations but that's an other subject).

Each block has 8 nybbles (4 bytes) that you can use to store additional settings. The number of blocks used by your sprite is automatically calculated by Reggie. You can add as many blocks as you need.

> [!IMPORTANT]\
> Avoid skipping blocks, it's not a good idea, as it will waste memory if you don't use them all.

Then, for every setting you want to add, add a `block` attribute to your setting tag (e.g. `<value nybble="1" block="2">`)

> [!NOTE]\
> To access the nybbles 1 to 4 and 13 to 16, use 0 as the block value (e.g. `<value nybble="1" block="0">`)

> [!IMPORTANT]\
> **If your sprite is extended, don't use nybbles 5 to 12 of block 0, they are reserved by this feature**

*You can check out the Better Actor Spawner's `spritedata.xml` for an example*

If you want to use the `requirednybble` and `requiredvalue` attributes, you should use the `requiredblock` attribute to specify the block to check (e.g. `<value nybble="1" block="2" requirednybble="1" requiredvalue="1" requiredblock="0">`)

> [!NOTE]\
> The multi-value format stays the same (e.g. `<value nybble="1" block="2" requirednybble="1,2" requiredvalue="1,0" requiredblock="0,1">`)

You should then see the extended settings in the sprite settings window in Reggie:\
<img src="https://raw.githubusercontent.com/Synell/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/ExtendedSpriteSettings/ExtendedSpriteSettings.png" />

For comparison, here is the old sprite settings window in Reggie:\
<img src="https://raw.githubusercontent.com/Synell/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/ExtendedSpriteSettings/VanillaSpriteSettings.png" />


#### Python

The old method of getting the settings is still available for non-extended sprites OR can be used to access nybbles 1 to 4 and 13 to 16 for extended sprites:
```python
# Get the first 4 bits (from the right) of the second byte, so nybble 4
self.parent.spritedata[1] & 0xF
```
However, to access the extended settings, you can use the `blocks` property:
```python
# Get the first 4 bits (from the right) of the second byte of the third block, so nybble 4 of block 3
self.parent.spritedata.blocks[2][1] & 0xF
```
`blocks` is a list of bytes object (each bytes object is 4 bytes long aka 8 nybbles).

> [!NOTE]\
> **Quick reminder**\
> In Python and C++, indexes start at 0 (not 1), so the first block is at index 0, the second block is at index 1, etc. Same goes for the bytes object, the first byte is at index 0, the second byte is at index 1, etc.

### Kamek

When creating a new sprite, you can access the block settings using the `getBlockSettings` method:
```cpp
u32 firstBlock = this->getBlockSettings(0); // Get the settings of the first block
u32 secondBlock = this->getBlockSettings(1); // Get the settings of the second block
// etc.
```

> [!NOTE]\
> If the block ID is invalid (out of range), the method will return 0

> [!IMPORTANT]\
> When using an extended sprite, don't use the `settings` member, as it's not something you should use anymore as it's used by the extended settings. Use the `getBlockSettings` method instead.

If, for some reason, you need to get the amount of blocks used by the sprite, you can use the `getBlockSettingCount` method:
```cpp
u32 blockCount = this->getBlockSettingCount(); // Get the amount of blocks used by the sprite
```


### Q&A

#### Can I convert a non-extended sprite to an extended sprite or vice versa?

Yes, you can.
If you want to convert a non-extended sprite to an extended sprite, just set to `true` the `extended` attribute to the sprite tag in the XML file and add the blocks you need, and replace all the `this->settings` from your code by the new method with the corresponding values.

If you want to convert an extended sprite to a non-extended sprite, just set to `false` the `extended` attribute (or remove it) to the sprite tag in the XML file and remove the blocks you don't need, and replace all the `this->getBlockSettings` from your code by the old method with the corresponding values.

> [!IMPORTANT]\
> __I'd recommand using the old method (so non-extended) if you don't need the extra nybbles__, as it's more optimized memory-wise. Also, **don't convert a sprite to an extended sprite if it's already used somewhere in your levels**, as it will likely break the levels using it.


#### Can I use non-extended sprites and extended sprites in the same level?

Yes, you can. You can use both types of sprites in the same level without any problem.

#### How do I spawn an extended actor within Reggie like with the Actor Spawner?

You can use the `Better Actor Spawner` sprite (included with this feature) to spawn extended actors within Reggie.


#### Can I use the extended settings with the old `Actor Spawner`?

No, you can't. You have to use the `Better Actor Spawner` sprite to use the extended settings.


#### Can I spawn extended actors from my code directly?

Yes, you can. You can use the `createExtended` and `createExtendedChild` methods (from the `dStageActor_c` class) to spawn extended actors from your code.


#### Can I use the extended settings when replacing a vanilla sprite?

Yes, you can. You can use the extended settings when replacing a vanilla sprite as if it was a custom sprite.

However, I'd recommend making a new sprite using the [More sprites](https://github.com/Synell/NSMBW-Custom-Sprites/releases/tag/MoreSprites) feature from AboodXD & Nin0 to avoid replacing vanilla sprites.



--------------------------------------------

If there is a compilation or a game problem, tell Synel or Nin0, maybe we forgot something somewhere.
