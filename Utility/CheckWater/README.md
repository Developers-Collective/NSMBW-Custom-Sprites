# Check Water v1.0.0
*by Synel*


## Kamek
- Add `src/checkWater.S` in your `src` folder
- Add `/checkWater.yaml` in your `Kamek` folder
- Reference the `checkWater.yaml` in your `NewerProject.yaml`
- Add these lines to your `Kamek/kamek_pal.x`:
```cpp
	continueOriginalCheckWater = 0x80075274;
```
- Compile your code

*PS: You won't be able tell if it'll work before having a sprite that uses it.*

If there is a compilation or a game problem, tell me, maybe I forgot something somewhere.
