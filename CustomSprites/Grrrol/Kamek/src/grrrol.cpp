#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <profile.h>
#include <sfx.h>
#include <timekeeper.h>
#include <UnitType.h>

const char* GrrrolArcNameList [] = {
	"grrrol",
	NULL
};

extern "C" void AddScore(int howMany, int playerID);
extern "C" void DisplayScoreAddPopup(Vec pos, int smallScoreID, int playerID, bool wantToShowThisForever);

class daGrrrol_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c model;
	m3d::anmTexSrt_c srtAnimation;
	mEf::es2 crashEffect;
	mEf::es2 rollEffect;
	nw4r::snd::SoundHandle rollSoundHandle;
	mMtx matrix;
	bool modelLoaded;
	bool srtLoaded;

	float moveSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	bool isMega;
	bool noSlowWallBounce;
	bool suppressLandingBounce;
	bool suppressMicroBounceLanding;
	bool spawnedFromSpawner;
	bool pipeDropSpawn;
	bool pipeDropWaitingForGround;
	float pipeDropSpawnStartY;
	bool freezeEyeRotation;
	bool rollingOnSpawnerGuide;
	bool activatedByView;
	u8 crashEffectTimer;
	u8 rollEffectTimer;
	u16 rollTimer;
	s16 deathRotX;

	void updateModelMatrices();
	bool calculateTileCollisions();
	bool handleSpawnerGuideCollision();
	void reverseDirection(bool doHop, bool slowDown);
	void spawnCrashEffects();
	void updateCrashEffect();
	void stopCrashEffect();
	void spawnLandingEffects();
	void updateRollSound();
	void stopRollSound();
	void updateRollEffect();
	void stopRollEffect();
	bool breakBlocksAhead(u8 movementDirection);
	bool breakMegaTopBlock(u8 movementDirection);
	bool breakMegaBlockSpritesAhead(u8 movementDirection);
	void setEyeRotationFrozen(bool frozen);
	void dieSpinOffscreen(dStageActor_c *killer, float xSpeed = 0.0f, bool awardScore = true);

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	void _vf140(dStageActor_c *actor);
	void powBlockActivated(bool isNotMPGP);
	u32 canBePowed();

	USING_STATES(daGrrrol_c);
	DECLARE_STATE(Roll);

	public: static dActor_c *build();
};

dActor_c *daGrrrol_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daGrrrol_c));
	return new(buffer) daGrrrol_c;
}

const SpriteData GrrrolSpriteData = { ProfileId::Grrrol, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile GrrrolProfile(&daGrrrol_c::build, SpriteId::Grrrol, &GrrrolSpriteData, ProfileId::Grrrol, ProfileId::Grrrol, "Grrrol", GrrrolArcNameList, 0);

CREATE_STATE(daGrrrol_c, Roll);

static const float GrrrolBounceSpeed = 1.3f;
static const float GrrrolRollingBounceSpeed = 0.45f;
static const u16 GrrrolRollingBounceInterval = 8;

static const float GrrrolAcceleration = 0.12f;
static const float GrrrolWallSlowdownMultiplier = 0.5f;
static const float GrrrolWallSlowdownFrames = 45.0f;
static const float GrrrolPipeDropMinimumFallDistance = 32.0f;
static const float GrrrolPipeDropWaitingSpeed = -0.7f;
static const float GrrrolPipeDropNormalGravity = -0.1875f;
static const u8 GrrrolRollEffectFrames = 8;
static const int GrrrolNormalSideSensorBelow = 12;
static const int GrrrolNormalSideSensorAbove = 20;
static const int GrrrolNormalSideSensorDistance = 15;
static const int GrrrolMegaSideSensorBelow = 4;
static const int GrrrolMegaSideSensorAbove = 60;
static const int GrrrolMegaSideSensorDistance = 26;
static const float GrrrolMegaBlockSpriteBehindSensor = 16.0f;
static const float GrrrolMegaBlockSpriteAheadSensor = 17.0f;
static const u32 GrrrolMegaWallSensorFlags =
	SENSOR_10000000 | SENSOR_BREAK_BLOCK | SENSOR_BREAK_BRICK | SENSOR_80000000;
static bool GrrrolLayerExists(u8 layer) {
	if (layer >= 3)
		return false;
	if (!BGDatClass::instance || !dScStage_c::instance)
		return false;

	u8 area = dScStage_c::instance->curArea;
	if (area >= 4)
		return false;

	return BGDatClass::instance->tilemaps[area][layer] != 0;
}

static bool GrrrolIsBreakableBlockTile(u16 tile) {
	u16 tileNum = tile & 0x3FF;

	return tileNum >= 0x1B && tileNum <= 0x34;
}

static u16 GrrrolBlockTileShardType(u16 tile) {
	u16 tileNum = tile & 0x3FF;

	switch (tileNum) {
		case 0x31: return 3;
		case 0x32: return 4;
		case 0x33: return 2;
		case 0x34: return 1;
		default: return 0;
	}
}

static u32 GrrrolBreakVelocityForDirection(u8 movementDirection) {
	return movementDirection ? 1 : 0;
}

static void GrrrolPlayBlockBreakSoundAt(dStageActor_c *source, Vec *stagePos) {
	if (SoundPlayingClass::instance2) {
		Vec2 soundPos;
		ConvertStagePositionToScreenPosition(&soundPos, stagePos);
		SoundPlayingClass::instance2->PlaySoundAtPosition(SE_OBJ_BLOCK_BREAK, &soundPos, 0);
		return;
	}

	if (source)
		PlaySound(source, SE_OBJ_BLOCK_BREAK);
}

static bool GrrrolIsBreakableBlockSprite(u16 profileId) {
	return profileId == ProfileId::EN_OBJ_HATENA_BLOCK
		|| profileId == ProfileId::EN_OBJ_RENGA_BLOCK;
}

static bool GrrrolBreakBlockSprite(daGrrrol_c *source, dStageActor_c *block, u8 movementDirection) {
	if (!block || block->isDeleted)
		return false;

	daEnBlockMain_c *blockMain = (daEnBlockMain_c*)block;
	u8 playerId = blockMain->playerID;
	bool noPlayer = playerId == 0xFF;
	u32 settings = ((u32)blockMain->_68D << 4)
		| (1 << 8)
		| ((u32)(movementDirection & 3) << 24)
		| ((noPlayer ? 0 : (playerId & 3)) << 16)
		| ((u32)noPlayer << 18);
	Vec breakPos = {
		block->pos.x - 8.0f,
		block->pos.y + 8.0f,
		block->pos.z
	};

	dStageActor_c::create(ProfileId::EN_BKBLOCK, settings, &breakPos, 0, block->currentLayerID);
	GrrrolPlayBlockBreakSoundAt(source, &breakPos);
	block->Delete(1);
	return true;
}

static bool GrrrolUsesSandLandSmoke(u8 footnote) {
	return footnote == daPlBase_c::FOOTNOTE_SAND
		|| footnote == daPlBase_c::FOOTNOTE_BLOWSAND
		|| footnote == daPlBase_c::FOOTNOTE_SAND_2;
}

static bool GrrrolShouldIgnoreSprite(u16 profileId) {
	return profileId == EN_COIN || profileId == EN_EATCOIN || profileId == AC_BLOCK_COIN
		|| profileId == EN_COIN_JUGEM || profileId == EN_COIN_ANGLE || profileId == EN_COIN_JUMP
		|| profileId == EN_COIN_FLOOR || profileId == EN_COIN_VOLT || profileId == EN_COIN_WIND
		|| profileId == EN_BLUE_COIN || profileId == EN_COIN_WATER || profileId == EN_REDCOIN
		|| profileId == EN_GREENCOIN || profileId == EN_JUMPDAI || profileId == EN_ITEM;
}

static float GrrrolAbsSpeed(float speed) {
	return speed < 0.0f ? -speed : speed;
}

static u8 GrrrolDirectionToNearestPlayer(Vec pos, u8 fallbackDirection) {
	float nearestDistance = 1000000.0f;
	u8 result = fallbackDirection;

	for (int i = 0; i < 4; i++) {
		dAc_Py_c *player = GetSpecificPlayerActor(i);
		if (!player)
			continue;

		float distance = player->pos.x - pos.x;
		float absDistance = distance < 0.0f ? -distance : distance;
		if (absDistance < nearestDistance) {
			nearestDistance = absDistance;
			result = distance > 0.0f ? 0 : 1;
		}
	}

	return result;
}

static bool GrrrolStartsFacingAnyPlayer(Vec pos, u8 direction) {
	bool foundPlayer = false;

	for (int i = 0; i < 4; i++) {
		dAc_Py_c *player = GetSpecificPlayerActor(i);
		if (!player)
			continue;

		foundPlayer = true;
		bool playerIsRight = player->pos.x > pos.x;
		if ((direction == 0 && playerIsRight) || (direction == 1 && !playerIsRight))
			return true;
	}

	return !foundPlayer;
}

static bool GrrrolInRange(float value, float min, float max) {
	return value >= min && value <= max;
}

static float GrrrolSpawnerPipePixelsFromSettings(u32 settings) {
	u8 pipeLength = (settings >> 24) & 0xF;
	return ((float)pipeLength + 1.0f) * 16.0f;
}

static bool GrrrolIsSpawnerPhysics(Physics *sColl) {
	return sColl && sColl->owner && !sColl->owner->isDeleted
		&& sColl->owner->profileId == ProfileId::GrrrolSpawner;
}

static bool GrrrolShouldDespawnFromCamera(const Vec &pos) {
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	if (!cwci)
		return false;

	float xDistance = pos.x - cwci->screenCentreX;
	float yDistance = pos.y - cwci->screenCentreY;
	float xLimit = cwci->screenWidth * 1.5f;
	float yLimit = cwci->screenHeight * 1.5f;

	return xDistance < -xLimit || xDistance > xLimit
		|| yDistance < -yLimit || yDistance > yLimit;
}

static void DamagePlayerFromGrrrol(daGrrrol_c *actor, ActivePhysics *apThis, ActivePhysics *apOther) {
	actor->dEn_c::playerCollision(apThis, apOther);
	actor->_vf220(apOther->owner);

	actor->deathInfo.isDead = 0;
	actor->flags_4FC |= (1 << (31 - 7));

	if (apOther->owner->which_player == 255) {
		actor->counter_504[0] = 0;
		actor->counter_504[1] = 0;
		actor->counter_504[2] = 0;
		actor->counter_504[3] = 0;
	} else {
		actor->counter_504[apOther->owner->which_player] = 0;
	}
}

static void GrrrolPlayCrashSoundAt(dStageActor_c *source, Vec *stagePos) {
	if (SoundPlayingClass::instance2) {
		Vec2 soundPos;
		ConvertStagePositionToScreenPosition(&soundPos, stagePos);
		SoundPlayingClass::instance2->PlaySoundAtPosition(SE_OBJ_TEKKYU_CRASH, &soundPos, 0);
		return;
	}

	if (source)
		PlaySound(source, SE_OBJ_TEKKYU_CRASH);
}

void daGrrrol_c::dieSpinOffscreen(dStageActor_c *killer, float xSpeed, bool awardScore) {
    if (this->acState.getCurrentState()->isEqual(&dEn_c::StateID_DieFall))
        return;

    this->stopCrashEffect();
    this->stopRollEffect();
    this->stopRollSound();

    if (xSpeed == 0.0f) {
        if (this->speed.x > 0.05f)
            xSpeed = 2.0f;
        else if (this->speed.x < -0.05f)
            xSpeed = -2.0f;
        else if (killer && killer != this && killer->pos.x != this->pos.x)
            xSpeed = this->pos.x >= killer->pos.x ? 2.0f : -2.0f;
        else
            xSpeed = this->direction ? -2.0f : 2.0f;
    }

    this->direction = xSpeed < 0.0f ? 1 : 0;
    Vec2 killSpeed = {xSpeed, 3.5f};
    PlaySound(this, SE_EMY_DOWN);
    this->deathRotX = this->rot.x;
    this->killWithSpecifiedState(killer ? killer : this, &killSpeed, &dEn_c::StateID_DieFall);
}

void daGrrrol_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	dStageActor_c *other = apOther->owner;
	u16 name = other->profileId;

	if (GrrrolShouldIgnoreSprite(name))
		return;

	if (this->isMega && GrrrolIsBreakableBlockSprite(name)) {
		u8 movementDirection = this->speed.x < 0.0f ? 1 : (this->speed.x > 0.0f ? 0 : this->direction);
		GrrrolBreakBlockSprite(this, other, movementDirection);
		return;
	}

	if (name == ProfileId::Grrrol) {
		daGrrrol_c *otherGrrrol = (daGrrrol_c*)other;

		if (this->isMega != otherGrrrol->isMega) {
			daGrrrol_c *killer = this->isMega ? this : otherGrrrol;
			daGrrrol_c *victim = this->isMega ? otherGrrrol : this;
			Vec2 killSpeed = {
				killer->direction ? -2.0f : 2.0f,
				2.5f
			};

			victim->dieSpinOffscreen(killer, killSpeed.x);
			return;
		}

		if (this->id > otherGrrrol->id)
			return;

		daGrrrol_c *leftBall = (this->pos.x <= otherGrrrol->pos.x) ? this : otherGrrrol;
		daGrrrol_c *rightBall = (this->pos.x <= otherGrrrol->pos.x) ? otherGrrrol : this;
		if (leftBall->speed.x <= rightBall->speed.x) {
			return;
		}

		if (this->pos.x <= otherGrrrol->pos.x) {
			this->pos.x -= 1.5f;
			otherGrrrol->pos.x += 1.5f;
		} else {
			this->pos.x += 1.5f;
			otherGrrrol->pos.x -= 1.5f;
		}

		bool thisOnGround = this->collMgr.isOnTopOfTile() || this->rollingOnSpawnerGuide;
		bool otherOnGround = otherGrrrol->collMgr.isOnTopOfTile() || otherGrrrol->rollingOnSpawnerGuide;

		if (abs(this->pos.y - otherGrrrol->pos.y) < 4.0f) {
			bool sharedGround = thisOnGround || otherOnGround;
			thisOnGround = sharedGround;
			otherOnGround = sharedGround;
		}

		if (thisOnGround != otherOnGround) {
			daGrrrol_c *grounded = thisOnGround ? this : otherGrrrol;
			daGrrrol_c *falling = thisOnGround ? otherGrrrol : this;

			u8 groundedDir = grounded->speed.x < 0.0f ? 1 : (grounded->speed.x > 0.0f ? 0 : grounded->direction);
			u8 fallingDir = falling->speed.x < 0.0f ? 1 : (falling->speed.x > 0.0f ? 0 : falling->direction);

			bool fallingIsInFront = (groundedDir == 0 && falling->pos.x > grounded->pos.x) ||
			                         (groundedDir == 1 && falling->pos.x < grounded->pos.x);

			if (fallingIsInFront) {
				if (groundedDir != fallingDir) {
					grounded->reverseDirection(true, false);
					falling->reverseDirection(false, false); 
				} else {
					grounded->reverseDirection(true, false);
				}
			} else {
				falling->reverseDirection(false, false); 
			}

			return; 
		}

		u8 thisMovementDirection = this->speed.x < 0.0f ? 1 : (this->speed.x > 0.0f ? 0 : this->direction);
		u8 otherMovementDirection = otherGrrrol->speed.x < 0.0f ? 1 : (otherGrrrol->speed.x > 0.0f ? 0 : otherGrrrol->direction);
		bool thisMoving = GrrrolAbsSpeed(this->speed.x) > 0.05f;
		bool otherMoving = GrrrolAbsSpeed(otherGrrrol->speed.x) > 0.05f;
		bool oppositeDirections = thisMoving && otherMoving && thisMovementDirection != otherMovementDirection;

		if (oppositeDirections) {
			this->reverseDirection(thisOnGround, false);
			otherGrrrol->reverseDirection(otherOnGround, false);
			return;
		}

		bool thisIsBehind = (thisMovementDirection == 0 && this->pos.x <= otherGrrrol->pos.x)
			|| (thisMovementDirection == 1 && this->pos.x >= otherGrrrol->pos.x);
		if (thisIsBehind) {
			this->reverseDirection(thisOnGround, false);
		} else {
			otherGrrrol->reverseDirection(otherOnGround, false);
		}
		return;
	}

	if (other->stageActorType == dStageActor_c::EntityType) {
		dEn_c *enemy = (dEn_c*)other;
		if (enemy->collisionCat9_RollingObject(apOther, apThis)) {
			apOther->someFlagByte |= 2;
			return;
		}

		Vec2 killSpeed = {
			this->direction ? -2.0f : 2.0f,
			2.5f
		};

		enemy->killWithSpecifiedState(this, &killSpeed, &dEn_c::StateID_DieOther);
		return;
	}

	dEn_c::spriteCollision(apThis, apOther);
}

void daGrrrol_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayerFromGrrrol(this, apThis, apOther);
}

void daGrrrol_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apOther->info.category2 == 0xF)
		return;

	DamagePlayerFromGrrrol(this, apThis, apOther);
}

bool daGrrrol_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return dEn_c::collisionCat3_StarPower(apThis, apOther);
}

bool daGrrrol_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayerFromGrrrol(this, apThis, apOther);
	return true;
}

bool daGrrrol_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayerFromGrrrol(this, apThis, apOther);
	return true;
}

bool daGrrrol_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayerFromGrrrol(this, apThis, apOther);
	return true;
}

bool daGrrrol_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat7_GroundPound(apThis, apOther);
}

bool daGrrrol_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayerFromGrrrol(this, apThis, apOther);
	return true;
}

bool daGrrrol_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayerFromGrrrol(this, apThis, apOther);
	return true;
}

bool daGrrrol_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	dStageActor_c *rollingObject = apOther ? apOther->owner : 0;
	if (rollingObject && rollingObject->stageActorType == dStageActor_c::EntityType) {
		dEn_c *enemy = (dEn_c*)rollingObject;
		Vec2 killSpeed = {
			this->direction ? -2.0f : 2.0f,
			2.5f
		};
		Vec effectPos = {
			rollingObject->pos.x,
			rollingObject->pos.y,
			rollingObject->pos.z + 500.0f
		};
		S16Vec effectRot = {0, 0, 0};
		Vec effectScale = {1.0f, 1.0f, 1.0f};
		PlaySound(enemy, SE_EMY_DOWN);
		SpawnEffect("Wm_mr_hardhit", 0, &effectPos, &effectRot, &effectScale);
		if (rollingObject->which_player < 4) {
			Vec scorePos = {
				rollingObject->pos.x,
				rollingObject->pos.y + 24.0f,
				rollingObject->pos.z
			};
			DisplayScoreAddPopup(scorePos, 0x1, rollingObject->which_player, false);
			AddScore(200, rollingObject->which_player);
		}

		enemy->killWithSpecifiedState(this, &killSpeed, &dEn_c::StateID_DieFall);
	}

	return true;
}

bool daGrrrol_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daGrrrol_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	dStageActor_c *iceball = apOther->owner;
	Vec effectPos = iceball ? iceball->pos : this->pos;
	effectPos.z += 500.0f;
	S16Vec effectRot = {0, 0, 0};
	Vec effectScale = {1.0f, 1.0f, 1.0f};

	SpawnEffect("Wm_ob_icemisshit", 0, &effectPos, &effectRot, &effectScale);
	SpawnEffect("Wm_ob_icemisshit_smk", 0, &effectPos, &effectRot, &effectScale);
	PlaySound(this, SE_OBJ_PNGN_ICEBALL_DISAPP);
	return true;
}

bool daGrrrol_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daGrrrol_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

void daGrrrol_c::_vf140(dStageActor_c *actor) {
	return dEn_c::_vf140(actor);
}

void daGrrrol_c::powBlockActivated(bool isNotMPGP) {
	return dEn_c::powBlockActivated(isNotMPGP);;
}

u32 daGrrrol_c::canBePowed() {
	return true;
}

bool daGrrrol_c::calculateTileCollisions() {
	u8 movementDirection = this->speed.x < 0.0f ? 1 : (this->speed.x > 0.0f ? 0 : direction);
	bool waitingForRelease = this->pipeDropSpawn && this->pipeDropWaitingForGround;
	bool isMoving = GrrrolAbsSpeed(this->speed.x) > 0.05f;

	this->rollingOnSpawnerGuide = false;
	bool brokeMegaBlockSprite = this->isMega && isMoving && this->breakMegaBlockSpritesAhead(movementDirection);
	if (waitingForRelease) {
		this->speed.x = 0.0f;
		this->max_speed.x = 0.0f;
		this->x_speed_inc = 0.0f;
	}

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	if (this->speed.y < -0.5f) {
		this->suppressMicroBounceLanding = false;
	}

	Vec posBeforeCollision = this->pos;
	bool wasOnGround = collMgr.isOnTopOfTile();
	this->rollingOnSpawnerGuide = this->handleSpawnerGuideCollision();

	cmgr_returnValue = wasOnGround;

	bool ignoredSpawnerBelow = false;
	if (this->rollingOnSpawnerGuide && !waitingForRelease) {
		collMgr.sCollBelow = 0;
		collMgr.outputMaybe = 0;
		collMgr.onGround_maybe = 0;
	} else {
		collMgr.calculateBelowCollisionWithSmokeEffect();
		if (GrrrolIsSpawnerPhysics(collMgr.sCollBelow)) {
			ignoredSpawnerBelow = true;
		}

		if (ignoredSpawnerBelow) {
			collMgr.sCollBelow = 0;
			collMgr.outputMaybe = 0;
			collMgr.onGround_maybe = 0;
			this->pos.y = posBeforeCollision.y;
		}
	}
	bool isOnGround = (!ignoredSpawnerBelow && collMgr.isOnTopOfTile()) || this->rollingOnSpawnerGuide;
	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta > 0.05f) {
		direction = 0;
	} else if (xDelta < -0.05f) {
		direction = 1;
	}

	if (isOnGround) {
		if (waitingForRelease) {
			if ((this->pipeDropSpawnStartY - this->pos.y) >= GrrrolPipeDropMinimumFallDistance) {
				u8 directionSetting = (this->settings >> 4) & 0x3;
				if (directionSetting == 2) {
					this->direction = GrrrolDirectionToNearestPlayer(this->pos, this->direction);
					this->rot.y = this->direction ? 0xD800 : 0x2800;
					if (this->srtLoaded) {
						float rate = this->direction ? -1.0f : 1.0f;
						this->srtAnimation.setUpdateRateForEntry(rate, 0);
					}
				}

				this->pipeDropSpawn = false;
				this->pipeDropWaitingForGround = false;
				this->setEyeRotationFrozen(false);
				this->suppressLandingBounce = true;
				this->suppressMicroBounceLanding = false;
				this->speed.x = this->direction ? -this->moveSpeed : this->moveSpeed;
				this->max_speed.x = this->speed.x;
				this->x_speed_inc = GrrrolAcceleration;
			} else {
				this->speed.y = GrrrolPipeDropWaitingSpeed;
				this->speed.x = 0.0f;
				this->max_speed.x = 0.0f;
				this->suppressLandingBounce = true;
			}
		} else if (this->pipeDropSpawn) {
			this->pipeDropSpawn = false;
			this->setEyeRotationFrozen(false);
			this->suppressLandingBounce = true;
			this->suppressMicroBounceLanding = false;
			this->speed.x = this->direction ? -this->moveSpeed : this->moveSpeed;
			this->max_speed.x = this->speed.x;
		}
		if (!wasOnGround && !this->rollingOnSpawnerGuide && !waitingForRelease) {
			if (this->suppressMicroBounceLanding) {
				this->suppressMicroBounceLanding = false;
			} else if (this->suppressLandingBounce) {
				this->suppressLandingBounce = false;
			} else {
				this->speed.y = GrrrolBounceSpeed;
				this->suppressLandingBounce = true;
				this->spawnLandingEffects();
				Vec crashSoundPos = this->pos;
				GrrrolPlayCrashSoundAt(this, &crashSoundPos);
			}
		}

		if (this->speed.y <= 0.0f)
			speed.y = 0.0f;
		if (!waitingForRelease)
			max_speed.x = direction ? -moveSpeed : moveSpeed;
	} else {
		rot.y = this->direction ? 0xD800 : 0x2800;

		if (_34A & 4) {
			Vec v = (Vec){0.0f, 1.0f, 0.0f};
			collMgr.pSpeed = &v;

			if (collMgr.calculateAboveCollision(collMgr.outputMaybe)) {
				if (GrrrolIsSpawnerPhysics(collMgr.sCollAbove)) {
					this->pos.y = posBeforeCollision.y;
					collMgr.sCollAbove = 0;
					collMgr.outputMaybe = 0;
				} else {
					speed.y = 0.0f;
				}
			}

			collMgr.pSpeed = &speed;
		} else {
			if (collMgr.calculateAboveCollision(collMgr.outputMaybe)) {
				if (GrrrolIsSpawnerPhysics(collMgr.sCollAbove)) {
					this->pos.y = posBeforeCollision.y;
					collMgr.sCollAbove = 0;
					collMgr.outputMaybe = 0;
				} else {
					speed.y = 0.0f;
				}
			}
		}
	}

	collMgr.calculateAdjacentCollision(0);
	if (GrrrolIsSpawnerPhysics(collMgr.sCollAdjacent[0])) {
		collMgr.sCollAdjacent[0] = 0;
		collMgr.sCollAdjacentLast = 0;
		collMgr.outputMaybe &= ~0x15;
	}
	if (GrrrolIsSpawnerPhysics(collMgr.sCollAdjacent[1])) {
		collMgr.sCollAdjacent[1] = 0;
		collMgr.sCollAdjacentLast = 0;
		collMgr.outputMaybe &= ~(0x15 << 1);
	}

	if (this->rollingOnSpawnerGuide && isMoving) {
		float wallX = this->pos.x + (movementDirection ? -16.0f : 16.0f);
		const float wallYOffsets[] = {4.0f, 12.0f, 20.0f};
		for (int i = 0; i < 3; i++) {
			UnitType tileType = (UnitType)collMgr.getTileBehaviour1At(
				wallX, this->pos.y + wallYOffsets[i], this->currentLayerID);
			if (tileType & (SOLID | SOLID_ON_TOP | PARTIAL_SOLID | PIPE | BRICK | QUESTION | EXPLODABLE)) {
				collMgr.outputMaybe |= (0x15 << movementDirection);
				break;
			}
		}
	}

	if ((collMgr.outputMaybe & (0x15 << movementDirection))) {
		bool brokeBlock = brokeMegaBlockSprite;
		bool isNormalPipeDropRestricted = !this->isMega && this->pipeDropSpawn && ((this->pipeDropSpawnStartY - this->pos.y) < GrrrolPipeDropMinimumFallDistance);
		if (isMoving && !isNormalPipeDropRestricted && !waitingForRelease)
            brokeBlock = this->breakBlocksAhead(movementDirection) || brokeBlock;
		if (this->isMega && brokeBlock)
			return false;

		if (brokeBlock && !this->rollingOnSpawnerGuide) {
			collMgr.calculateBelowCollisionWithSmokeEffect();
			isOnGround = collMgr.isOnTopOfTile();
			if (!isOnGround) {
				this->dieSpinOffscreen(this, this->speed.x, false);
				return false;
			}
		}

		if (isOnGround)
			isBouncing = true;
		return true;
	}
	
	return false;
}

bool daGrrrol_c::handleSpawnerGuideCollision() {

	bool standing = false;
	float radius = 9.0f;
	fBase_c *iterator = 0;

	while ((iterator = fBase_c::searchByProfileId(ProfileId::GrrrolSpawner, iterator))) {
		dStageActor_c *spawner = (dStageActor_c*)iterator;
		if (!spawner || spawner->isDeleted || spawner->currentLayerID != this->currentLayerID)
			continue;

		u8 spawnDirection = (spawner->settings >> 20) & 0x3;
		if (spawnDirection == 2)
			continue;

		float pipePixels = GrrrolSpawnerPipePixelsFromSettings(spawner->settings);
		float floorY = spawner->pos.y - 16.0f;
		float guideLength = pipePixels + 48.0f;
		float floorMinX, floorMaxX;
		if (spawnDirection == 0) {
			floorMinX = spawner->pos.x;
			floorMaxX = spawner->pos.x + guideLength;
		} else {
			floorMinX = spawner->pos.x - guideLength;
			floorMaxX = spawner->pos.x;
		}

		if (GrrrolInRange(this->pos.x, floorMinX - radius, floorMaxX + radius)
				&& this->last_pos.y >= floorY - 12.0f
				&& GrrrolInRange(this->pos.y, floorY - 12.0f, floorY + 8.0f)
				&& this->speed.y <= 0.0f) {
			this->pos.y = floorY;
			this->speed.y = 0.0f;
			standing = true;
		}
	}
	return standing;
}

void daGrrrol_c::reverseDirection(bool doHop, bool slowDown) {
	this->stopRollSound();
	
	this->direction ^= 1;
	float targetSpeed = this->direction ? -this->moveSpeed : this->moveSpeed;
	this->speed.x = targetSpeed;
	this->max_speed.x = targetSpeed;
	this->x_speed_inc = 0.0f;
	this->rot.y = this->direction ? 0xD800 : 0x2800;
	Vec crashSoundPos = this->pos;
	GrrrolPlayCrashSoundAt(this, &crashSoundPos);
	if (slowDown)
		this->spawnCrashEffects();

	if (slowDown && !this->noSlowWallBounce && this->moveSpeed > 0.0f) {
		this->speed.x = targetSpeed * GrrrolWallSlowdownMultiplier;
		this->x_speed_inc = this->moveSpeed / GrrrolWallSlowdownFrames;
	}

	if (this->srtLoaded) {
		float rate = this->direction ? -1.0f : 1.0f;
		this->srtAnimation.setUpdateRateForEntry(rate, 0);
	}

	if (doHop) {
		this->speed.y = GrrrolBounceSpeed;
		this->isBouncing = false;
		this->suppressLandingBounce = true;
		this->suppressMicroBounceLanding = false;
	}
}

bool daGrrrol_c::breakBlocksAhead(u8 movementDirection) {
	if (!dBgGm_c::instance)
		return false;

	u8 layersToCheck[4] = {this->currentLayerID, 0, 1, 2};
	bool brokeBrick = false;
	float sideDistance = this->isMega ? GrrrolMegaSideSensorDistance : GrrrolNormalSideSensorDistance;
	float heightToCheck = 24.0f;
	float checkX = this->pos.x + (movementDirection ? -(sideDistance + 4.0f) : sideDistance + 4.0f);
	u16 worldX = ((int)checkX) & 0xFFF0;

	for (float yOffset = 0.0f; yOffset <= heightToCheck; yOffset += 8.0f) {
		float checkY = this->pos.y + yOffset;
		u16 worldY = ((int)-checkY) & 0xFFF0;
		float tileCenterX = ((float)(s16)worldX) + 8.0f;
		float tileCenterY = ((float)-(s16)worldY) - 8.0f;

		for (int layerIndex = 0; layerIndex < 4; layerIndex++) {
			u8 layer = layersToCheck[layerIndex];
			if (!GrrrolLayerExists(layer))
				continue;
			if (layerIndex > 0) {
				bool duplicateLayer = false;
				for (int previousLayerIndex = 0; previousLayerIndex < layerIndex; previousLayerIndex++) {
					if (layersToCheck[previousLayerIndex] == layer) {
						duplicateLayer = true;
						break;
					}
				}
				if (duplicateLayer)
					continue;
			}

			u16 *tile = dBgGm_c::instance->getPointerToTile(worldX, worldY, layer);
			if (!tile)
				continue;

			u16 tileValue = *tile;
			if (!GrrrolIsBreakableBlockTile(tileValue))
				continue;

			dBgGm_c::instance->placeTile(worldX, worldY, layer, 0);
			brokeBrick = true;

			Vec breakPos = {
				tileCenterX,
				tileCenterY,
				this->pos.z + 500.0f
			};

			if (dEffectBreakMgr_c::instance) {
				u16 shardType = GrrrolBlockTileShardType(tileValue);
				u32 breakSettings = ((u32)shardType << 8) | GrrrolBreakVelocityForDirection(movementDirection);
				Vec effectPos = breakPos;
				effectPos.y -= 8.0f;
				dEffectBreakMgr_c::instance->spawnTile(&effectPos, breakSettings, 0);
			}

			GrrrolPlayBlockBreakSoundAt(this, &breakPos);
		}
	}

	return brokeBrick;
}

bool daGrrrol_c::breakMegaTopBlock(u8 movementDirection) {
	if (!dBgGm_c::instance)
		return false;

	bool brokeBrick = false;
	u8 layersToCheck[4] = {this->currentLayerID, 0, 1, 2};


	float topSensorHeight = 72.0f; 
	float topSensorDistance = GrrrolMegaSideSensorDistance - 16.0f; 
	
	float checkX = this->pos.x + (movementDirection ? -(topSensorDistance + 4.0f) : topSensorDistance + 4.0f);
	float checkY = this->pos.y + topSensorHeight;
	
	u16 worldX = ((int)checkX) & 0xFFF0;
	u16 worldY = ((int)-checkY) & 0xFFF0;
	float tileCenterX = ((float)(s16)worldX) + 8.0f;
	float tileCenterY = ((float)-(s16)worldY) - 8.0f;

	for (int layerIndex = 0; layerIndex < 4; layerIndex++) {
		u8 layer = layersToCheck[layerIndex];
		if (!GrrrolLayerExists(layer)) continue;
		
		if (layerIndex > 0) {
			bool duplicateLayer = false;
			for (int previousLayerIndex = 0; previousLayerIndex < layerIndex; previousLayerIndex++) {
				if (layersToCheck[previousLayerIndex] == layer) {
					duplicateLayer = true;
					break;
				}
			}
			if (duplicateLayer) continue;
		}

		u16 *tile = dBgGm_c::instance->getPointerToTile(worldX, worldY, layer);
		if (!tile) continue;

		u16 tileValue = *tile;
		if (!GrrrolIsBreakableBlockTile(tileValue)) continue;

		dBgGm_c::instance->placeTile(worldX, worldY, layer, 0);
		brokeBrick = true;

		Vec breakPos = { tileCenterX, tileCenterY, this->pos.z + 500.0f };

		if (dEffectBreakMgr_c::instance) {
			u16 shardType = GrrrolBlockTileShardType(tileValue);
			u32 breakSettings = ((u32)shardType << 8) | GrrrolBreakVelocityForDirection(movementDirection);
			Vec effectPos = breakPos;
			effectPos.y -= 8.0f;
			dEffectBreakMgr_c::instance->spawnTile(&effectPos, breakSettings, 0);
		}

		GrrrolPlayBlockBreakSoundAt(this, &breakPos);
	}

	return brokeBrick;
}

bool daGrrrol_c::breakMegaBlockSpritesAhead(u8 movementDirection) {
	static const u16 breakableProfiles[] = {
		ProfileId::EN_OBJ_HATENA_BLOCK,
		ProfileId::EN_OBJ_RENGA_BLOCK
	};

	bool brokeBlock = false;

	for (int profileIndex = 0; profileIndex < 2; profileIndex++) {
		fBase_c *iterator = 0;

		while ((iterator = fBase_c::searchByProfileId(breakableProfiles[profileIndex], iterator))) {
			dStageActor_c *block = (dStageActor_c*)iterator;
			float xDistance = block->pos.x - this->pos.x;
			float yDistance = block->pos.y - this->pos.y;

			if (movementDirection) {
				if (xDistance < -(GrrrolMegaSideSensorDistance + GrrrolMegaBlockSpriteAheadSensor)
						|| xDistance > -(GrrrolMegaSideSensorDistance - GrrrolMegaBlockSpriteBehindSensor))
					continue;
			} else {
				if (xDistance < GrrrolMegaSideSensorDistance - GrrrolMegaBlockSpriteBehindSensor
						|| xDistance > GrrrolMegaSideSensorDistance + GrrrolMegaBlockSpriteAheadSensor)
					continue;
			}

			if (yDistance < GrrrolMegaSideSensorBelow - 16.0f
					|| yDistance > GrrrolMegaSideSensorAbove + 16.0f)
				continue;

			if (GrrrolBreakBlockSprite(this, block, movementDirection))
				brokeBlock = true;
		}
	}

	return brokeBlock;
}

int daGrrrol_c::onCreate() {
	bool spawnedFromPipe = ((this->settings >> 30) & 0x1) != 0;
	u8 pipeSpawnDirection = (this->settings >> 6) & 0x3;
	this->isMega = ((this->settings >> 28) & 0x1) != 0;
	this->noSlowWallBounce = ((this->settings >> 18) & 0x1) != 0;
	bool onlySpawnFacingPlayer = ((this->settings >> 17) & 0x1) != 0;
	u8 directionSetting = (this->settings >> 4) & 0x3;
	u8 startingDirection = directionSetting == 1 ? 0 : 1;
	if (directionSetting == 2)
		startingDirection = GrrrolDirectionToNearestPlayer(this->pos, startingDirection);
	int speedSetting = this->settings & 0x3;
	float bodyRadius = this->isMega ? 15.0f : 9.0f;
	float hurtRadiusX = this->isMega ? 26.0f : 14.0f;
	float hurtRadiusY = this->isMega ? 25.0f : 13.0f;
	float hurtCenterY = hurtRadiusY + 2.0f;
	this->modelLoaded = false;
	this->srtLoaded = false;
	this->suppressLandingBounce = false;
	this->suppressMicroBounceLanding = false;
	this->spawnedFromSpawner = spawnedFromPipe;
	this->deleteForever = spawnedFromPipe;
	this->pipeDropSpawn = spawnedFromPipe && pipeSpawnDirection == 2;
	this->pipeDropWaitingForGround = this->pipeDropSpawn;
	this->pipeDropSpawnStartY = this->pos.y;
	this->freezeEyeRotation = this->pipeDropSpawn;
	this->rollingOnSpawnerGuide = false;
	this->activatedByView = spawnedFromPipe;
	this->crashEffectTimer = 0;
	this->rollEffectTimer = 0;

	if (this->eventId1 != 0 && dFlagMgr_c::instance->active(this->eventId1 - 1)) {
		this->Delete(1);
		return true;
	}

	if (onlySpawnFacingPlayer && !GrrrolStartsFacingAnyPlayer(this->pos, startingDirection)) {
		this->Delete(1);
		return true;
	}

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("grrrol", "g3d/gorogoro.brres");
	if (!this->resFile.data) {
		OSReport("Grrrol: could not load grrrol/g3d/gorogoro.brres\n");
	} else {
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl(this->isMega ? "gorogoroL" : "gorogoro");
		if (!mdl.data) {
			mdl = this->resFile.GetResMdl(this->isMega ? "gorogoro" : "gorogoroL");
		}

		if (!mdl.data) {
			OSReport("Grrrol: resource is missing models 'gorogoro' and 'gorogoroL'\n");
		} else {
			model.setup(mdl, &allocator, 0x224, 1, 0);
			SetupTextures_Enemy(&model, 0);
			this->modelLoaded = true;

			nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("gorogoro");
			if (anmSrt.data) {
				this->srtAnimation.setup(mdl, anmSrt, &allocator, 0, 1);
				this->srtAnimation.bindEntry(&this->model, anmSrt, 0, 1);
				this->srtAnimation.setFrameForEntry(0.0f, 0);
				this->srtAnimation.setUpdateRateForEntry(1.0f, 0);
				this->model.bindAnim(&this->srtAnimation, 1.0f);
				this->srtLoaded = true;
			}
		}
	}

	allocator.unlink();

	this->scale = (Vec){1.0f, 1.0f, 1.0f};

	this->rot.x = 0;
	this->rot.z = 0;
	this->direction = startingDirection;
	this->rot.y = this->direction ? 0xD800 : 0x2800;
	if (this->srtLoaded) {
		float rate = this->direction ? -1.0f : 1.0f;
		float startFrame = this->direction ? 60.0f : 0.0f;
		this->srtAnimation.setFrameForEntry(startFrame, 0);
		this->srtAnimation.setUpdateRateForEntry(rate, 0);
	}

	static const float speeds[] = {1.0f, 1.5f, 2.0f};
	this->moveSpeed = speeds[speedSetting];
	this->rollTimer = 0;
	this->speed.x = 0.0f;
	this->speed.y = 0.0f;
	this->max_speed.x = this->moveSpeed;
	this->x_speed_inc = GrrrolAcceleration;

	ActivePhysics::Info hitInfo;

	hitInfo.xDistToCenter = 0.0f;
	hitInfo.yDistToCenter = hurtCenterY;
	hitInfo.xDistToEdge = hurtRadiusX;
	hitInfo.yDistToEdge = hurtRadiusY;
	hitInfo.category1 = 0x3;
	hitInfo.category2 = 0x0;
	hitInfo.bitfield1 = 0x4F;
	hitInfo.bitfield2 = 0xffbafffe;
	hitInfo.unkShort1C = 0;
	hitInfo.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &hitInfo);
	this->aPhysics.addToList();

	this->pos_delta2.x = 0.0f;
	this->pos_delta2.y = 0.0f;
	this->pos_delta2.z = 0.0f;
	this->spriteSomeRectX = this->isMega ? 52.0f : 28.0f;
	this->spriteSomeRectY = this->isMega ? 58.0f : 38.0f;
	this->_320 = 0.0f;
	this->_324 = this->isMega ? 34.0f : 18.0f;
	this->_36D = 0;

	static const lineSensor_s belowNormal(13 << 12, -(9 << 12), (0 << 12));
	static const pointSensor_s aboveNormal(0 << 12, 13 << 12);
	static const lineSensor_s adjacentNormal(SENSOR_BREAK_BRICK | SENSOR_80000000, GrrrolNormalSideSensorBelow << 12, GrrrolNormalSideSensorAbove << 12, GrrrolNormalSideSensorDistance << 12);

	static const lineSensor_s belowMega(22 << 12, -(24 << 12), (0 << 12));
	static const pointSensor_s aboveMega(0 << 12, 24 << 12);
	static const lineSensor_s adjacentMega(
		GrrrolMegaWallSensorFlags,
		GrrrolMegaSideSensorBelow << 12,
		GrrrolMegaSideSensorAbove << 12,
		GrrrolMegaSideSensorDistance << 12);

collMgr.init(this,
		this->isMega ? &belowMega : &belowNormal,
		this->isMega ? &aboveMega : &aboveNormal,
		this->isMega ? &adjacentMega : &adjacentNormal);
		
	if (!this->spawnedFromSpawner) {
		this->pos.y -= 32.0f;
		if (this->isMega) {
			this->pos.y -= 40.0f;
			this->pos.x += 32.0f; 
		} else {
			this->pos.x += 12.0f;
		}
	}

	this->last_pos = this->pos;

	if (this->spawnedFromSpawner && !this->pipeDropSpawn) {
		this->rollingOnSpawnerGuide = true;
		cmgr_returnValue = true;
		isBouncing = false;
	} else {
		collMgr.calculateBelowCollisionWithSmokeEffect();
		cmgr_returnValue = collMgr.isOnTopOfTile();
		isBouncing = !collMgr.isOnTopOfTile();
	}
	doStateChange(&StateID_Roll);
	this->onExecute();
	return true;
}

int daGrrrol_c::onDelete() {
	this->stopCrashEffect();
	this->stopRollEffect();
	this->stopRollSound();
	return true;
}

int daGrrrol_c::onExecute() {
	if (GrrrolShouldDespawnFromCamera(this->pos)) {
		this->Delete(this->deleteForever);
		return true;
	}

	if (!this->spawnedFromSpawner && !this->activatedByView) {
		if (this->isOutOfView())
			return true;

		this->activatedByView = true;
	}

	acState.execute();
	if (this->acState.getCurrentState()->isEqual(&dEn_c::StateID_DieFall)) {
		this->rot.z += -0x0800;
	}
	if (this->speed.x == 0.0f) {
		this->freezeEyeRotation = true;
	
		if (this->srtLoaded) {
			this->srtAnimation.setUpdateRate(0.0f);
		}
	} else {
		this->freezeEyeRotation = false;
    
		if (this->srtLoaded) {
			this->srtAnimation.setUpdateRate(1.0f);
		}
	}

	if (this->modelLoaded) {
		if (this->srtLoaded) {
			if (this->freezeEyeRotation) {
				this->srtAnimation.setUpdateRateForEntry(0.0f, 0);
			} else {
				this->srtAnimation.process();
				float frame = this->srtAnimation.getFrameForEntry(0);
				float rate = this->srtAnimation.getUpdateRateForEntry(0);

				if (rate > 0.0f && frame >= 60.0f)
					this->srtAnimation.setFrameForEntry(frame - 60.0f, 0);
				else if (rate < 0.0f && frame <= 0.0f)
					this->srtAnimation.setFrameForEntry(frame + 60.0f, 0);
			}
		}

		this->updateCrashEffect();
		this->updateRollSound();
		this->updateRollEffect();
		updateModelMatrices();
		model._vf1C();
	}
	return true;
}

int daGrrrol_c::onDraw() {
	if (this->modelLoaded)
		model.scheduleForDrawing();
	return true;
}

void daGrrrol_c::spawnCrashEffects() {
	this->stopCrashEffect();

	float sideOffset = this->isMega ? 21.0f : 14.0f;
	Vec effectPos = {
		this->pos.x + (this->direction ? sideOffset : -sideOffset),
		this->pos.y + (this->isMega ? 20.0f : 16.0f),
		this->pos.z + 500.0f
	};
	S16Vec effectRot = {0, 0, 0};
	float scaleValue = this->isMega ? 2.0f : 1.0f;
	Vec effectScale = {scaleValue, scaleValue, scaleValue};
	this->crashEffect.spawn("Wm_en_iron", 0, &effectPos, &effectRot, &effectScale);
	this->crashEffectTimer = GrrrolRollEffectFrames;
}

void daGrrrol_c::spawnLandingEffects() {
	Vec effectPos = {
		this->pos.x,
		this->pos.y,
		this->pos.z + 500.0f
	};
	S16Vec effectRot = {0, 0, 0};
	float scaleValue = this->isMega ? 2.0f : 1.0f;
	Vec effectScale = {scaleValue, scaleValue, scaleValue};
	const char *effectName = GrrrolUsesSandLandSmoke(this->collMgr.tileBelowSubType)
		? "Wm_en_sndlandsmk_s"
		: "Wm_en_landsmoke_s";
	SpawnEffect(effectName, 0, &effectPos, &effectRot, &effectScale);
}

void daGrrrol_c::setEyeRotationFrozen(bool frozen) {
	this->freezeEyeRotation = frozen;
	if (!this->srtLoaded)
		return;

	if (frozen) {
		this->srtAnimation.setUpdateRateForEntry(0.0f, 0);
		return;
	}

	float rate = this->direction ? -1.0f : 1.0f;
	this->srtAnimation.setUpdateRateForEntry(rate, 0);
}

void daGrrrol_c::updateRollSound() {
	bool shouldRoll = !this->isOutOfView()
		&& (this->collMgr.isOnTopOfTile() || this->rollingOnSpawnerGuide || this->suppressMicroBounceLanding)
		&& !this->suppressLandingBounce
		&& GrrrolAbsSpeed(this->speed.x) > 0.05f;

	if (!shouldRoll) {
		this->stopRollSound();
		return;
	}

	int soundId = this->isMega ? SE_OBJ_TEKKYU_L_ROLL : SE_OBJ_TEKKYU_ROLL;
	if (!this->rollSoundHandle.Exists())
		PlaySoundWithFunctionB4(SoundRelatedClass, &this->rollSoundHandle, soundId, 1);
	this->rollSoundHandle.SetVolume(this->isMega ? 1.25f : 1.15f, 1);
}

void daGrrrol_c::stopRollSound() {
	if (this->rollSoundHandle.Exists()) {
		this->rollSoundHandle.Stop(0);
		this->rollSoundHandle.DetachSound();
	}
}

void daGrrrol_c::updateCrashEffect() {
	if (this->crashEffectTimer == 0)
		return;

	--this->crashEffectTimer;
	if (this->crashEffectTimer == 0)
		this->stopCrashEffect();
}

void daGrrrol_c::stopCrashEffect() {
	this->crashEffect._vf10();
	this->crashEffect._vf18();
	this->crashEffect.clear();
	this->crashEffectTimer = 0;
}

void daGrrrol_c::updateRollEffect() {
	bool shouldRoll = (this->collMgr.isOnTopOfTile() || this->rollingOnSpawnerGuide || this->suppressMicroBounceLanding)
		&& !this->suppressLandingBounce
		&& GrrrolAbsSpeed(this->speed.x) > 0.05f;

	if (!shouldRoll) {
		this->stopRollEffect();
		return;
	}

	if (this->rollEffectTimer > 0) {
		--this->rollEffectTimer;
		if (this->rollEffectTimer == 0)
			this->stopRollEffect();
		return;
	}

	Vec effectPos = {
		this->pos.x,
		this->pos.y + 2.0f,
		this->pos.z + 500.0f
	};
	S16Vec effectRot = {0, 0, 0};
	Vec effectScale = {1.0f, 1.0f, 1.0f};
	this->rollEffect.spawn("Wm_en_iron", 0, &effectPos, &effectRot, &effectScale);
	this->rollEffectTimer = GrrrolRollEffectFrames;
}

void daGrrrol_c::stopRollEffect() {
	this->rollEffect._vf10();
	this->rollEffect._vf18();
	this->rollEffect.clear();
	this->rollEffectTimer = 0;
}

void daGrrrol_c::updateModelMatrices() {
	static const s16 modelYaw = 0x7FE0;
	float hitRadius = this->isMega ? 15.0f : 9.0f;
	float modelYOffset = this->isMega ? 14.0f : 7.0f;
	s16 modelRotX = this->rot.z;
	s16 modelRotY = modelYaw;
	s16 modelRotZ = -this->rot.x;

	if (this->acState.getCurrentState()->isEqual(&dEn_c::StateID_DieFall)) {
	modelRotX = this->rot.z;
	modelRotZ = -this->deathRotX;
	modelRotY += this->direction ? 0x1000 : -0x1000;
}

	matrix.translation(pos.x, pos.y + hitRadius + modelYOffset, pos.z);
	matrix.applyRotationYXZ(&modelRotX, &modelRotY, &modelRotZ);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);
}

void daGrrrol_c::beginState_Roll() {
	this->max_speed.y = -4.0f;
	if (this->pipeDropSpawn) {
		this->pipeDropWaitingForGround = true;
		this->speed.x = 0.0f;
		this->max_speed.x = 0.0f;
		this->speed.y = 0.0f;
	} else {
		this->speed.x = this->direction ? -this->moveSpeed : this->moveSpeed;
		this->max_speed.x = this->speed.x;
		this->speed.y = 0.0f;
	}
	this->y_speed_inc = -0.1875f;
	this->x_speed_inc = GrrrolAcceleration;
}

void daGrrrol_c::executeState_Roll() {
	this->rollTimer++;
	if (this->pipeDropSpawn) {
		this->speed.x = 0.0f;
		this->max_speed.x = 0.0f;
		if ((this->pipeDropSpawnStartY - this->pos.y) < GrrrolPipeDropMinimumFallDistance) {
			this->speed.y = GrrrolPipeDropWaitingSpeed;
			this->y_speed_inc = 0.0f;
		} else {
			this->y_speed_inc = GrrrolPipeDropNormalGravity;
		}
	} else {
		this->y_speed_inc = GrrrolPipeDropNormalGravity;
	}
	u8 movementDirection = this->speed.x < 0.0f ? 1 : (this->speed.x > 0.0f ? 0 : this->direction);
	if (this->isMega && GrrrolAbsSpeed(this->speed.x) > 0.05f) {
        if (!this->pipeDropSpawn)
            this->breakMegaTopBlock(movementDirection);
    }
if (this->calculateTileCollisions() && !this->pipeDropSpawn) {
		bool hitRightWall = (this->collMgr.outputMaybe & 0x15) != 0;
		bool hitLeftWall  = (this->collMgr.outputMaybe & 0x2A) != 0;
		if ((hitRightWall && this->speed.x >= 0.0f) || (hitLeftWall && this->speed.x <= 0.0f)) {
			this->reverseDirection(this->collMgr.isOnTopOfTile() || this->suppressMicroBounceLanding, true);
		}
	}

	if ((this->collMgr.isOnTopOfTile() || this->rollingOnSpawnerGuide)
			&& GrrrolAbsSpeed(this->speed.x) > 0.05f
			&& !this->isBouncing
			&& this->speed.y == 0.0f
			&& (this->rollTimer % GrrrolRollingBounceInterval) == 0) {
		this->speed.y = GrrrolRollingBounceSpeed;
		this->suppressMicroBounceLanding = true;
	}

	float rollSpeed = GrrrolAbsSpeed(this->speed.x);

	s16 rollAmount = (s16)(2048.0f * rollSpeed * 0.25f);
	if (this->speed.x < 0.0f)
		this->rot.x += rollAmount;
	else
		this->rot.x -= rollAmount;
	if (GrrrolAbsSpeed(this->speed.x) >= this->moveSpeed) {
		this->x_speed_inc = GrrrolAcceleration;
	}
	this->rot.z = 0;
}

void daGrrrol_c::endState_Roll() { }
