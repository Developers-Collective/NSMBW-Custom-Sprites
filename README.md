<h1 align="center"><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Logo.png" width="32" align="center" /> NSMBW Custom Sprites Repository</h1>
<p align="center">
  <a href="https://www.python.org/downloads/">
    <img alt="Compiler: CodeWarrior" src="https://img.shields.io/badge/Compiler-CodeWarrior-blue" />
  </a>
  <a href="https://doc.qt.io/qtforpython/index.html">
    <img alt="Compatible: Newer 1.3.X and NSMBWer+" src="https://img.shields.io/badge/Compatible-Newer%201.3.X%20|%20NSMBWer+-magenta" />
  </a>
  <a href="https://github.com/Developers-Collective/OGE-Next/blob/master/LICENSE">
    <img alt="License: LGPL" src="https://img.shields.io/badge/License-MIT-green" target="_blank" />
  </a>
  <img alt="Platforms: Wii and Dolphin" src="https://img.shields.io/badge/Platforms-Wii%20|%20Dolphin-yellow" />
</p>

----------------------------------------------------------------------

This repository contains the source code for the custom sprites I made for New Super Mario Bros. Wii.

If you want to use some of this code, be sure to credit the corresponding author(s) and to follow the instructions in the `README.md` file in the extracted folder.


## Important Notices

> [!IMPORTANT]\
> You need to be able to read and follow a tutorial; double check you followed **all the steps** before asking for help.
> 
> If you don't know how to do something, go search for the required knowledge on the [wiki](https://horizon.miraheze.org/wiki/Main_Page).
>
> Please credit the corresponding author(s) when using their sprite(s) / feature(s). The author(s) are found on top of their corresponding `readme.md` file.


## Before Continuing

> [!WARNING]\
> Some stuff can have some bugs, especially when they just get released. If you find one, you can report it to the corresponding author(s) while **staying calm and polite** and **wait patiently** for an update.

> [!TIP]\
> It is recommended to keep track of the sprite / feature version(s) you're using so you can update when an update and / or a bugfix releases.


## How to use

1. Download the sprite you want to use from the [releases](https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases) page or in the list below.
2. Extract the archive.
3. Follow the instructions in the `README.md` file in the extracted folder.
4. Enjoy!


> [!NOTE]\
> Don't hesitate to check out some other repos if you can't find what you need here ;)
> - [wakanameko's NSMBW Custom Sprites](https://github.com/wakanameko/NSMBW-Custom-Sprites)


## Utilities

<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/CheckWater/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/CheckWater">Check Water</a></summary>
  <p></p>

This utility is used to check if a sprite is in a liquid or not and to get the liquid type.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/ExtendedSpriteSettings/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/ExtendedSpriteSettings">Extended Sprite Settings</a></summary>
  <p></p>

This utility is used to add more settings to your custom sprites.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/GroupControllerFix/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/GroupControllerFix">Group Controller Fix</a></summary>
  <p></p>

When you have a sprite that uses the Group Controller, it won't spawn at all if its Group Controller is already loaded (e.g. being near the player spawn camera view). This utility fixes this issue by delaying the Group Controller onCreate method by one frame, which is enough to make the sprite behave correctly, as the global Group List is loaded at the end of the first frame (so after the loaded onCreate methods).

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/Light/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/Light">Light & Light Path</a></summary>
  <p></p>

This utility is used to create light in the darkness. It also allows you to create light paths.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/MoreSFX/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/MoreSFX">More SFX</a></summary>
  <p></p>

This utility allows you to add more sound effects to your custom sprites.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/MoreSprites/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/MoreSprites">More Sprites</a></summary>
  <p></p>

This utility allows you to add more sprites to the game without replacing existing ones.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/PathController/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/PathController">Path Controller</a></summary>
  <p></p>

This utility allows you to use path controlled sprites.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/RotatingActivePhysics/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/RotatingActivePhysics">Rotating Active Physics</a></summary>
  <p></p>

This utility is used to create rotating active physics for your custom sprites.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/SpriteDataUtils/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/SpriteDataUtils">Sprite Data Utils</a></summary>
  <p></p>

This utility contains a set of tools to help you create sprite data.

</details>

<!-- <details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/Utilities/Layer0Lava/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/Layer0Lava">Layer 0 Lava</a></summary>
  <p></p>

This utility allows you to place lava on layer 0, like water.

</details> -->


## Custom Sprites

<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/BackgroundAnimPlayer/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/BackgroundAnimPlayer">Background Anim Player</a></summary>
  <p></p>

A sprite that allow playing most Wii animation formats on backgrounds and to trigger them based on triggering events.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/Biddybud/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/Biddybud">Biddybud</a></summary>
  <p></p>

Biddybud and Para-Biddybud follow a path, in groups.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/CameraScrollLimiter/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/CameraScrollLimiter">Camera Scroll Limiter</a></summary>
  <p></p>

Smooth camera scroll limiter that unlocks the camera if the player gets too close to it while permeable.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/CoinStack/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/CoinStack">Coin Stack</a></summary>
  <p></p>

A stack of 5 coins.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/Goombrat/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/Goombrat">Goombrat</a></summary>
  <p></p>

Goombrat can walk back and forth, like a Goomba, but turns around edges.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/Draglet/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/Draglet">Draglet</a></summary>
  <p></p>

Draglet from Super Mario 3D Land. It flies around and shoots fireballs when Mario is near.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/DryBones/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/DryBones">Dry Bones & Para Bones</a></summary>
  <p></p>

A skeletal Koopa Troopa that cannot die from being stomped on.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/BlockElectricity/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/BlockElectricity">Electricity Block</a></summary>
  <p></p>

Electricity Block from Super Mario Bros. Wonder that shoots electricity.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/FlipBlock/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/FlipBlock">Flip Block</a></summary>
  <p></p>

Flip Blocks from Super Mario Bros. Wonder. It flips when Mario hits them.

https://github.com/Developers-Collective/NSMBW-Custom-Sprites/assets/70210528/4ce1e7e5-289d-45e7-8424-a211ef7295ee

https://github.com/Developers-Collective/NSMBW-Custom-Sprites/assets/70210528/0ea4f2e4-f8ee-48f0-b633-87376a66c731

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/Komboo/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/Komboo">Komboo</a></summary>
  <p></p>

A walking giant kelp-like creatures with eyes. Their name is likely a reference to kombu, a variety of seaweed used extensively in Japanese cuisine.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/MidwayFlag/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/MidwayFlag">Midway Flag (with gravity)</a></summary>
  <p></p>

Allows the midway flag to fall when not on the ground.

https://github.com/Developers-Collective/NSMBW-Custom-Sprites/assets/70210528/84dda543-da57-426c-b8ac-760b6d5c8352

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/Signboard/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/Signboard">Signboard (with gravity)</a></summary>
  <p></p>

Allows the signboard to fall when not on the ground.

</details>


<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/CustomSprites/SpikeTrap/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/SpikeTrap">Spike Trap</a></summary>
  <p></p>

A spike that comes out from the ground to damage every player who would adventure too far.

</details>


## Sprite Tex

<details>
  <summary><img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/SpriteTex/StarCoin/Icon.png" width="24" align="center" />&nbsp;&nbsp;<a href="https://github.com/Developers-Collective/NSMBW-Custom-Sprites/releases/tag/StarCoin">Star Coin</a></summary>
  <p></p>

Sprite Tex for the Star Coin.

<img src="https://raw.githubusercontent.com/Developers-Collective/Assets/main/NSMBW-Custom-Sprites/readme/SpriteTex/StarCoin/Demo.png" />

</details>
