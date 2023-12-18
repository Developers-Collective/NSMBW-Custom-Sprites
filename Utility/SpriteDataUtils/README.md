# Sprite Data Utils v1.0.0
*by RedStoneMatt, Synel & Nin0*

## Kamek
Open `include/game.h` and add this function at the end of the file (before the `#endif` line):
```cpp
int getNybbleValue(u32 settings, int fromNybble, int toNybble) {
	int numberOfNybble = (toNybble - fromNybble) + 1;
	int valueToUse = 48 - (4 * toNybble);
	int fShit = pow(16, numberOfNybble) - 1;
	return ((settings >> valueToUse) & fShit);
}

void getSpriteTexResName(char* buffer, int resID) {
	sprintf(buffer, "g3d/t%02d.brres", resID);
	buffer[strlen(buffer)] = 0;
}

void getSpriteTexResName255(char* buffer, int resID) {
	sprintf(buffer, "g3d/t%03d.brres", resID);
	buffer[strlen(buffer)] = 0;
}
```

Then in `include/game.h`, find this line in the `dStageActor_c` class:
```cpp
u16 spriteFlagNum;
```
And replace it with this if not already done:
```cpp
union {
	u16 spriteFlagNum;
	struct {
		u8 eventId2; // nybble 1-2
		u8 eventId1; // nybble 3-4
	};
};
```

Now, still in `include/game.h`, under the `float floor(float x);` line, add this:
```cpp
float pow(float x, float y);
float sqrt(float x) { return pow(x, 0.5f); }
```

Add these addresses to your `kamek_pal.x`:
```cpp
pow = 0x802E5894;
```
