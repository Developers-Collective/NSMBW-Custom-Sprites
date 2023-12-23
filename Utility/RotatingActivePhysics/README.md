# Rotating Active Physics v1.0.1
*by Synel*

## Kamek
- Add `src/rotatingActivePhysics.h` in your `src` folder
- In your `game.h` file, add the following line in the `ActivePhysics` class, under the `Info` struct:
```cpp
enum CollisionTypes {
    NORMAL = 0,
    CIRCLE = 1,
    VERTICAL_TRAPEZOID = 2,
    HORIZONTAL_TRAPEZOID = 3
};
```

*PS: You won't be able tell if it'll work before having a sprite that uses it.*

If there is a compilation or a game problem, tell me, maybe I forgot something somewhere.
