# More Sprites v1.0.0
*by AboodXD, updated by Nin0*

*Original code can be found here: https://github.com/N-I-N-0/New-Super-Mario-Lost-Worlds*

## Kamek
- Add `include/asm_setup.S` in your `include` folder
- Add the files from `src/k_stdlib/` into your `src/k_stdlib/` folder (if you don't have a `k_stdlib` folder, create one in your `src` folder)
- Add `src/profile.cpp` in your `src` folder
- Add `src/profileid.h` in your `src` folder
- Add `src/profile.S` in your `src` folder
- Add `profile.yaml` in your `Kamek` folder
- Add `include/profile.h` in your `include` folder
- Add `include/asm_profiles.S` in your `include` folder
- Don't forget to reference `profile.yaml` in `NewerProject.yaml` or whatever file you use to compile
- Add these addresses to your `kamek_pal.x`:
```
	originalSprites = 0x8030a340;
	originalProfiles = 0x8076a748;
	ObjectProfileList = 0x8042a698;
	spriteFiles = 0x8031ab4c;
	profileNames = 0x80320b58;
```
- Compile your code

*Note that this code will not compile until you add your first new sprite, due to the list being empty if no custom sprites are found.*

## Add a new sprite
Let's say you want to add a sprite with a class named `dMySprite_c` in a file named `mySprite.cpp` with a filelist named `MySpriteFileList`.

First thing first, your YAML file should look like this:
```yaml
---
source_files: [../src/mySprite.cpp]
hooks: []
```
As you can see, we don't have any hooks, because we don't need any. We're just adding a new sprite, not modifying an existing one.

*PS: Don't forget to reference your YAML to `NewerProject.yaml` or whatever file you use to compile, below `profile.yaml`*

Open `src/profileid.h`. There should be a `// Start of custom entries` line and a `Beginning of new entries`.

The first one is for sprite IDs, the second one is for actor IDs.

Let's choose a name for our new sprite. Because our class name is `dMySprite_c`, let's call it `MySprite`. This name is our **sprite name**.

*Make sure the name doesn't contain any spaces or special characters and doesn't exist yet in both lists.*

Go to the end of the sprite IDs list and get the last sprite ID of the list. In this case, because we never added a sprite before, it's `AC_BOSS_KOOPA_BG_EFFECT = 482,` so its sprite ID is 482. So, our sprite ID will be 483 (482 + 1).

So let's add our sprite ID to the list by adding this line:
```cpp
	MySprite = 483,
```
so the end of the list should look like this:
```cpp
	AC_BOSS_KOOPA_BG_EFFECT = 482,

	// Start of custom entries
	MySprite = 483,

	Num
};
```

Now, let's add the actor ID. Go to the end of the actor IDs list and get the last actor ID of the list. In this case, because we never added an actor before, it's `LASTACTOR, // 749` so its actor ID is 749. So, our actor ID will be 750 (749 + 1).

So let's add our actor ID to the list by adding this line:
```cpp
	MySprite = 750,
```
so the end of the list should look like this:
```cpp
	LASTACTOR, // 749

	// Beginning of new entries
	MySprite = 750,

	Num
};
```

Now, let's reference our new actor ID to the `asm_profiles.S` list. Open `include/asm_profiles.S`.

Go to the end of the file and add this line by replace WWWWWW with your sprite name and XXX with your **actor ID**:
```asm
#define WWWWWW PROFILEID_ADJUST_C+PROFILEID_ADJUST_KW+XXX
```

In our case, our sprite name is `MySprite` and our actor ID is 750, so we'll add this line:
```asm
#define MySprite PROFILEID_ADJUST_C+PROFILEID_ADJUST_KW+750
```
so in our case the end of the file should look like this:
```asm
#define LASTACTOR_STAGE PROFILEID_ADJUST_C+PROFILEID_ADJUST_KW+748
#define LASTACTOR PROFILEID_ADJUST_C+PROFILEID_ADJUST_KW+749

#define MySprite PROFILEID_ADJUST_C+PROFILEID_ADJUST_KW+750
```

Now, let's configure the sprite. Open your sprite filename, in our case, it's `src/mySprite.cpp`.
First, we need to change the build method as it only accepts `dActor_c*` as an argument. We need to change both the method declaration and the method definition.

For the method declaration, change this:
```cpp
public: static dMySprite_c *build();
```
to this:
```cpp
public: static dActor_c *build();
```
*Make sure the method is public and static.*

For the method definition, change this:
```cpp
dMySprite_c *dMySprite_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dMySprite_c));
	return new(buffer) dMySprite_c;
}
```
to this:
```cpp
dActor_c *dMySprite_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dMySprite_c));
	return new(buffer) dMySprite_c;
}
```

Now, let's configure the sprite. Just below the method definition, add this:
```cpp
const SpriteData MySpriteSpriteData = { ProfileId::MySprite, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile MySpriteProfile(&dMySprite_c::build, SpriteId::MySprite, &MySpriteSpriteData, ProfileId::MySprite, ProfileId::MySprite, "MySprite", MySpriteFileList, 0);
```
*Make sure to replace every `MySprite` with your sprite name and `MySpriteFileList` with your filelist name.*

Let's explain what each SpriteData value means, from left to right:
```cpp
const SpriteData MySpriteSpriteData = { ProfileId::MySprite, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
```
- `ProfileId::MySprite`: The actor ID of the sprite. It's the same as the actor ID we added to `profileid.h` (the `750`).
- `8`: The sprite's position offset on the X axis.
- `-8`: The sprite's position offset on the Y axis.
- `0`: The spawn range offset on the X axis.
- `0`: The spawn range offset on the Y axis.
- `0x100`: The sprite range size on the X axis.
- `0x100`: The sprite range size on the Y axis.
- `0`: Don't know what it does, leave it at 0.
- `0`: Don't know what it does, leave it at 0.
- `0`: Don't know what it does, leave it at 0.
- `0`: Don't know what it does, leave it at 0.
- `0`: The spawning flags. Don't know the values, so you can leave it at 0.

Now, let's explain what each Profile value means, from left to right:
```cpp
Profile MySpriteProfile(&dMySprite_c::build, SpriteId::MySprite, &MySpriteSpriteData, ProfileId::MySprite, ProfileId::MySprite, "MySprite", MySpriteFileList, 0);
```
- `&dMySprite_c::build`: The build method of the sprite. It's the same as the method we changed earlier.
- `SpriteId::MySprite`: The sprite ID of the sprite. It's the same as the sprite ID we added to `profileid.h` (the `483`).
- `&MySpriteSpriteData`: The sprite data of the sprite. It's the reference to the SpriteData we created earlier.
- `ProfileId::MySprite`: The execution order of the sprite. It's the same as the actor ID we added to `profileid.h` (the `750`) but can be different if needed. If you're using a Physics variable that needs to move objects with it, change this execution order to `ProfileId::EN_SLIP_PENGUIN`.
- `ProfileId::MySprite`: The drawing order of the sprite. It's the same as the actor ID we added to `profileid.h` (the `750`) but can be different if needed.
- `"MySprite"`: The name of the sprite. It's the same as the sprite name we added to `profileid.h` (the `MySprite`).
- `MySpriteFileList`: The filelist of the sprite.
- `0`: Spawning flags. Here are the different flags you can add:
  - `0x2`: Always draws the actor
  - `0x4`: Supports kill combos ? (Not sure)
  - `0x8`: Makes Mario look at the actor
  - `0x10`: Killed by level clear
  - `0x20`: Killed by level clear and awards score
  - `0x40`: Only set on balloons ? (Not sure)
  - `0x80`: Can be killed by iceballs (magic e.g. podoboos)
  - `0x200`: Bounces Penguin Mario back when sliding into the actor
  - `0x400`: Ignores wrap around edges

You can then compile your code and enjoy !
