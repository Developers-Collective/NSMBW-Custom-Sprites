# Light & Light Path v1.0.0
*by Synel*

## Kamek
- Add `src/light.h` and `src/lightPath.h` in your `src` folder
- Add these addresses to your `kamek_pal.x`:
```cpp
	init__Q25Light18dCircleLightMask_cFP12mAllocator_cUi = 0x800ccb70;
	execute__Q25Light18dCircleLightMask_cFv = 0x800ccc30;
	draw__Q25Light18dCircleLightMask_cFv = 0x800ccce0;
	setup__Q25Light7dMask_cFP16mHeapAllocator_cPQ25Light8spline_cUi = 0x800cb580;
	execute__Q25Light7dMask_cFv = 0x800cb6a0;
	draw__Q25Light7dMask_cFv = 0x800cb6c0;
	GetLightBTI = 0x800cac00;
	LightTypeArray = 0x80317a20;
```

*PS: You won't be able tell if it'll work before having a sprite that uses it.*

If there is a compilation or a game problem, tell me, maybe I forgot something somewhere.
