#include <asm_setup.S>

.text
.align 4

.extern AreCustomFlowersLoaded
.extern AddFlowerEntry
.extern AreCustomFlowersLoaded
.extern LoadCustomGrassBrres
.extern LoadCustomFlowerBrres
.extern returnTo80077F8C
.extern instancePtr


# Dehardcode tileset name check
.global hookAt80077F88
hookAt80077F88:
    # Check if flowers were loaded
    bl AreCustomFlowersLoaded

    # Move result to r29 and restore r0
    mr. r29, r3
    lbz r0, 0(r26)

    # Check result
    beq end

    # If result is 1, automatically set first character of string to 0 to skip the string checks
    li r0, 0

    # Restore r3 and execute original instruction
    end:
    mr r3, r26
    extsh. r0, r0
    b returnTo80077F8C


# Dehardcode tile position checks - trampoline
.global hookAt8007803C
hookAt8007803C:
    # Original instruction
    addis r3, r6, 1

    # If dGrassBinMng is not initialized, fall back to Nintendo's code
    lis r4, instancePtr@ha
    lwz r4, instancePtr@l(r4)
    lwz r4, 0(r4)
    cmpwi r4, 0
    beqlr+

    # Push stack manually
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)

    # Call the C++ function
    mr r3, r21
    mr r4, r6
    addi r5, r26, 0x8
    addi r6, r25, 0x10
    bl AddFlowerEntry

    # Move result to r6
    mr r6, r3

    # Pop stack and return (later)
    lwz r12, 0x14(r1)
    addi r12, r12, 0x1AC
    mtlr r12
    addi r1, r1, 0x10
    blr


# Dehardcode tileset name check 2
.global hookAt808762CC
hookAt808762CC:
    # Push stack manually and save r3
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xC(r1)
    mr r31, r3

    # Check if flowers were loaded
    bl AreCustomFlowersLoaded

    # Modified original instruction
    lbz r0, 0(r31)

    # Multiply result by 2 and move it to r28
    slwi. r28, r3, 1
    beq end2

    # Override r0 to skip the string check
    li r0, 0

    # Restore r3, push stack manually and return
    end2:
    mr r3, r31
    lwz r31, 0xC(r1)
    lwz r12, 0x14(r1)
    mtlr r12
    addi r1, r1, 0x10
    blr


# Switch ARC+BRRES file on the fly
.global hookAt80876364
hookAt80876364:
    # Original instruction
    cmpwi r28, 0

    # If style is not custom, return immediately
    cmpwi cr7, r28, 2
    bltlr+ cr7

    # Alter LR to return later
    mflr r12
    addi r12, r12, 0x2EC
    mtlr r12

    # Call C++ function (does not return)
    b LoadCustomFlowerBrres


# Switch ARC+BRRES file on the fly 2
.global hookAt808763C8
hookAt808763C8:
    # Original instruction
    cmpwi r28, 0

    # If style is not custom, return immediately
    cmpwi cr7, r28, 2
    bltlr+ cr7

    # Alter LR to return later
    mflr r12
    addi r12, r12, 0x38
    mtlr r12

    # Call C++ function (does not return)
    b LoadCustomFlowerBrres


# Switch ARC+BRRES file on the fly 3
.global hookAt8087657C
hookAt8087657C:
    # Call c++ function (does not return)
    mr r6, r28
    b LoadCustomGrassBrres
