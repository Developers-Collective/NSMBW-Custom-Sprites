#include <asm_setup.S>

.text

.extern continueOriginalCheckWater
.global checkWater
checkWater:
	stwu sp, -0x80(sp)
	b continueOriginalCheckWater
