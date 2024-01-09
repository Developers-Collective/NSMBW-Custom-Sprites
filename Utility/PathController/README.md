# Path Controller v1.0.1
*by Nin0, slightly edited by Synel*

*Original code can be found on the [New Super Mario Lost Worlds](https://github.com/N-I-N-0/New-Super-Mario-Lost-Worlds) repo*

## Kamek
- Add `src/path.h` in your `src` folder
- In your `include/dCourse.h`, add this line before the `#endif` line:
```cpp
extern "C" dCourse_c::rail_s* GetRail(int id);
```
- Add this address to your `kamek_pal.x`:
```cpp
	GetRail = 0x800D91B0;
```

*PS: You won't be able tell if it'll work before having a sprite that uses it.*

If there is a compilation or a game problem, tell me, maybe I forgot something somewhere.
