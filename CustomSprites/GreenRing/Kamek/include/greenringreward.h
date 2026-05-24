#pragma once

#include <common.h>
#include <game.h>
#include <actors.h>
#include <profile.h>
#include <profileid.h>
#include <sfx.h>

enum GreenRingRewardType {
    GREEN_RING_REWARD_FIRE = 0,
    GREEN_RING_REWARD_ICE = 1,
    GREEN_RING_REWARD_PROPELLER = 2,
    GREEN_RING_REWARD_PENGUIN = 3,
    GREEN_RING_REWARD_HAMMER = 4,
    GREEN_RING_REWARD_STAR = 5,
    GREEN_RING_REWARD_COUNT
};

struct GreenRingRewardCandidate {
    daPlBase_c* actor;
    int playerIndex;
};

static const int kTotalWaves = 5;
static const int kMaxGreenRingId = 16;
static const int kMaxRewardPlayerIndex = 4;
static const u32 kRewardPlayerShiftDelta = 0x10000;
static const u32 kRewardSpawnFlags = 0x2080;
static const float kRewardHoldOffsetY = 120.0f;
static const float kRewardDropReleaseSpeedY = -600.0f;
static const float kRewardDropReleaseMaxSpeedY = 600.0f;
static const int kGreenRingRewardBitsShift = 24;
static const int kGreenRingRewardMask = 0xF;
static const int kGreenRingRewardPlayerIdShift = 8;
static const u32 kGreenRingRewardPlayerIdMask = 0xFF;
static const u32 kRewardItemProfile = ProfileId::EN_ITEM;
static const int kGreenRingRewardNone = -1;

static const u32 kRewardSettings[GREEN_RING_REWARD_COUNT] = {
    9,
    14,
    21,
    17,
    6,
    1
};

static const u32 kRewardOneUpItemId = 7;

inline u16 makeChildSettings(u32 rawSettings) {
    return (u16)rawSettings;
}

inline u32 makeGreenRingRewardSettings(u32 rewardId, int collectorPlayerId) {
    return (rewardId << kGreenRingRewardPlayerIdShift) |
           ((u32)collectorPlayerId & kGreenRingRewardPlayerIdMask);
}

inline u32 extractGreenRingRewardId(u32 settings) {
    return settings >> kGreenRingRewardPlayerIdShift;
}

inline int extractGreenRingCollectorPlayer(u32 settings) {
    return (int)(settings & kGreenRingRewardPlayerIdMask);
}

static const u32 kPlayerModelHandlerOffset = 0x2A60;
static const u32 kPlayerPowerupOffset = 0x1090;
static const u8 kFirePowerupId = 2;
static const u8 kIcePowerupId = 6;
static const u8 kPropellerPowerupId = 3;
static const u8 kPenguinPowerupId = 5;
static const u8 kHammerPowerupId = 7;

inline dPlayerModelHandler_c* getPlayerModelHandler(dAc_Py_c* player) {
    if (!player)
        return NULL;
    return (dPlayerModelHandler_c*)((u8*)player + kPlayerModelHandlerOffset);
}

inline u8 getPlayerModelPowerupId(dAc_Py_c* player) {
    dPlayerModelHandler_c* handler = getPlayerModelHandler(player);
    if (!handler || !handler->mdlClass)
        return 0xFF;
    return handler->mdlClass->powerup_id;
}

inline u8 getPlayerActualPowerupId(dAc_Py_c* player) {
    if (!player)
        return 0xFF;
    return (u8)(*(int*)((u8*)player + kPlayerPowerupOffset));
}

inline bool isThirdStagePowerupId(u8 powerupId) {
    return powerupId == kFirePowerupId || powerupId == kIcePowerupId ||
           powerupId == kPropellerPowerupId || powerupId == kPenguinPowerupId ||
           powerupId == kHammerPowerupId;
}

inline bool playerHasThirdStagePowerup(dAc_Py_c* player) {
    if (!player)
        return false;

    u8 actualPowerupId = getPlayerActualPowerupId(player);
    u8 modelPowerupId = getPlayerModelPowerupId(player);

    return isThirdStagePowerupId(actualPowerupId) || isThirdStagePowerupId(modelPowerupId);
}

inline bool isRewardCandidate(daPlBase_c* actor) {
    if (!actor)
        return false;

    if (actor->profileId != ProfileId::PLAYER && actor->profileId != ProfileId::YOSHI)
        return false;

    if (actor->which_player < 0 || actor->which_player >= kMaxRewardPlayerIndex)
        return false;

    return true;
}

inline int collectGreenRingRewardCandidates(GreenRingRewardCandidate* buffer, int maxCandidates) {
    int count = 0;
    for (int idx = 0; idx < kMaxRewardPlayerIndex && count < maxCandidates; ++idx) {
        daPlBase_c* actor = GetPlayerOrYoshi(idx);
        if (!isRewardCandidate(actor))
            continue;

        buffer[count].actor = actor;
        buffer[count].playerIndex = idx;
        ++count;
    }
    return count;
}

inline dStageActor_c* spawnRewardItem(u32 rewardId, const Vec& spawnPos, dAc_Py_c* player, int playerIndex) {
    (void)player;
    u16 childSettings = makeChildSettings(rewardId);
    S16Vec zeroRot = {0, 0, 0};

    u32 playerFlag = 0;
    if (playerIndex >= 0 && playerIndex < kMaxRewardPlayerIndex)
        playerFlag = (u32)playerIndex * kRewardPlayerShiftDelta;

    u32 spawnSettings = (u32)childSettings | playerFlag | kRewardSpawnFlags;

    dStageActor_c* realItem = dStageActor_c::create((u16)kRewardItemProfile, spawnSettings,
                                                    (Vec*)&spawnPos, &zeroRot, 0);
    if (!realItem)
        return NULL;

    *(int*)((u8*)realItem + 0xDB0) = 0;
    realItem->speed = (Vec){0.0f, kRewardDropReleaseSpeedY, 0.0f};
    realItem->max_speed = (Vec){0.0f, kRewardDropReleaseMaxSpeedY, 0.0f};

    return realItem;
}
