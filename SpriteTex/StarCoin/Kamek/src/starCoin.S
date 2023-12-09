.text
.align 4
.extern GetTexFilenameForR5 #spriteTex

.global TEX_StarCoin
TEX_StarCoin:
    lwz r5, 4(r29)
    srwi r5, r5, 24
    andi. r5, r5, 0xF
    b GetTexFilenameForR5
