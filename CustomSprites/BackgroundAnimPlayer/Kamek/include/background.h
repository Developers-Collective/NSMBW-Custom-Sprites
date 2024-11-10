#ifndef __BACKGROUND_H
#define __BACKGROUND_H

#include <game.h>

class daFarBG_c : public dStageActor_c {
    public:
        class nodeCallback_c {
            void * vtable;
            daFarBG_c * parent;
        };

        class bgData_t {
            public:
                Vec pos;
                u32 field1_0xc;
                Vec field2_0x10;
                u32 field3_0x1c;
                Vec field4_0x20;
                u8 unk[2200];
        };

        class BackgroundEntry {
            public:
                ushort id;
                s16 x_scroll_rate;
                s16 y_scroll_rate;
                short yOffset;
                short xOffset;
                ushort file_id_1;
                ushort file_id_2;
                ushort file_id_3;
                u8 field8_0x10;
                u8 field9_0x11;
                ushort scale;
                u8 field11_0x14;
                u8 field12_0x15;
                u8 field13_0x16;
                u8 field14_0x17;
        };

        class BackgroundModelThing {
            public:
                m3d::mdl_c * model;
                m3d::anmChr_c * chrAnimation;
                m3d::anmClr_c * clrAnimation;
                m3d::anmTexSrt_c * srtAnimation;
                bool isTranslation;
                bool field5_0x11;
                u8 field6_0x12[2];
                bool enabled;
                u8 padMaybe[3];
                EGG::Effect follow_effects[2];
                // Start of new stuff
                m3d::anmTexPat_c * patAnimation;
                m3d::anmVis_c * visAnimation;
                dStageActor_c * currentChrActor, * currentClrActor, * currentSrtActor, * currentPatActor, * currentVisActor;
        };

        class PLANE {
            Vec N;
            float d;
        };

        class AABB {
            Vec Pmin;
            Vec Pmax;
        };

        class FRUSTUM {
            Mtx cam;
            PLANE leftPlane;
            PLANE rightPlane;
            PLANE topPlane;
            PLANE bottomPlane;
            float near;
            float far;
            AABB box;
            PLANE planes[6];
        };

        nodeCallback_c callback;
        dHeapAllocator_c allocator;
        bgData_t * m_bgData;
        BackgroundEntry * m_background_entries[2];
        BackgroundModelThing * backgrounds[27];
        BackgroundModelThing * staticBackground;
        uint staticBackgroundIdxMaybe;
        Vec field80_0x438[6];
        Vec2 m_offset;
        u8 field82_0x488;
        u8 field83_0x489;
        u8 field84_0x48a;
        u8 field85_0x48b;
        Vec2 _48C;
        u8 field87_0x494;
        u8 field88_0x495;
        u8 field89_0x496;
        u8 field90_0x497;
        Vec field91_0x498[6];
        int m_repeat_type;
        int isBGB;
        float bg_scroll; // (?)
        u32 field95_0x4ec;
        u32 field96_0x4f0;
        FRUSTUM m_frustum;
        u8 field98_0x5e4;
        u8 field99_0x5e5;
        u8 field100_0x5e6;
        u8 field101_0x5e7;
        u8 field102_0x5e8[2];
        u8 field103_0x5ea;
        u8 field104_0x5eb;
        float center_x_pos;
        uint center_mode;
        ushort field107_0x5f4;
        u16 field108_0x5f6;
        ushort field109_0x5f8;
        short field110_0x5fa;
        Mtx field111_0x5fc;
        u8 unk2[44];
        ushort random_shorts[100];

        void doDelete(); // 0x80116000
        void prepareIndividualBackgroundObject(ushort bgId, daFarBG_c::BackgroundModelThing *bgMdl, bool unk); // 0x80116570
        void UpdateAnim(); // 0x80118f00
};

/// @brief Gets the far background object.
/// @param isBgB Whether to get the B background or not (A is default)
/// @return The far background object.
daFarBG_c * getFarBG(bool isBgB) {
    daFarBG_c * bg = (daFarBG_c*)fBase_c::searchByProfileId((u16)ProfileId::FAR_BG, 0);
    if (bg->isBGB != isBgB) bg = (daFarBG_c*)fBase_c::searchByProfileId((u16)ProfileId::FAR_BG, bg);
    return bg;
}

#endif // __BACKGROUND_H
