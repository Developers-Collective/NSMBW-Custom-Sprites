#ifndef __KAMEK_TIMEKEEPER_H
#define __KAMEK_TIMEKEEPER_H

#include <common.h>
#include <game.h>
#include <dMultiMng_c.h>

class TimeKeeper {                       //The TimeKeeper class, handles things relited to the level's timer
	public:
		u32 *vtable;
		u32 timePlusFFFTimes40000;
		u16 time;
		u8 ambushMaybe;
		u8 timeLessThan100;
		u8 paused;
		
		static TimeKeeper* instance;
		void setTime(int time);
};

class Zone {                             //The Zone class, handles things related to a zone (duh)
    public:
        u16 x;
        u16 y;
        u16 w;
        u16 h;
        u16 objShad;
        u16 terShad;
        u8 id;
        u8 bnd_id;
        u8 scroll;
        u8 zoom;
        u8 field_10;
        u8 light;
        u8 fg_id;
        u8 bg_id;
        u8 mp_bias;
        u8 field_15;
        u8 music;
        u8 audio;
};


class Stage32C {                         //The Stage32C class, handles much random stuff
    public:
        u32 bulletData1[32];
        u32 bulletData2[32];
        u32 enemyCombos[4];
        u32 somethingAboutHatenaBalloons;
        u32 redCoinCount[4];
        u32 field_124;
        u32 field_128;
        u32 field_12C;
        u32 field_130;
        u32 greenCoinsCollected;
        u32 hasKilledEnemyThisTick_maybe;
        u16 booID;
        u8 _13E;
        u8 _13F;
        u32 bigBooID;
        u32 homingBanzaiID;
        u16 bulletBillCount;
        u8 _14A;
        u8 _14B;
        u32 bombCount;
        u32 goombaCount;
        u32 enemyKillCounter_maybe;
        u32 a_counter;
        u32 setToOneByBossDemoOrSomething;
        u32 aboutMortonBigPile;
        u32 somethingAboutPunchingChainlinkFence;
        u32 currentBigHanaMgr;
        u8 _16C;
        u8 _16D;
        u8 _16E;
        u8 _16F;
        u8 penguinCount;
        u8 _171;
        u8 _172;
        u8 _173;
        u32 pokeyTimer;
};

/*************************************************************************************************************************/
/*The MicroGoombaManager, dEffectExplosionMgr_c, dTimerMgr_c, dBlockMgr_c, StageF70, StageF74Sub and the StageF74 classes*/
/*************************************are here only to make the dScStage_c class work*************************************/
/*************************************************************************************************************************/

class MicroGoombaManager {
    public:
        u32 storedActorIDs[100];
        u8 storedGroupIDs[100];
        u8 generatedNumberFlag[4];
        u16 generatedNumbers[4];
};

class dEffectExplosionMgr_c {
    public:
        u16 field_0;
};

class dTimerMgr_c {
    public:
        u16 field_0;
};

class dBlockMgr_c {
    public:
        u32 areDonutLiftsDisabled;
};

class StageF70 {
    public:
        u16 field_0;
};

class StageF74Sub {
    public:
        u16 field_0;
        u8 _2;
        u8 _3;
        float field_4;
        float field_8;
        float field_C;
        float field_10;
        float field_14;
        float field_18;
        float field_1C;
        float field_20;
};

class StageF74 {
    public:
        StageF74Sub field_0[16];
};

class dCurtainMng_c {
    class Sub {
        u16 field0_0x0;
        u8 field1_0x2;
        u8 field2_0x3;
        float field3_0x4[8];
    };

    dCurtainMng_c::Sub a[4];
    dCurtainMng_c::Sub b[4];
    dCurtainMng_c::Sub c[4];
    dCurtainMng_c::Sub d[4];
};

class PauseManager_c { /* StageC4 */
    void *vtable;
    u32 stageNum;
    u32 selection;
    u32 playerNum;
    u32 _10;
    u32 cancelMenuSelection;
    u8 flags;
    bool buttonAnimPlaying;
    bool exitingMenu;
    bool willDisplay;
    bool _1C;
    bool disablePause;
    u16 pad;
};

class StageE4Small {
    u8 field_0;
    u8 field_1;
    u8 field_2;
};

struct StageE4Sub {
    Vec position;
    u32 isEnabled;
};

class dActorMng_c { /* Formerly known as "StageE4" */
    StageE4Small fields[4];
    u8 platformCounter; /* used for z positioning in the platform generator */
    u8 currPlayerId;
    u8 nextPlayerId;
    u8 _F;
    u32 _10;
    u32 isExitingBossBattle;
    daBossDemo_c *currentBossDemo;
    void *kazanMgr;
    u32 autoscroolSwichID;
    u32 stopSpawningEnemies;
    u32 goombaZOrderThing;
    u32 liquidDirection;
    u32 enableVolcanoEruptions;
    u32 _34;
    u32 lakituCloudSinks;
    u32 isEnteringAmbush;
    u8 playerIdCopy;
    u8 pad[3];
    float flagpoleXPos;
    StageE4Sub subs[32];
};

class daBigHanaMgr_c : public dActorState_c {
    public:
        u32 pathID;
        float distance_to_next_wiggler;
        u32 idOfActor;
        u32 field_3DC;
};

class dEnemyMng_c { // Stage32C
    public:
        uint bulletData1[4][8];
        uint bulletData2[4][8];
        uint enemyCombos[4];
        uint somethingAboutHatenaBalloons;
        uint redCoinCount[4];
        uint _124[4];
        uint greenCoinsCollected;
        uint hasKilledEnemyThisTick_maybe;
        ushort booID;
        u8 _13E[2];
        uint bigBooID;
        uint homingBanzaiID;
        ushort bulletBillCount;
        u8 _14A[2];
        uint bombCount;
        uint goombaCount;
        int enemyKillCounter_maybe;
        int a_counter;
        uint setToOneByBossDemoOrSomething;
        uint aboutMortonBigPile;
        uint somethingAboutPunchingChainlinkFence;
        daBigHanaMgr_c * currentBigHanaMgr;
        uint _16C;
        u8 penguinCount;
        u8 _171[3];
        int pokeyTimer;

        static dEnemyMng_c *instance;
};

class dScStage_c: public dScene_c { /* 0x1218 */
    public:
        u32 runningFrameCount_probably;
        FunctionChain chain;
        Stage80 quake;
        PauseManager_c pause_manager;
        dActorMng_c actor_mng;
        dEnemyMng_c enemy_mng;
        MicroGoombaManager microGoombaManager;
        dWaterManager_c waterManager;
        dEffectExplosionMgr_c effectExplosionManager;
        dTimerMgr_c ten_coin_mgr;
        dBlockMgr_c blockMgr;
        dScoreMng_c score_mng;
        dCurtainMng_c curtain_mng;
        u8 unk[32];
        u32 ptrToGameDisplay; // dGameDisplay_c *ptrToGameDisplay;
        fBase_c *ptrToGoalManager;
        fBase_c *ptrToSmallScoreManager;
        u32 ptrToFukidashiManager; // dfukidashiManager_c *ptrToFukidashiManager;
        u32 ptr_course_time_up; // dCourseTimeUp_c *ptr_course_time_up;
        u32 ptr_mini_game_cannon; // dMiniGameCannon_c *ptr_mini_game_cannon;
        u32 ptr_mini_game_wire; // dMiniGameWire_c *ptr_mini_game_wire;
        u32 ptr_model_play_manager; // dModelPlayManager_c *ptr_model_play_manager;
        u32 ptr_message_window; // dMessageWindow_c *ptr_message_window;
        fBase_c *ptr_model_play_guide;
        u32 ptr_staff_credit_score; // dStaffCreditScore_c *ptr_staff_credit_score;
        dTheEnd_c *ptr_the_end;
        dYesNoWindow_c *ptr_yes_no_window;
        u32 unk2;
        u8 curWorld; /* 0: W1; 1: W2; etc. */
        u8 curLevel; /* 0: X-1; 1: X-2; etc. */
        u8 curArea; /* 0: Area 1; 1: Area 2; etc. */
        u8 curZone;
        u8 curLayer;
        u8 curEntrance;
        u8 field_1212;
        u8 field_1213;
        u32 uselessPTMFIndex;

        static u32 exeFrame;

        static dScStage_c *instance; // 0x8042a4a8

        int onExecute_orig(); //80925A60
        int newOnExecute();
};


#endif
