# Group Controller Fix v1.0.0
*by Synel*


## Kamek
- Add `src/groupController.h` and `src/groupController.cpp` in your `src` folder
- Add `/groupController.yaml` in your `Kamek` folder
- Reference the `groupController.yaml` in your `NewerProject.yaml`
- Add these lines to your `Kamek/kamek_pal.x`:
```cpp
	groupControllerOnCreate = 0x807fc7a0;
	groupControllerOnExecute = 0x807fc860;
```
- Compile your code

*PS: You won't be able tell if it'll work before having a sprite that uses it.*

If there is a compilation or a game problem, tell me, maybe I forgot something somewhere.
