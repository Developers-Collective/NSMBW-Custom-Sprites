#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <actors.h>
#include <profile.h>
#include <profileid.h>
#include <timekeeper.h>

#include "greenringreward.h"

class dAc_Py_c;
class daYoshi_c;
class daGreenRing_c;
class daGreenCoin_c;
class daGreenRingRewardItem_c;

static inline void stopAndDetachSoundHandle(nw4r::snd::SoundHandle& handle);
static inline void startGreenTimerSound(nw4r::snd::SoundHandle& handle, dStageActor_c* actor);
static inline void spawnBasicEffect(const char* effectName, const Vec& effectPos);
static inline u8 getRingFacingDirection(dEn_c* actor);
static inline void giveGreenCoinReward(int playerID);

typedef void (*CreateSmallScoreFn)(const Vec*, int, int, bool);
static const CreateSmallScoreFn kCreateSmallScore = (CreateSmallScoreFn)0x800B3540;

static int gGreenRingWaveCounts[kMaxGreenRingId];
extern "C" int gGreenRingPopupGuard = 0;
extern "C" int gGreenRingSuppressItemFallEffectCount = 0;

static inline void enableBigSmallAnimOnLatestSmallScorePopup() {
    static const int kSmallScoreCount = 30;
    static const int kSmallScoreEntrySize = 0x224;
    static const int kSmallScoreManagerScoresOffset = 0x148;
    static const int kSmallScoreManagerNextIdxOffset = 0x4A10;
    static const int kSmallScoreEnableBigSmallAnimOffset = 0x21F;

    dScStage_c* stage = (dScStage_c*)StagePtr;
    if (!stage || !stage->ptrToSmallScoreManager)
        return;

    u8* manager = (u8*)stage->ptrToSmallScoreManager;
    int nextIdx = *(int*)(manager + kSmallScoreManagerNextIdxOffset);
    int popupIdx = nextIdx - 1;
    if (popupIdx < 0)
        popupIdx = kSmallScoreCount - 1;
    else if (popupIdx >= kSmallScoreCount)
        popupIdx %= kSmallScoreCount;

    u8* popup = manager + kSmallScoreManagerScoresOffset + (popupIdx * kSmallScoreEntrySize);
    *(u8*)(popup + kSmallScoreEnableBigSmallAnimOffset) = 1;
}

static void showWavePopup(const Vec& pos, int waveNumber, int playerID) {
    Vec popupPos = pos;
    popupPos.y += 10.0f;
    int clampedWave = waveNumber;
    if (clampedWave < 1)
        clampedWave = 1;
    else if (clampedWave > kTotalWaves)
        clampedWave = kTotalWaves;

    int popupType = 0x7 + clampedWave;
    int validPlayerId = (playerID >= 0 && playerID < 4) ? playerID : -1;
    kCreateSmallScore(&popupPos, popupType, validPlayerId, false);
}

static inline bool shouldTintItemFallEffectName(const char* name) {
    if (!name)
        return false;
    return strcmp(name, "Wm_ob_itemfall") == 0
        || strcmp(name, "Wm_ob_itemfall_a") == 0
        || strcmp(name, "Wm_ob_itemfall_b") == 0;
}

extern "C" int GreenRingShouldTintItemFallEffect(const char* name) {
    if (gGreenRingSuppressItemFallEffectCount <= 0)
        return 0;
    if (!shouldTintItemFallEffectName(name))
        return 0;
    --gGreenRingSuppressItemFallEffectCount;
    return 1;
}

static inline void applyItemFallGreenTint(void* effectObj) {
    if (!effectObj)
        return;

    EGG::Effect* effect = (EGG::Effect*)effectObj;
    GXColor primary = {245, 255, 240, 255};
    GXColor secondary = {170, 255, 0, 255};
    effect->_vf58(245, 255, 240, 255, 3);
    effect->_vf5C(primary, secondary, 0, 3);
    effect->_vf5C(primary, secondary, 1, 3);
    effect->_vf60(255, 255, 0, 3);
    effect->_vf60(255, 255, 1, 3);
}

extern "C" void GreenRingItemFallEffectHook(int self) {
    if (!self)
        return;

    Vec pos;
    pos.x = *(float*)(self + 0xAC);
    pos.y = *(float*)(self + 0xB0) + 8.0f;
    pos.z = *(float*)(self + 0xB4);

    const char* effectName = (*(int*)(self + 0xDB0) == 0) ? "Wm_ob_itemfall" : "Wm_ob_redcioinitem02";
    int shouldTint = GreenRingShouldTintItemFallEffect(effectName);

    void* effectObj = (void*)(self + 0x994);
    void* vtbl = *(void**)effectObj;
    typedef void (*CreateEffectFn)(void* self, const char* name, int unk, Vec* pos, void* rot, void* scale);
    CreateEffectFn fn = *(CreateEffectFn*)((u8*)vtbl + 0xA0);

    if (shouldTint)
        applyItemFallGreenTint(effectObj);
    fn(effectObj, effectName, 0, &pos, 0, 0);
    if (shouldTint)
        applyItemFallGreenTint(effectObj);
}

extern "C" void GreenRing_setNormalOrBlueColor(void* self) {
    if (!self)
        return;

    u8* base = (u8*)self;
    u8* hasBlue = base + 0x221;
    u32* textBoxPtr = (u32*)(base + 0x1E0);
    if (!textBoxPtr || *textBoxPtr == 0)
        return;

    nw4r::lyt::TextBox* textBox = (nw4r::lyt::TextBox*)(*textBoxPtr);

    bool wasBlue = (*hasBlue != 0);
    if (wasBlue)
        *hasBlue = 0;

    GXColor top;
    GXColor bottom;

    if (gGreenRingPopupGuard > 0) {
        top = (GXColor){0, 255, 0, 255};
        bottom = (GXColor){200, 255, 200, 255};
    } else if (wasBlue) {
        top = (GXColor){60, 120, 255, 255};
        bottom = (GXColor){200, 240, 255, 255};
    } else {
        top = (GXColor){255, 10, 10, 255};
        bottom = (GXColor){255, 150, 150, 255};
    }

    nw4r::ut::Color convertedTop;
    nw4r::ut::Color convertedBottom;
    convertedTop = top;
    convertedBottom = bottom;
    textBox->SetVtxColor(0, convertedTop);
    textBox->SetVtxColor(2, convertedBottom);
}

static void handleGreenRingWaveReward(daGreenRing_c* ring, int completedWaveIndex, int playerID);

static void resetGreenRingWaveCount(int ringId) {
    if (ringId >= 0 && ringId < kMaxGreenRingId)
        gGreenRingWaveCounts[ringId] = 0;
}

static const float kTwoPi = 6.283185307179586f;
static const float kPi = 3.141592653589793f;

static inline float wrapRadians(float value) {
    while (value > kPi)
        value -= kTwoPi;
    while (value < -kPi)
        value += kTwoPi;
    return value;
}

static inline float fastAbs(float value) {
    return value < 0.0f ? -value : value;
}

static inline float fastSin(float angle) {
    angle = wrapRadians(angle);
    const float B = 1.27323954473516f;
    const float C = -0.405284734569351f;
    const float P = 0.225f;
    float y = B * angle + C * angle * fastAbs(angle);
    return P * (y * fastAbs(y) - y) + y;
}

static inline float fastCos(float angle) {
    return fastSin(angle + (kTwoPi * 0.25f));
}

static inline float clampFloat(float value, float minValue, float maxValue) {
    if (value < minValue)
        return minValue;
    if (value > maxValue)
        return maxValue;
    return value;
}

static const float kCoinOrbitRadius = 8.0f;
static const float kCoinOrbitSpeed = -0.12f;
static const float kGreenRingAnimationRate = 0.45f;
static const float kGreenRingFogDepthOffset = 12.0f;
static const float kGreenRingVerticalOffset = -8.0f;
static const float kCoinAngleStep = kTwoPi / 3.0f;
static const int kCoinWaveSize = 3;
static const int kWaveTimeoutFrames = 240;
static const int kTimerStartDelayFrames = 35;
static const int kTimerEndDelayFrames = 35;
static const u32 kGreenCoinDynamicFlag = 0x80000000;
static const u16 kCoinSpinRate = 0x0600;
static const u16 kCoinSpinPhaseDelta = 0x10000 / kCoinWaveSize;
static const float kCoinSpinAngleToRadians = kTwoPi / 65536.0f;
static const Vec kCoinFrontNormal = {0.0f, 1.0f, 0.0f};
static const float kCoinAxisLengthThreshold = 0.000001f;
static const float kCoinTiltAngleThreshold = 0.0001f;
static const u32 kGreenRingBaseBitfield2 = 0xFF8ABFF8;
static const u32 kGreenRingDisableYoshiBitsMask =
    ~((1u << 15) | (1u << 16) | (1u << 19));
static const u32 kGreenRingBitfield2 = kGreenRingBaseBitfield2 & kGreenRingDisableYoshiBitsMask;

static inline float fastAtan2(float y, float x) {
    if (x == 0.0f) {
        if (y > 0.0f) return kPi * 0.5f;
        if (y < 0.0f) return -kPi * 0.5f;
        return 0.0f;
    }

    float z = y / x;
    float atan;

    if (fastAbs(z) < 1.0f) {
        atan = z / (1.0f + 0.28f * z * z);
        if (x < 0.0f) {
            if (y < 0.0f)
                return atan - kPi;
            return atan + kPi;
        }
    } else {
        atan = kPi * 0.5f - z / (z * z + 0.28f);
        if (y < 0.0f)
            return atan - kPi;
    }

    return atan;
}

static const int kGreenRingLinkIdPrimaryShift = 28;
static const int kGreenRingLinkIdLegacyShift = 20;
static const int kGreenRingLinkIdMask = 0xF;

static inline int extractGreenRingLinkId(u32 settings) {
    int ringId = (settings >> kGreenRingLinkIdPrimaryShift) & kGreenRingLinkIdMask;
    if (ringId == 0)
        ringId = (settings >> kGreenRingLinkIdLegacyShift) & kGreenRingLinkIdMask;

    if (ringId < 0 || ringId >= kMaxGreenRingId)
        return 0;
    return ringId;
}

static bool ownerIsPlayerOrYoshi(dStageActor_c* owner) {
    if (!owner)
        return false;

    for (int i = 0; i < 4; ++i) {
        dStageActor_c* playerActor = GetSpecificPlayerActor(i);
        if (playerActor && playerActor == owner)
            return true;

        daPlBase_c* playerOrYoshi = GetPlayerOrYoshi(i);
        if (playerOrYoshi && (dStageActor_c*)playerOrYoshi == owner)
            return true;
    }

    return false;
}

static int decodeMarkerWaveIndex(u32 settings) {
    static const int shifts[] = {24, 16, 12, 8, 4, 0};

    for (int i = 0; i < (int)(sizeof(shifts) / sizeof(shifts[0])); ++i) {
        int value = (settings >> shifts[i]) & 0xF;
        if (value != 0 || i == (int)(sizeof(shifts) / sizeof(shifts[0])) - 1) {
            if (value < kTotalWaves)
                return value;
            return 0;
        }
    }

    return 0;
}

static Vec gGreenCoinWaveCenters[kMaxGreenRingId][kTotalWaves];
static bool gGreenCoinWaveCenterValid[kMaxGreenRingId][kTotalWaves];

static inline bool hasLinkedWaveCenter(int ringId, int waveIndex) {
    if (ringId < 0 || ringId >= kMaxGreenRingId || waveIndex < 0 || waveIndex >= kTotalWaves)
        return false;

    return gGreenCoinWaveCenterValid[ringId][waveIndex];
}

static inline void registerGlobalGreenCoinWaveCenter(int ringId, int waveIndex, const Vec& center) {
    if (ringId < 0 || ringId >= kMaxGreenRingId || waveIndex < 0 || waveIndex >= kTotalWaves)
        return;

    gGreenCoinWaveCenters[ringId][waveIndex] = center;
    gGreenCoinWaveCenterValid[ringId][waveIndex] = true;
}

const char* GreenRingArcNameList[] = {"green_ring", NULL};
const char* GreenCoinArcNameList[] = {"obj_coin", NULL};

class daGreenRing_c : public dEn_c {
public:
    bool activated;
    int ringId;

    daGreenCoin_c* activeCoins[kCoinWaveSize];
    int coinsInWave;
    float orbitAngle;
    float orbitRadius;
    float orbitSpeed;
    int wavesSpawned;
    int waveTimer;
    int tickTimer;
    bool timerPlayedThisWave;
    mEf::es2 idleSparkleEffect;
    bool ringComplete;
    bool shouldSpawnNextWave;
    int coinsCollectedThisWave;
    bool waveFailedDueToTimeout;
    int rewardType;
    bool rewardGranted;
    Vec baseCenter;
    s16 modelRotationOffsetY;
    Vec idleSparklePos;

    mHeapAllocator_c allocator;
    m3d::mdl_c bodyModel;
    m3d::anmChr_c ringAnimation;
    m3d::anmTexSrt_c ringTexSrtAnimation;
    m3d::mdl_c fogModel;
    m3d::anmTexSrt_c fogTexSrtAnimation;
    m3d::anmClr_c fogClrAnimation;
    bool hasRingAnimation;
    bool hasRingTexSrtAnimation;
    bool hasFogModel;
    bool hasFogTexSrtAnimation;
    bool hasFogClrAnimation;
    nw4r::snd::SoundHandle timerSfxHandle;

    static dActor_c* build();

    int onCreate();
    int onDelete();
    int onExecute();
    int onDraw();

    void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
    void spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther);
    void spawnNextWave();
    void coinCollected(daGreenCoin_c* coin, int playerID);
    void destroyActiveCoins();
    void finishRing();
    void updateModelMatrices();
    void updateIdleSparkleAnchor();
    Vec getWaveCenter(int waveIndex) const;
    void playRedCoinWaveSound(int waveIndex);
    void grantReward(int playerID);
    void yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat8_FencePunch(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);
};

class daGreenCoin_c : public dEn_c {
public:
    daGreenRing_c* ring;
    int orbitIndex;
    int waveIndex;
    Vec orbitCenter;

    bool visible;
    bool drawVisible;
    bool dynamicCoin;

    mHeapAllocator_c allocator;
    m3d::mdl_c bodyModel;
    u16 spinAngle;
    u16 spinOffset;

    static dActor_c* build();

    int onCreate();
    int onExecute();
    int onDraw();

    int makeVisible();

    void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
    void spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther);
    static void greenCoinCollision(ActivePhysics* apThis, ActivePhysics* apOther);
    void yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat8_FencePunch(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
    bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);

    void updateModelMatrices();
};

dActor_c* daGreenRing_c::build() {
    void *buffer = AllocFromGameHeap1(sizeof(daGreenRing_c));
    return new(buffer) daGreenRing_c;
}

dActor_c* daGreenCoin_c::build() {
    void *buffer = AllocFromGameHeap1(sizeof(daGreenCoin_c));
    return new(buffer) daGreenCoin_c;
}

const SpriteData GreenRingSpriteData = { ProfileId::EN_GREENRING, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile GreenRingProfile(&daGreenRing_c::build, SpriteId::EN_GREENRING, &GreenRingSpriteData, ProfileId::EN_GREENRING, ProfileId::EN_GREENRING, "EN_GREENRING", GreenRingArcNameList, 0);

const SpriteData GreenCoinSpriteData = { ProfileId::EN_GREENCOINU, 8, -16, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile GreenCoinProfile(&daGreenCoin_c::build, SpriteId::EN_GREENCOINU, &GreenCoinSpriteData, ProfileId::EN_GREENCOINU, ProfileId::EN_GREENCOINU, "EN_GREENCOINU", GreenCoinArcNameList, 0);

int daGreenRing_c::onCreate() {
    activated = false;
    ringId = extractGreenRingLinkId(this->settings);
    if (ringId < 0 || ringId >= kMaxGreenRingId)
        ringId = 0;
    resetGreenRingWaveCount(ringId);
    coinsInWave = 0;
    orbitAngle = kPi * 0.5f;
    orbitRadius = kCoinOrbitRadius;
    orbitSpeed = kCoinOrbitSpeed;
    wavesSpawned = 0;
    waveTimer = 0;
    tickTimer = 0;
    timerPlayedThisWave = false;
    stopAndDetachSoundHandle(timerSfxHandle);
    idleSparkleEffect._vf10();
    idleSparkleEffect._vf18();
    idleSparkleEffect.clear();
    ringComplete = false;
    shouldSpawnNextWave = false;
    coinsCollectedThisWave = 0;
    waveFailedDueToTimeout = false;
    baseCenter = pos;
    int rewardValue = (this->settings >> kGreenRingRewardBitsShift) & kGreenRingRewardMask;
    rewardType = rewardValue < GREEN_RING_REWARD_COUNT ? rewardValue : kGreenRingRewardNone;
    rewardGranted = false;

    for (int i = 0; i < kCoinWaveSize; ++i)
        activeCoins[i] = 0;

    modelRotationOffsetY = 0;
    hasRingAnimation = false;
    hasRingTexSrtAnimation = false;
    hasFogModel = false;
    hasFogTexSrtAnimation = false;
    hasFogClrAnimation = false;
    idleSparklePos = pos;
    idleSparklePos.z += 20.0f;

    allocator.link(-1, GameHeaps[0], 0, 0x20);
    const char* ringArcName = "green_ring";
    const char* ringBrresName = "g3d/green_ring.brres";
    const char* preferredModelName = "green_ring";
    const char* fallbackModelName = "red_ring";
    const char* ringAnimName = "green_ring";
    const char* ringFogModelName = "green_ring_fog";
    const char* ringFogAnimName = "green_ring_fog";

    u8* arcData = getResource(ringArcName, ringBrresName);
    if (!arcData) {
        OSReport("GreenRing: falling back to obj_coin resource\n");
        arcData = getResource("obj_coin", "g3d/obj_coin.brres");
        fallbackModelName = "obj_coin_green";
    }

    if (!arcData) {
        allocator.unlink();
        return false;
    }

    nw4r::g3d::ResFile rf(arcData);

    void* preferredModel = rf.GetResMdl(preferredModelName);
    void* actualModel = preferredModel;

    if (!preferredModel) {
        OSReport("GreenRing: preferred model '%s' missing, trying '%s'\n", preferredModelName, fallbackModelName);
        actualModel = rf.GetResMdl(fallbackModelName);
    }

    if (!actualModel) {
        OSReport("GreenRing: could not load any model from %s\n", ringBrresName);
        allocator.unlink();
        return false;
    }

    nw4r::g3d::ResMdl ringModel = nw4r::g3d::ResMdl(actualModel);

    bodyModel.setup(ringModel, &allocator, 0x224, 1, 0);
    SetupTextures_MapObj(&bodyModel, 0);

    nw4r::g3d::ResAnmChr ringAnm = rf.GetResAnmChr(ringAnimName);
    if (ringAnm.data) {
        if (ringAnimation.setup(ringModel, ringAnm, &allocator, 0)) {
            ringAnimation.bind(&bodyModel, ringAnm, 1);
            ringAnimation.setUpdateRate(kGreenRingAnimationRate);
            bodyModel.bindAnim(&ringAnimation, 0.0f);
            hasRingAnimation = true;
        } else {
            OSReport("GreenRing: failed to setup animation '%s'\n", ringAnimName);
        }
    }

    nw4r::g3d::ResAnmTexSrt ringTexSrtAnm = rf.GetResAnmTexSrt(ringAnimName);
    if (ringTexSrtAnm.data) {
        if (ringTexSrtAnimation.setup(ringModel, ringTexSrtAnm, &allocator, 0, 1)) {
            ringTexSrtAnimation.bindEntry(&bodyModel, ringTexSrtAnm, 0, 1);
            ringTexSrtAnimation.setFrameForEntry(0.0f, 0);
            ringTexSrtAnimation.setUpdateRateForEntry(1.0f, 0);
            bodyModel.bindAnim(&ringTexSrtAnimation, 1.0f);
            hasRingTexSrtAnimation = true;
        } else {
            OSReport("GreenRing: failed to setup tex SRT animation '%s'\n", ringAnimName);
        }
    }

    nw4r::g3d::ResMdl fogRes = rf.GetResMdl(ringFogModelName);
    if (fogRes.data) {
        fogModel.setup(fogRes, &allocator, 0x32C, 1, 0);
        SetupTextures_MapObj(&fogModel, 0);
        hasFogModel = true;

        nw4r::g3d::ResAnmTexSrt fogTexSrtAnm = rf.GetResAnmTexSrt(ringFogAnimName);
        if (fogTexSrtAnm.data) {
            if (fogTexSrtAnimation.setup(fogRes, fogTexSrtAnm, &allocator, 0, 1)) {
                fogTexSrtAnimation.bindEntry(&fogModel, fogTexSrtAnm, 0, 1);
                fogTexSrtAnimation.setFrameForEntry(0.0f, 0);
                fogTexSrtAnimation.setUpdateRateForEntry(1.0f, 0);
                fogModel.bindAnim(&fogTexSrtAnimation, 1.0f);
                hasFogTexSrtAnimation = true;
            } else {
                OSReport("GreenRing: failed to setup fog tex SRT animation '%s'\n", ringFogAnimName);
            }
        }

        nw4r::g3d::ResAnmClr fogClrAnm = rf.GetResAnmClr(ringFogAnimName);
        if (fogClrAnm.data) {
            if (fogClrAnimation.setup(fogRes, fogClrAnm, &allocator, 0, 1)) {
                fogClrAnimation.bind(&fogModel, fogClrAnm, 0, 1);
                fogModel.bindAnim(&fogClrAnimation, 1.0f);
                hasFogClrAnimation = true;
            } else {
                OSReport("GreenRing: failed to setup fog color animation '%s'\n", ringFogAnimName);
            }
        }
    }

    allocator.unlink();

    ActivePhysics::Info hit;

    hit.xDistToCenter = 0;
    hit.yDistToCenter = kGreenRingVerticalOffset;
    hit.xDistToEdge = 10;
    hit.yDistToEdge = 16;
    hit.category1 = 0x5;
    hit.category2 = 0;
    hit.bitfield1 = 0x4F;
    hit.bitfield2 = kGreenRingBitfield2;
    hit.callback = &dEn_c::collisionCallback;

    aPhysics.initWithStruct(this, &hit);
    aPhysics.addToList();

    direction = getRingFacingDirection(this);
    rot.y = direction ? 0xE000 : 0x2000;

    updateModelMatrices();

    if (!idleSparkleEffect.probablyCreateWithName("Wm_ob_greenringkira", 0))
        idleSparkleEffect.spawn("Wm_ob_greenringkira", 0, &idleSparklePos, 0, 0);
    idleSparkleEffect._vfB0(&idleSparklePos, 0, 0);

    bodyModel._vf1C();
    if (hasFogModel)
        fogModel._vf1C();

    return true;
}

int daGreenRing_c::onDelete() {
    stopAndDetachSoundHandle(timerSfxHandle);
    return true;
}

void daGreenRing_c::playerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
    if (!apOther || !apOther->owner || !ownerIsPlayerOrYoshi(apOther->owner))
        return;

    if (activated || ringComplete)
        return;

    activated = true;
    rewardGranted = false;
    idleSparkleEffect._vf10();
    idleSparkleEffect._vf18();
    idleSparkleEffect.clear();
    spawnBasicEffect("Wm_ob_greenringget", idleSparklePos);
    spawnNextWave();
    PlaySoundAsync(this, SE_SYS_RED_RING);

    aPhysics.removeFromList();
}

void daGreenRing_c::spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
    return;
}

void daGreenRing_c::yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
}

bool daGreenRing_c::collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

static inline void stopAndDetachSoundHandle(nw4r::snd::SoundHandle& handle) {
    if (handle.Exists()) {
        handle.Stop(0);
        handle.DetachSound();
    }
}

static inline bool allPlayersDead() {
    for (int i = 0; i < 4; ++i) {
        if (Player_Active[i] == 0)
            continue;

        dAc_Py_c* playerActor = GetSpecificPlayerActor(i);
        if (!playerActor)
            continue;

        daPlBase_c* playerBase = (daPlBase_c*)playerActor;

        if (!playerBase->testFlag(4) && !playerBase->testFlag(0x24))
            return false;
    }

    return true;
}

static inline bool isStageEnding() {
    return GameMgrP && (GameMgrP->_B5A != 0);
}

static inline void startGreenTimerSound(nw4r::snd::SoundHandle& handle, dStageActor_c* actor) {
    stopAndDetachSoundHandle(handle);
    if (!actor)
        return;
    playSoundDistance(&handle, actor->pos, SFX_SE_OBJ_GREEN_TIMER, 1.0f, 1.0f);
}

static inline void spawnBasicEffect(const char* effectName, const Vec& effectPos) {
    if (!effectName)
        return;
    Vec mutablePos = effectPos;
    S16Vec efRot = {0, 0, 0};
    Vec efScale = {1.0f, 1.0f, 1.0f};
    SpawnEffect(effectName, 0, &mutablePos, &efRot, &efScale);
}

static inline u8 getRingFacingDirection(dEn_c* actor) {
    if (!actor)
        return 0;
    return dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(actor, actor->pos);
}

static inline void giveGreenCoinReward(int playerID) {
    if (playerID < 0 || playerID >= 4)
        return;

    Player_Coins[playerID] += 1;
    while (Player_Coins[playerID] >= 100) {
        Player_Coins[playerID] -= 100;
        Player_Lives[playerID] += 1;
    }
}

static inline void refreshRingIdleSparkleAfterPhysicsHit(daGreenRing_c* ring) {
    if (!ring || ring->activated || ring->ringComplete)
        return;

    ring->idleSparkleEffect._vf10();
    ring->idleSparkleEffect._vf18();
    ring->idleSparkleEffect.clear();
}

bool daGreenRing_c::collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenRing_c::collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenRing_c::collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenRing_c::collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenRing_c::collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenRing_c::collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenRing_c::collisionCat8_FencePunch(ActivePhysics* apThis, ActivePhysics* apOther) {
    return false;
}

bool daGreenRing_c::collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther) {
    return false;
}

bool daGreenRing_c::collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther) {
    refreshRingIdleSparkleAfterPhysicsHit(this);
    return false;
}

bool daGreenRing_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther) {
    refreshRingIdleSparkleAfterPhysicsHit(this);
    return false;
}

bool daGreenRing_c::collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther) {
    refreshRingIdleSparkleAfterPhysicsHit(this);
    return false;
}

bool daGreenRing_c::collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther) {
    refreshRingIdleSparkleAfterPhysicsHit(this);
    return false;
}

void daGreenRing_c::destroyActiveCoins() {
    for (int i = 0; i < kCoinWaveSize; ++i) {
        if (activeCoins[i]) {
            activeCoins[i]->Delete(1);
            activeCoins[i] = 0;
        }
    }

    coinsInWave = 0;
    tickTimer = 0;
}

void daGreenRing_c::finishRing() {
    destroyActiveCoins();
    stopAndDetachSoundHandle(timerSfxHandle);
    ringComplete = true;
    waveTimer = 0;
    tickTimer = 0;
    timerPlayedThisWave = false;
    shouldSpawnNextWave = false;
    resetGreenRingWaveCount(ringId);
}

Vec daGreenRing_c::getWaveCenter(int waveIndex) const {
    if (ringId >= 0 && ringId < kMaxGreenRingId && waveIndex >= 0 && waveIndex < kTotalWaves) {
        if (gGreenCoinWaveCenterValid[ringId][waveIndex])
            return gGreenCoinWaveCenters[ringId][waveIndex];
    }

    return pos;
}

void daGreenRing_c::playRedCoinWaveSound(int waveIndex) {
    static const int redCoinWaveSounds[kTotalWaves] = {
        SE_OBJ_GET_RED_COIN,
        SE_OBJ_GET_RED_COIN_2,
        SE_OBJ_GET_RED_COIN_3,
        SE_OBJ_GET_RED_COIN_4,
        SE_OBJ_GET_RED_COIN_COMPLETE
    };

    if (waveIndex < 0 || waveIndex >= kTotalWaves)
        return;

    PlaySound((dStageActor_c*)this, redCoinWaveSounds[waveIndex]);
}

void daGreenRing_c::spawnNextWave() {
    if (ringComplete)
        return;

    if (wavesSpawned >= kTotalWaves) {
        finishRing();
        return;
    }

    destroyActiveCoins();

    waveTimer = kWaveTimeoutFrames;
    stopAndDetachSoundHandle(timerSfxHandle);
    tickTimer = kTimerStartDelayFrames;
    timerPlayedThisWave = false;
    coinsInWave = 0;
    coinsCollectedThisWave = 0;
    waveFailedDueToTimeout = false;

    int waveIndex = wavesSpawned;
    bool hasExplicitLinkId = (ringId != 0);

    if (hasExplicitLinkId && !hasLinkedWaveCenter(ringId, waveIndex)) {
        finishRing();
        return;
    }

    Vec waveCenter;

    if (waveIndex == 0) {
        waveCenter = getWaveCenter(0);
        baseCenter = waveCenter;
    } else {
        waveCenter = getWaveCenter(waveIndex);

        if (!hasLinkedWaveCenter(ringId, waveIndex))
            waveCenter = baseCenter;
    }

    for (int i = 0; i < kCoinWaveSize; ++i) {
        daGreenCoin_c* coin =
            (daGreenCoin_c*)dStageActor_c::create(
                ProfileId::EN_GREENCOINU,
                kGreenCoinDynamicFlag,
                &waveCenter,
                0,
                0
            );

        if (!coin)
            continue;

        coin->ring = this;
        coin->orbitIndex = i;
        coin->waveIndex = waveIndex;
        coin->orbitCenter = waveCenter;
        coin->spinOffset = i * kCoinSpinPhaseDelta;
        activeCoins[i] = coin;

        if (coin->makeVisible())
            ++coinsInWave;
        else
            activeCoins[i] = 0;
    }

    if (coinsInWave > 0)
        wavesSpawned++;
    else
        finishRing();
}

void daGreenRing_c::coinCollected(daGreenCoin_c* coin, int playerID) {
    if (!coin)
        return;

    int completedWaveIndex = coin->waveIndex;

    for (int i = 0; i < kCoinWaveSize; ++i) {
        if (activeCoins[i] == coin) {
            activeCoins[i] = 0;
            break;
        }
    }

    if (coinsInWave > 0)
        --coinsInWave;

    coinsCollectedThisWave++;

    bool waveCompletedByPlayer = coinsCollectedThisWave == kCoinWaveSize;

    if (coinsInWave <= 0) {
        stopAndDetachSoundHandle(timerSfxHandle);
        waveTimer = 0;
        tickTimer = 0;
        timerPlayedThisWave = false;
        shouldSpawnNextWave = true;
        if (waveCompletedByPlayer && !waveFailedDueToTimeout) {
            handleGreenRingWaveReward(this, completedWaveIndex, playerID);
            if (completedWaveIndex == kTotalWaves - 1) {
                grantReward(playerID);
            }
        }
    }
}

void daGreenRing_c::grantReward(int playerID) {
    if (rewardGranted || rewardType == kGreenRingRewardNone || playerID < 0)
        return;

    dAc_Py_c* player = GetSpecificPlayerActor(playerID);
    if (!player)
        return;

    u32 rewardId = playerHasThirdStagePowerup(player) ? kRewardOneUpItemId : kRewardSettings[rewardType];

    Vec rewardSpawnPos = pos;
    rewardSpawnPos.y += kRewardHoldOffsetY;

    u32 settings = makeGreenRingRewardSettings(rewardId, playerID);
    dStageActor_c* rewardActor = dStageActor_c::create(ProfileId::GreenRingReward, settings,
                                                       (Point3d*)&rewardSpawnPos, 0, 0);
    if (!rewardActor)
        return;

    rewardGranted = true;
}

static void handleGreenRingWaveReward(daGreenRing_c* ring, int completedWaveIndex, int playerID) {
    if (!ring)
        return;

    int id = ring->ringId;
    if (id < 0 || id >= kMaxGreenRingId)
        id = 0;

    int nextWave = gGreenRingWaveCounts[id] + 1;
    if (nextWave > kTotalWaves)
        nextWave = kTotalWaves;

    gGreenRingWaveCounts[id] = nextWave;
    gGreenRingPopupGuard = 20;
    Vec popupPos = ring->getWaveCenter(completedWaveIndex);
    showWavePopup(popupPos, nextWave, playerID);
    if (nextWave == kTotalWaves)
        enableBigSmallAnimOnLatestSmallScorePopup();
    ring->playRedCoinWaveSound(completedWaveIndex);
    if (completedWaveIndex == kTotalWaves - 1) {
        PlaySoundAsync((dStageActor_c*)ring, SE_SYS_NICE_S);
    }
}

int daGreenRing_c::onExecute() {
    const bool playersAreDead = allPlayersDead();
    const bool stageEnding = isStageEnding();
    const bool shouldForceStopTimer = playersAreDead || stageEnding;

    if (timerSfxHandle.Exists() && shouldForceStopTimer)
        stopAndDetachSoundHandle(timerSfxHandle);

    if (shouldForceStopTimer) {
        timerPlayedThisWave = true;
        return true;
    }

    if (gGreenRingPopupGuard > 0)
        --gGreenRingPopupGuard;

    if (!activated) {
        if (!idleSparkleEffect.probablyCreateWithName("Wm_ob_greenringkira", 0))
            idleSparkleEffect.spawn("Wm_ob_greenringkira", 0, &idleSparklePos, 0, 0);
        idleSparkleEffect._vfB0(&idleSparklePos, 0, 0);

        if (hasRingAnimation) {
            ringAnimation.process();
            if (ringAnimation.isAnimationDone())
                ringAnimation.setCurrentFrame(0.0f);
        }

        if (hasRingTexSrtAnimation) {
            ringTexSrtAnimation.process();
            if (ringTexSrtAnimation.isEntryAnimationDone(0))
                ringTexSrtAnimation.setFrameForEntry(0.0f, 0);
        }

        if (hasFogTexSrtAnimation) {
            fogTexSrtAnimation.process();
            if (fogTexSrtAnimation.isEntryAnimationDone(0))
                fogTexSrtAnimation.setFrameForEntry(0.0f, 0);
        }

        if (hasFogClrAnimation) {
            fogClrAnimation.process();
        }

        updateModelMatrices();
        bodyModel._vf1C();
        if (hasFogModel)
            fogModel._vf1C();

    } else if (coinsInWave > 0) {
        orbitAngle += orbitSpeed;
        orbitAngle = wrapRadians(orbitAngle);

        if (waveTimer > 0) {
            if (--waveTimer <= 0) {
                destroyActiveCoins();
                waveFailedDueToTimeout = true;
                finishRing();
            } else {
                if (waveTimer > kTimerEndDelayFrames) {
                    if (!timerPlayedThisWave) {
                        if (tickTimer > 0)
                            --tickTimer;
                    }
                    if (!timerPlayedThisWave && tickTimer == 0) {
                        startGreenTimerSound(timerSfxHandle, this);
                        timerPlayedThisWave = true;
                    }
                }
            }
        }

    } else if (shouldSpawnNextWave) {
        shouldSpawnNextWave = false;
        spawnNextWave();

    } else if (wavesSpawned >= kTotalWaves && !ringComplete) {
        finishRing();
    }

    return true;
}

int daGreenRing_c::onDraw() {
    if (timerSfxHandle.Exists() && (allPlayersDead() || isStageEnding()))
        stopAndDetachSoundHandle(timerSfxHandle);

    if (!activated) {
        if (hasFogModel)
            fogModel.scheduleForDrawing();
        bodyModel.scheduleForDrawing();
    }

    return true;
}

void daGreenRing_c::updateIdleSparkleAnchor() {
    Mtx nodeMatrix;
    if (bodyModel.getMatrixForNode(0, &nodeMatrix)) {
        idleSparklePos.x = nodeMatrix[0][3];
        idleSparklePos.y = nodeMatrix[1][3];
        idleSparklePos.z = nodeMatrix[2][3];
    } else {
        idleSparklePos = pos;
        idleSparklePos.z += 20.0f;
    }
}

void daGreenRing_c::updateModelMatrices() {
    Vec transformedPos = pos;
    S16Vec transformedRot = rot;
    transformedPos.y += kGreenRingVerticalOffset;

    matrix.translation(transformedPos.x, transformedPos.y, transformedPos.z);
    matrix.applyRotationYXZ(&transformedRot.x, &transformedRot.y, &transformedRot.z);

    bodyModel.setDrawMatrix(matrix);
    bodyModel.setScale(scale.x, scale.y, scale.z);
    bodyModel.calcWorld(false);
    updateIdleSparkleAnchor();

    if (hasFogModel) {
        Vec fogPos = pos;
        fogPos.y += kGreenRingVerticalOffset;

        mMtx fogMatrix;
        fogMatrix.translation(fogPos.x, fogPos.y, fogPos.z - kGreenRingFogDepthOffset);
        fogModel.setDrawMatrix(fogMatrix);
        fogModel.setScale(scale.x, scale.y, scale.z);
        fogModel.calcWorld(false);
    }
}

int daGreenCoin_c::onCreate() {
    visible = false;
    drawVisible = false;
    orbitIndex = -1;
    waveIndex = -1;
    ring = 0;
    dynamicCoin = (settings & kGreenCoinDynamicFlag) != 0;
    orbitCenter = pos;
    spinAngle = 0;
    spinOffset = 0;

    if (!dynamicCoin) {
        int coinRingID = extractGreenRingLinkId(settings);
        int markerWave = decodeMarkerWaveIndex(settings);
        registerGlobalGreenCoinWaveCenter(coinRingID, markerWave, this->pos);
        Delete(1);
        return true;
    }

    return true;
}

int daGreenCoin_c::makeVisible() {
    visible = true;
    drawVisible = true;
    if (ring)
        pos = orbitCenter;

    spinAngle = spinOffset;

    speed.x = speed.y = speed.z = 0.0f;
    max_speed.x = max_speed.y = max_speed.z = 0.0f;

    allocator.link(-1, GameHeaps[0], 0, 0x20);

    nw4r::g3d::ResFile localRes;
    localRes.data = getResource("obj_coin", "g3d/obj_coin.brres");
    if (!localRes.data) {
        allocator.unlink();
        return false;
    }

    nw4r::g3d::ResMdl coinModel = localRes.GetResMdl("obj_coin_green");
    if (!coinModel.data)
        coinModel = localRes.GetResMdl("obj_coin");
    if (!coinModel.data) {
        allocator.unlink();
        return false;
    }

    bodyModel.setup(coinModel, &allocator, 0x224, 1, 0);
    SetupTextures_MapObj(&bodyModel, 0);

    allocator.unlink();

    ActivePhysics::Info hit;

    hit.xDistToCenter = 0;
    hit.yDistToCenter = 0;
    hit.xDistToEdge = 6;
    hit.yDistToEdge = 6;
    hit.category1 = 0x5;
    hit.category2 = 0;
    hit.bitfield1 = 0x4F;
    hit.bitfield2 = kGreenRingBitfield2;
    hit.callback = &daGreenCoin_c::greenCoinCollision;

    aPhysics.initWithStruct(this, &hit);
    aPhysics.addToList();

    return true;
}

int daGreenCoin_c::onExecute() {
    if (!visible || !ring)
        return true;

    float angle = ring->orbitAngle + orbitIndex * kCoinAngleStep;
    float sinVal = fastSin(angle);
    float cosVal = fastCos(angle);

    pos.x = orbitCenter.x + ring->orbitRadius * cosVal;
    pos.y = orbitCenter.y + ring->orbitRadius * sinVal;
    pos.z = orbitCenter.z;

    spinAngle += kCoinSpinRate;

    updateModelMatrices();
    bodyModel._vf1C();

    return true;
}

int daGreenCoin_c::onDraw() {
    if (!drawVisible)
        return true;

    bodyModel.scheduleForDrawing();

    return true;
}

void daGreenCoin_c::playerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
    int playerID = -1;

    if (apOther && apOther->owner) {
        u16 profile = apOther->owner->profileId;
        if (profile == ProfileId::PLAYER || profile == ProfileId::YOSHI)
            playerID = apOther->owner->which_player;
    }

    bool completesWave = false;
    if (ring)
        completesWave = (ring->coinsCollectedThisWave == (kCoinWaveSize - 1));

    if (ring)
        ring->coinCollected(this, playerID);

    Vec efPos = pos;
    spawnBasicEffect("Wm_ob_greenncoinkira", efPos);

    giveGreenCoinReward(playerID);

    if (!completesWave)
        PlaySoundAsync(this, SE_OBJ_GET_COIN);

    Delete(1);
}

void daGreenCoin_c::spriteCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
    return;
}

void daGreenCoin_c::yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
}

bool daGreenCoin_c::collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenCoin_c::collisionCat5_Mario(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenCoin_c::collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenCoin_c::collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenCoin_c::collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenCoin_c::collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenCoin_c::collisionCat11_PipeCannon(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenCoin_c::collisionCat8_FencePunch(ActivePhysics* apThis, ActivePhysics* apOther) {
    return false;
}

bool daGreenCoin_c::collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther) {
    playerCollision(apThis, apOther);
    return false;
}

bool daGreenCoin_c::collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther) {
    return false;
}

bool daGreenCoin_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther) {
    return false;
}

bool daGreenCoin_c::collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther) {
    return false;
}

bool daGreenCoin_c::collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther) {
    return false;
}

void daGreenCoin_c::greenCoinCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
    if (!apOther || !apOther->owner || !ownerIsPlayerOrYoshi(apOther->owner))
        return;

    u16 profile = apOther->owner->profileId;
    if (profile != ProfileId::PLAYER && profile != ProfileId::YOSHI)
        return;

    dEn_c::collisionCallback(apThis, apOther);
}

void daGreenCoin_c::updateModelMatrices() {
    Vec toCenter;
    toCenter.x = orbitCenter.x - pos.x;
    toCenter.y = orbitCenter.y - pos.y;
    toCenter.z = orbitCenter.z - pos.z;

    Vec targetDir = kCoinFrontNormal;
    float centerDistanceSq = toCenter.x * toCenter.x + toCenter.y * toCenter.y + toCenter.z * toCenter.z;

    if (centerDistanceSq > 0.0f) {
        float invLength = 1.0f / sqrtf(centerDistanceSq);
        targetDir.x = toCenter.x * invLength;
        targetDir.y = toCenter.y * invLength;
        targetDir.z = toCenter.z * invLength;
    }

    Vec tiltAxis;
    tiltAxis.x = kCoinFrontNormal.y * targetDir.z - kCoinFrontNormal.z * targetDir.y;
    tiltAxis.y = kCoinFrontNormal.z * targetDir.x - kCoinFrontNormal.x * targetDir.z;
    tiltAxis.z = kCoinFrontNormal.x * targetDir.y - kCoinFrontNormal.y * targetDir.x;

    float axisLenSq = tiltAxis.x * tiltAxis.x + tiltAxis.y * tiltAxis.y + tiltAxis.z * tiltAxis.z;
    float dot = kCoinFrontNormal.x * targetDir.x + kCoinFrontNormal.y * targetDir.y + kCoinFrontNormal.z * targetDir.z;

    Mtx spinMatrix;
    float spinRad = (float)spinAngle * kCoinSpinAngleToRadians;
    Vec spinAxis = kCoinFrontNormal;
    MTXRotAxisRad(spinMatrix, &spinAxis, spinRad);

    Mtx rotationMatrix;
    if (axisLenSq > kCoinAxisLengthThreshold) {
        float axisLen = sqrtf(axisLenSq);
        Vec axisNorm;
        axisNorm.x = tiltAxis.x / axisLen;
        axisNorm.y = tiltAxis.y / axisLen;
        axisNorm.z = tiltAxis.z / axisLen;

        float clampedDot = clampFloat(dot, -1.0f, 1.0f);
        float tiltAngle = fastAtan2(axisLen, clampedDot);

        if (fastAbs(tiltAngle) > kCoinTiltAngleThreshold) {
            Mtx tiltMatrix;
            MTXRotAxisRad(tiltMatrix, &axisNorm, tiltAngle);
            MTXConcat(tiltMatrix, spinMatrix, rotationMatrix);
        } else {
            MTXCopy(spinMatrix, rotationMatrix);
        }
    } else if (dot < -0.9999f) {
        Mtx tiltMatrix;
        Vec altAxis = {1.0f, 0.0f, 0.0f};
        MTXRotAxisRad(tiltMatrix, &altAxis, kPi);
        MTXConcat(tiltMatrix, spinMatrix, rotationMatrix);
    } else {
        MTXCopy(spinMatrix, rotationMatrix);
    }

    rotationMatrix[0][3] = pos.x;
    rotationMatrix[1][3] = pos.y;
    rotationMatrix[2][3] = pos.z;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col)
            matrix[row][col] = rotationMatrix[row][col];
    }

    bodyModel.setDrawMatrix(matrix);
    bodyModel.calcWorld(false);
}

class daGreenRingRewardItem_c : public dEn_c {
public:
    u32 rewardId;
    int collectorPlayerId;
    bool handled;

    static dActor_c* build();

    int onCreate();
    int onExecute();
};

dActor_c* daGreenRingRewardItem_c::build() {
    void* buffer = AllocFromGameHeap1(sizeof(daGreenRingRewardItem_c));
    return new(buffer) daGreenRingRewardItem_c;
}

int daGreenRingRewardItem_c::onCreate() {
    rewardId = extractGreenRingRewardId(settings);
    collectorPlayerId = extractGreenRingCollectorPlayer(settings);
    handled = false;
    return true;
}

int daGreenRingRewardItem_c::onExecute() {
    if (handled)
        return true;

    handled = true;

    GreenRingRewardCandidate candidates[kMaxRewardPlayerIndex];
    int count = collectGreenRingRewardCandidates(candidates, kMaxRewardPlayerIndex);

    if (count == 0) {
        daPlBase_c* fallback = GetPlayerOrYoshi(collectorPlayerId);
        if (fallback) {
            candidates[0].actor = fallback;
            candidates[0].playerIndex = collectorPlayerId;
            count = 1;
        }
    }

    if (count > 0)
        gGreenRingSuppressItemFallEffectCount = count;

    for (int i = 0; i < count; ++i) {
        daPlBase_c* actor = candidates[i].actor;
        if (!actor)
            continue;

        int voice = -1;
        int slot = candidates[i].playerIndex;
        if (slot < 0 || slot > 3)
            slot = 0;

        switch (Player_ID[slot]) {
            case 0: voice = SE_VOC_MA_ITEM_COMPLETE; break;
            case 1: voice = SE_VOC_LU_ITEM_COMPLETE; break;
            case 2: voice = SE_VOC_KO_ITEM_COMPLETE; break;
            case 3: voice = SE_VOC_KO2_ITEM_COMPLETE; break;
            default: break;
        }

        if (voice != -1)
            PlaySoundAsync((dStageActor_c*)actor, voice);

        dAc_Py_c* player = GetSpecificPlayerActor(candidates[i].playerIndex);
        u32 candidateRewardId = (player && playerHasThirdStagePowerup(player)) ? kRewardOneUpItemId : rewardId;

        Vec dropPos = actor->pos;
        dropPos.y += kRewardHoldOffsetY;

        dStageActor_c* spawned = spawnRewardItem(candidateRewardId, dropPos, player, candidates[i].playerIndex);
        if (spawned)
            PlaySoundAsync(spawned, SE_SYS_STOCK_ITEM_USE);
    }

    Delete(1);
    return true;
}

Profile GreenRingRewardProfile(
    &daGreenRingRewardItem_c::build,
    ProfileId::GreenRingReward,
    NULL,
    ProfileId::GreenRingReward,
    ProfileId::GreenRingReward,
    "GreenRingReward",
    NULL,
    0
);
