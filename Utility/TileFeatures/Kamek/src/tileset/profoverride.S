#include <asm_setup.S>

.text
.align 4

.extern DoObjOverride
.extern DestroyOverrides
.extern GetRailColor
.extern GetRailTexScale
.extern returnTo8007E3AC
.extern returnTo8007E274
.extern returnTo808B2A44


.global hookAt8007E30C
hookAt8007E30C:
    # If there are no zones, bail
    cmpwi r3, 0
    beq end

    # Setup call + modified original instruction
    lwz r4, 0x8(r3)
    mr r3, r29
    bl DoObjOverride

    # Return later to skip all the Nintendo fluff
    end:
    b returnTo8007E3AC


.global hookAt8007E270
hookAt8007E270:
    # Call destroyer
    bl DestroyOverrides

    # Original instruction
    li r0, 0
    b returnTo8007E274


.global hookAt808B2A40
hookAt808B2A40:
    # Call C++ function
    addi r4, r6, 4
    bl GetRailColor

    # Move color to r0
    mr r0, r3

    # Restore registers and return
    mr r3, r29
    li r4, 1
    addi r5, r1, 0xC
    b returnTo808B2A44


.global hookAt808B2AE0
hookAt808B2AE0:
    # Original instruction
    add r3, r3, r0

    # Push stack manually
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)

    # Save r5
    stw r5, 0xC(r1)

    # Call CPP function
    addi r3, r3, 0x2C
    mr r4, r29
    bl GetRailTexScale

    # Subtract from offset to fit the original instructions
    subi r3, r3, 0x2C

    # Restore r5
    lwz r5, 0xC(r1)

    # Pop stack manually and return
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

