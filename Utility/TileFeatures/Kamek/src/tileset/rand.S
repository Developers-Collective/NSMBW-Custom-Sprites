#include <asm_setup.S>

.text
.align 4

.extern DoRandTile

.global hookAt80086B60
hookAt80086B60:
    # Push stack manually
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)

    # Modified original instruction
    mr r3, r29
    mr r4, r30
    bl DoRandTile

    # Restore r0 and reload LR
    lhz r0, 0xC10(r30)
    lwz r12, 0x14(r1)

    # If return is false, fall back to original call
    cmpwi r3, 0
    beq+ end

    # Else skip it
    addi r12, r12, 0x108

    # Restore r3
    end:
    mr r3, r29

    # Pop stack manually and return
    mtlr r12
    addi r1, r1, 0x10
    blr
