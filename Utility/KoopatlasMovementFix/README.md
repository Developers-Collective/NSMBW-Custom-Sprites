# Koopatlas Movement Fix v1.0.0
*by AboodXD*

*Original code can be found in this [pull request](https://github.com/Newer-Team/NewerSMBW/pull/9) on the [NewerSMBW](https://github.com/Newer-Team/NewerSMBW) repo*


## Explanation
This patch fixes 2 bugs of the player movement on the Koopatlas world map:
- **Stutter on every node:** When a movement step carries the player past a node, the game snaps the player back onto the node and throws away the rest of the step, so the player moves less than usual during this frame. This creates a visible stutter each time the player walks over a node. The rest of the step is now used on the next path, so the movement speed stays (almost) constant.
- **Rotation glitch on stacked nodes:** Maps can have several nodes placed on the same spot, which creates paths with a length of 0 between them. These paths have no direction, so the game calculates a wrong rotation for the player during the frame spent on them. The player now keeps the current rotation on these paths.


## Requirements
- Knowledge about how to install this kind of stuff (at least the basics)
- Kamek 1.3.X (changes may have to be made for Newer 1.2.X)
- A project using Koopatlas (NSMBWer+ doesn't use Koopatlas, don't ask how to use it there!)
- [Git](https://git-scm.com/downloads) if you want to use the patch file (method 1)


## Kamek
There are 2 ways to install this patch:
- **Method 1:** Apply `KoopatlasMovementFix.patch` with git. It's the fastest way, but it only works if the parts of the Koopatlas code changed by this patch are still the same as in the original Newer source code.
- **Method 2:** Do the changes by hand. It always works, even if you already edited your Koopatlas code.

If you're not sure which one to use, try the first method. If the patch can't be applied, nothing is changed and you can still use the second method.

Both methods change the same 3 files in the `Kamek/src/koopatlas` folder: `pathmanager.cpp`, `pathmanager.h` and `player.cpp`.


### Method 1: Patch File
- Copy `KoopatlasMovementFix.patch` into the root folder of your Newer source code (the folder containing the `Kamek` folder)
- Open a terminal in this folder (on Windows, you can type `cmd` in the address bar of the file explorer and press Enter)
- Run this command to check if the patch can be applied (this command doesn't change anything):
```bash
git apply --check -v KoopatlasMovementFix.patch
```
- If there is no line starting with `error:` or `Skipped patch`, the patch can be applied. The result should look like this (lines like `Hunk #1 succeeded at 1025 (offset -1 lines).` can also appear, they are fine):
```
Checking patch Kamek/src/koopatlas/pathmanager.cpp...
Checking patch Kamek/src/koopatlas/pathmanager.h...
Checking patch Kamek/src/koopatlas/player.cpp...
```
- Run this command to apply the patch:
```bash
git apply -v KoopatlasMovementFix.patch
```
- Make sure these 3 lines are at the end of the result:
```
Applied patch Kamek/src/koopatlas/pathmanager.cpp cleanly.
Applied patch Kamek/src/koopatlas/pathmanager.h cleanly.
Applied patch Kamek/src/koopatlas/player.cpp cleanly.
```
- Delete `KoopatlasMovementFix.patch` from your Newer folder
- Compile your code

If the check gives you something else:
- A line ending with `patch does not apply`: Your Koopatlas code is different from the original one. Use the second method instead.
- A line ending with `No such file or directory`: The terminal isn't opened in the folder containing the `Kamek` folder.
- Lines starting with `Skipped patch`: Make sure the terminal is opened in the folder containing the `Kamek` folder.

> [!TIP]\
> If you never edited these files but still get `patch does not apply`, the line endings of your files may have been changed (e.g. by your text editor). In this case, try again with `--ignore-whitespace` added to both commands (e.g. `git apply --check -v --ignore-whitespace KoopatlasMovementFix.patch`).


### Method 2: Manual Changes

#### `pathmanager.h`
- Open `Kamek/src/koopatlas/pathmanager.h` and in the `dWMPathManager_c` class, replace this line:
```cpp
		void moveThroughPath(int pressedDir);
```
with this:
```cpp
		// carryOver: distance to advance this call instead of a full moveSpeed step.
		// Negative means "use moveSpeed". Used to spend the distance left over after
		// crossing a node on the path we continue onto.
		void moveThroughPath(int pressedDir, float carryOver = -1.0f);
```

#### `pathmanager.cpp`
- Open `Kamek/src/koopatlas/pathmanager.cpp` and in the `startMovementTo` function, replace these lines:
```cpp
	// calculate direction of the path
	short deltaX = path->end->x - path->start->x;
	short deltaY = path->end->y - path->start->y;
	u16 direction = (u16)(atan2(deltaX, deltaY) / ((M_PI * 2) / 65536.0));
```
with these:
```cpp
	// calculate direction of the path
	short deltaX = path->end->x - path->start->x;
	short deltaY = path->end->y - path->start->y;

	// Maps chain several nodes onto a single spot, leaving zero length paths between them.
	// Such a path has no direction: atan2(0, 0) has no meaningful answer and returns 0,
	// and the reverseThroughPath flip below then turns that 0 into a hard 180, so the player
	// snaps to a garbage facing for the one frame the path is alive. Detect it here and
	// keep the facing we already have.
	const bool hasNoDirection = deltaX == 0 && deltaY == 0;

	u16 direction = (u16)(atan2(deltaX, deltaY) / ((M_PI * 2) / 65536.0));
```
- Still in the `startMovementTo` function, a few lines below, replace these lines:
```cpp
	daWMPlayer_c *player = daWMPlayer_c::instance;


	// Consider adding these as options
```
with these:
```cpp
	daWMPlayer_c *player = daWMPlayer_c::instance;

	// OSReport(
	// 	"startMovementTo: path=%08X start=(%d,%d) end=(%d,%d) curNode=%08X (%d,%d) "
	// 	"rev=%d dir=%d deg | player=(%f,%f,%f) targetRot=%d deg\n",
	// 	(u32)path, (s32)path->start->x, (s32)path->start->y,
	// 	(s32)path->end->x, (s32)path->end->y,
	// 	(u32)currentNode, (s32)currentNode->x, (s32)currentNode->y,
	// 	(s32)reverseThroughPath, (s32)((s16)direction * 180.0f / 32768.0f),
	// 	player->pos.x, player->pos.y, player->pos.z,
	// 	(s32)((s16)player->targetRotY * 180.0f / 32768.0f));

	// Consider adding these as options
```

- Still in the `startMovementTo` function, further down, replace these lines:
```cpp
		forcedRotation = false;
		player->setTargetRotY(direction);
```
with these:
```cpp
		forcedRotation = false;
		// A path with no length has no direction to face along, so leave the player as he is.
		if (!hasNoDirection)
			player->setTargetRotY(direction);
```
- Now go to the `moveThroughPath` function (right under the `startMovementTo` function) and replace the beginning of the function:
```cpp
void dWMPathManager_c::moveThroughPath(int pressedDir) {
	dKPNode_s *from, *to;

	from = reverseThroughPath ? currentPath->end : currentPath->start;
	to = reverseThroughPath ? currentPath->start : currentPath->end;

	daWMPlayer_c *player = daWMPlayer_c::instance;
```
with this:
```cpp
void dWMPathManager_c::moveThroughPath(int pressedDir, float carryOver) {
	dKPNode_s *from, *to;

	from = reverseThroughPath ? currentPath->end : currentPath->start;
	to = reverseThroughPath ? currentPath->start : currentPath->end;

	short deltaX = to->x - from->x;
	short deltaY = to->y - from->y;

	daWMPlayer_c *player = daWMPlayer_c::instance;
```
- Still in the `moveThroughPath` function, replace this line:
```cpp
		int whatDirDegrees = ((int)(atan2(to->x-from->x, to->y-from->y) / ((M_PI * 2) / 360.0)) + 360) % 360;
```
with this:
```cpp
		int whatDirDegrees = ((int)(atan2(deltaX, deltaY) / ((M_PI * 2) / 360.0)) + 360) % 360;
```
- Still in the `moveThroughPath` function, replace this line:
```cpp
		OSReport("Delta: %d, %d; Degrees: %d (Atan result is %f); Calced dir is %d; Pressed dir is %d\n", to->x-from->x, to->y-from->y, whatDirDegrees, atan2(to->x-from->x,to->y-from->y), whatDir, pressedDir);
```
with this:
```cpp
		OSReport("Delta: %d, %d; Degrees: %d (Atan result is %f); Calced dir is %d; Pressed dir is %d\n", deltaX, deltaY, whatDirDegrees, atan2(deltaX, deltaY), whatDir, pressedDir);
```
- Still in the `moveThroughPath` function, replace these lines:
```cpp
	Vec move = (Vec){to->x - from->x, to->y - from->y, 0};
	VECNormalize(&move, &move);
	VECScale(&move, &move, moveSpeed);
```
with these:
```cpp
	Vec move = (Vec){deltaX, deltaY, 0};
	VECNormalize(&move, &move);
	// How much ground this call may cover: normally a full step, or whatever was left of the
	// step when we crossed a node partway through it.
	const float stepSize = carryOver >= 0.0f ? carryOver : moveSpeed;
	VECScale(&move, &move, stepSize);
```
- Still in the `moveThroughPath` function, a few lines below in the `if (isJumping)` part, replace this line:
```cpp
			float xDistance = to->x - from->x;
```
with this:
```cpp
			float xDistance = deltaX;
```
- Still in the `moveThroughPath` function, under the `// Check if we've reached the end yet` comment, replace these lines:
```cpp
			(from->x == to->x && from->y == to->y)
	   ) {

		currentNode = to;
```
with these:
```cpp
			(deltaX == 0 && deltaY == 0)
	   ) {
		// We almost never land exactly on the node: this frame's step usually carries us
		// some way past it. Measure that leftover distance before snapping onto the node,
		// so it can be spent on the path we continue onto instead of being thrown away.
		float leftover;
		if (deltaX == 0 && deltaY == 0)
			leftover = stepSize;
		else {
			Vec past = {player->pos.x - to->x, -player->pos.y - to->y, 0};
			leftover = VECMag(&past);
		}

		currentNode = to;
```
- Still in the `moveThroughPath` function, near the end of the function, replace these lines:
```cpp
			if (!movingAgain)
				player->startAnimation(wait_select, 1.2, 10.0, 0.0);

		} else {
			startMovementTo(to->getOppositeAvailableExitTo(currentPath));
			SpammyReport("passthrough node, continuing to next path\n");
		}
```
with these:
```cpp
			if (!movingAgain)
				player->startAnimation(wait_select, 1.2, 10.0, 0.0);
			else if (isMoving && leftover > 0.0f)
				moveThroughPath(-1, leftover);

		} else {
			startMovementTo(to->getOppositeAvailableExitTo(currentPath));
			SpammyReport("passthrough node, continuing to next path\n");
			if (isMoving && leftover > 0.0f)
				moveThroughPath(-1, leftover);
		}
```

#### `player.cpp`
- Open `Kamek/src/koopatlas/player.cpp` and in the `onExecute` function, replace these lines:
```cpp
	if (spinning)
		rot.y += 0xC00;
	else
		SmoothRotation(&rot.y, targetRotY, 0xC00);

	if (dScKoopatlas_c::instance->mapIsRunning())
		dScKoopatlas_c::instance->pathManager.execute();
```
with these (the `pathManager.execute()` call is moved above the rotation code):
```cpp
	if (dScKoopatlas_c::instance->mapIsRunning())
		dScKoopatlas_c::instance->pathManager.execute();

	if (spinning)
		rot.y += 0xC00;
	else
		SmoothRotation(&rot.y, targetRotY, 0xC00);
```

- Compile your code