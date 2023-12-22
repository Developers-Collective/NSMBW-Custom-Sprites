#ifndef __LIGHT_H
#define __LIGHT_H

#include <game.h>

namespace Light
{
    enum LightType {
        Cave = 0,
        CaveSmall = 1,
        Dark = 2,
        DarkPlayer = 3,
        DeepSea = 4,
        FlashLightA = 5,
        FlashLightB = 6,
    };

    extern "C" u8 GetLightBTI(u32 lightType);
    extern "C" char *LightTypeArray[7];

    class StageActorLightNode1 : public m3d::scnLeaf_c {
        public:
            u32 light_bti;
            struct spline_c * spline;
            float field3_0x10;
            u8 field4_0x14;
            u8 field5_0x15;
            u8 field6_0x16;
            u8 field7_0x17;
    };

    class StageActorLightNode2 : public StageActorLightNode1 { };

    class dMask_c {
        public:
            StageActorLightNode1 * field1_0x4;
            StageActorLightNode2 * field2_0x8;

            u32 setup(mAllocator_c *allocator, spline_c *spline, int unk, u32 lightType);
            void draw();
            void execute();
    };

    struct quad_c {
        Vec vecs[4];
    };

    class dCircleLightMask_c {
        public:
            void * vtable;
            Vec position;
            float scale;
            dMask_c * mask;
            quad_c * quad;

            u32 init(mAllocator_c *allocator, u32 lightType); // 0x800ccb70 | vtable + 0x8
            u32 execute(); // 0x800ccc30 | vtable + 0xc
            u32 draw(); // 0x800ccce0 | vtable + 0x10
    };
};

#endif
