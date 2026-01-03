#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "sandPillar.h"


const char* WaterGeyserArcNameList [] = {
	"water_geyser",
	NULL
};

const u8 WaterGeyserRiseDelayList[] = {
	125,	// 0
	123,	// 1
	121,	// 2
	124,	// 3
	118,	// 4
	112,	// 5
	109,	// 6
	113,	// 7
	110,	// 8
	107,	// 9
	104,	// 10
	104,	// 11
	101,	// 12
	97,		// 13
	94,		// 14
	90		// 15+
};

u8 lastPillarEffectType[] = {0, 0, 0, 0};

char* pillarEffect[] = {
	"Wm_mr_sprisesmoke",
	"Wm_mr_foot_water", // Wm_en_cmnwatertail, Wm_en_cmnwater, Wm_mr_foot_water
};

class daWaterGeyser_c : public daSandPillar_c {
	enum Mode {
		TimeControlled,
		EventControlled,
	};

	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	void bindAnimSrt_and_setUpdateRate(const char* name, m3d::mdl_c* mdl, m3d::anmTexSrt_c* animTexSrt, float rate);
	void executeSrtAnimations();
	bool areAllSrtAnimationsDone();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c model, modelTop, modelBehind;
	m3d::anmTexSrt_c anmTexSrt, anmTexSrtTop, anmTexSrtBehind;

	mMtx matrixTop, matrixBehind;

	mEf::es2 effect;
	int height, topHeight;
	Vec topScale;

	float topY, bottomY, totalHeight;
	u8 initialRiseDelay, riseDelay;
	u16 riseTime;
	float risePerFrame;

	int timer;
	daPlBase_c* players[4];
	float effectSize;
	Vec soundPos;

	Mode mode;

	u8 lastEvState;
	u8 newEvState;
	u8 offState;
	u64 triggeringEventID;
	bool invert, multiUse;

	nw4r::snd::SoundHandle handle; // Sound Handle

	bool ranOnce;

	StandOnTopCollider standOnTopCollider;
	ActivePhysics topAP;

	void updateModelMatrices();

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) { return false; }
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { return false; }

	void doFunsuiStuffWithPlayers(bool force);
	void setSoundVolume();
	// void spawnWaterDrops();

	USING_STATES(daWaterGeyser_c);
	DECLARE_STATE(UpSignal);
	DECLARE_STATE(GoUp);
	DECLARE_STATE(Stabilize);
	DECLARE_STATE(WaitUp);
	DECLARE_STATE(GoDown);
	DECLARE_STATE(WaitDown);
	DECLARE_STATE(InitialWait);

	bool checkForEventTrigger(u8 lookForState);

	public: static dActor_c *build();
};

dActor_c *daWaterGeyser_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daWaterGeyser_c));
	return new(buffer) daWaterGeyser_c;
}

const SpriteData WaterGeyserSpriteData = { ProfileId::WaterGeyser, 8, -14, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0x10 };
Profile WaterGeyserProfile(&daWaterGeyser_c::build, SpriteId::WaterGeyser, &WaterGeyserSpriteData, ProfileId::WaterGeyser, ProfileId::WaterGeyser, "WaterGeyser", WaterGeyserArcNameList, 0);

///////////////////////
// Externs and States
///////////////////////
CREATE_STATE(daWaterGeyser_c, UpSignal);
CREATE_STATE(daWaterGeyser_c, GoUp);
CREATE_STATE(daWaterGeyser_c, Stabilize);
CREATE_STATE(daWaterGeyser_c, WaitUp);
CREATE_STATE(daWaterGeyser_c, GoDown);
CREATE_STATE(daWaterGeyser_c, WaitDown);
CREATE_STATE(daWaterGeyser_c, InitialWait);

extern "C" int FUN_808027f0(daSandPillar_c * actor, daPlBase_c * player); // Returns 1 if player is in Funsui state, 0 otherwise (?)
// extern "C" void FUN_808028e0(daSandPillar_c * actor, daPlBase_c * player); // Tf is this
extern "C" uint FUN_80802d30(daSandPillar_c * actor, daPlBase_c * player); // Release Funsui for one player
// extern "C" void FUN_80802b40(daSandPillar_c *param_1); // Release Funsui (+ weird sand pillar stuff)

void daWaterGeyser_c::bindAnimSrt_and_setUpdateRate(const char* name, m3d::mdl_c* mdl, m3d::anmTexSrt_c* animTexSrt, float rate) {
	nw4r::g3d::ResAnmTexSrt animTexSrtRes = this->resFile.GetResAnmTexSrt(name);
	animTexSrt->bindEntry(mdl, animTexSrtRes, 0, 1);
	animTexSrt->bindEntry(mdl, animTexSrtRes, 1, 1);
	animTexSrt->bindEntry(mdl, animTexSrtRes, 2, 1);
	animTexSrt->setUpdateRate(rate);
	animTexSrt->setCurrentFrame(0.0);
	mdl->bindAnim(animTexSrt);
}

////////////////////////
// Collision Functions
////////////////////////
void daWaterGeyser_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	daWaterGeyser_c *self = (daWaterGeyser_c*)apThis->owner;
	dEn_c *other = (dEn_c*)apOther->owner;

	if (other->stageActorType == other->EntityType) {
		if (self->aPhysics.left() <= other->aPhysics.left() && other->aPhysics.right() <= self->aPhysics.right()) {
			if (other->y_speed_inc >= 0.0) return;

			SpawnEffect("Wm_en_cmnwater", 0, &other->pos, &(S16Vec){0, 0, 0}, &(Vec){0.5, 0.5, 0.5});
			other->speed.y = 2.0;
		}
	}
}

void daWaterGeyser_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	daWaterGeyser_c *self = (daWaterGeyser_c*)apThis->owner;
	dAcPy_c *player = (dAcPy_c*)apOther->owner;

	int iVar2;
	if (iVar2 = FUN_808027f0(self, player), iVar2 != 0) {
		player->setFunsui();
		// FUN_808028e0(self, player);
		for (int i = 0; i < 4; i++) {
			daPlBase_c *p = GetPlayerOrYoshi(i);
			if (p != NULL && p == player && self->players[i] == NULL) {
				self->players[i] = p;
				lastPillarEffectType[i] = 1;
				break;
			}
		}
	}
}

void daWaterGeyser_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	return playerCollision(apThis, apOther);
}

static void WaterGeyserPhysAboveCB1(daWaterGeyser_c *one, dStageActor_c *two) { }

static void WaterGeyserPhysBelowCB1(daWaterGeyser_c *one, dStageActor_c *two) { }

static void WaterGeyserPhysAdjCB1(daWaterGeyser_c *one, dStageActor_c *two, bool unkMaybeNotBool) { }

static bool WaterGeyserPhysAboveCB2(daWaterGeyser_c *one, dStageActor_c *two) {
	return (one->pos_delta.y > 0.0f);
}

static bool WaterGeyserPhysBelowCB2(daWaterGeyser_c *one, dStageActor_c *two) {
	return (one->pos_delta.y < 0.0f);
}

static bool WaterGeyserPhysAdjCB2(daWaterGeyser_c *one, dStageActor_c *two, bool unkMaybeNotBool) {
	return false;
}

void WaterGeyserTopCallback(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apOther->owner->stageActorType != dStageActor_c::PlayerType && apOther->owner->stageActorType != dStageActor_c::YoshiType) return;
	daPlBase_c *player = (daPlBase_c*)apOther->owner;

	if (!player->collMgr.isOnTopOfTile()) return;

	player->footnote_type = daPlBase_c::FOOTNOTE_WATER;
}

int daWaterGeyser_c::onCreate() {
	if(!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	for (int i = 0; i < 4; i++) {
		this->players[i] = NULL;
	}

	int color = this->settings >> 20 & 0xF;
	float width = 0.25 + ((this->settings >> 16 & 0xF) * 0.25);
	if (width == 0.25) width = 1.0;
	u8 heightSetting = this->settings >> 8 & 0xFF;
	this->height = heightSetting + 6;
	this->topHeight = height * 15.5 - 4; // 15.5 seems to be the magic number here

	this->effectSize = width * 2.0;

	this->mode = (this->eventId1 >> 3 & 0x1) == 1 ? EventControlled : TimeControlled;

	char eventNum;
	switch (this->mode) {
		case EventControlled:
			eventNum = (int)this->settings & 0xFF;
			this->triggeringEventID = (u64)1 << (eventNum - 1);
			this->invert = (this->eventId1 >> 2) & 0x1;
			this->lastEvState = (int)(!this->invert);

			this->multiUse = (this->eventId1 >> 7) & 0x1;
			break;

		case TimeControlled:
			this->riseTime = (this->eventId1 >> 4 & 0xF) * 60;
			if (this->riseTime == 0) this->riseTime = 360;
			break;
	}

	this->initialRiseDelay = (this->eventId2 >> 4 & 0xF) * 10;
	this->riseDelay = (this->eventId2 & 0xF) + 30;
	if (this->riseDelay == 30) this->riseDelay = WaterGeyserRiseDelayList[min<u8>(heightSetting, 15)];

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16];
	getSpriteTexResName(resName, color);
	this->resFile.data = getResource("water_geyser", resName);


	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("water_geyser");
	model.setup(mdl, &allocator, 0x224, 1, 0);

	nw4r::g3d::ResAnmTexSrt anmTexSrtRes = this->resFile.GetResAnmTexSrt("water_geyser");
	anmTexSrt.setup(mdl, anmTexSrtRes, &allocator, 0, 3);
	anmTexSrt.bindEntry(&model, anmTexSrtRes, 0, 1);
	anmTexSrt.bindEntry(&model, anmTexSrtRes, 1, 1);
	anmTexSrt.bindEntry(&model, anmTexSrtRes, 2, 1);
	anmTexSrt.setCurrentFrame(0.0);
	this->model.bindAnim(&anmTexSrt);


	nw4r::g3d::ResMdl mdlTop = this->resFile.GetResMdl("water_geyser_top");
	modelTop.setup(mdlTop, &allocator, 0x224, 1, 0);

	nw4r::g3d::ResAnmTexSrt anmTexSrtResTop = this->resFile.GetResAnmTexSrt("water_geyser_top");
	anmTexSrtTop.setup(mdlTop, anmTexSrtResTop, &allocator, 0, 3);
	anmTexSrtTop.bindEntry(&modelTop, anmTexSrtResTop, 0, 1);
	anmTexSrtTop.bindEntry(&modelTop, anmTexSrtResTop, 1, 1);
	anmTexSrtTop.bindEntry(&modelTop, anmTexSrtResTop, 2, 1);
	anmTexSrtTop.setCurrentFrame(0.0);
	this->modelTop.bindAnim(&anmTexSrtTop);


	nw4r::g3d::ResMdl mdlBehind = this->resFile.GetResMdl("water_geyser_behind");
	modelBehind.setup(mdlBehind, &allocator, 0x224, 1, 0);

	nw4r::g3d::ResAnmTexSrt anmTexSrtResBehind = this->resFile.GetResAnmTexSrt("water_geyser_behind");
	anmTexSrtBehind.setup(mdlBehind, anmTexSrtResBehind, &allocator, 0, 3);
	anmTexSrtBehind.bindEntry(&modelBehind, anmTexSrtResBehind, 0, 1);
	anmTexSrtBehind.bindEntry(&modelBehind, anmTexSrtResBehind, 1, 1);
	anmTexSrtBehind.bindEntry(&modelBehind, anmTexSrtResBehind, 2, 1);
	anmTexSrtBehind.setCurrentFrame(0.0);
	this->modelBehind.bindAnim(&anmTexSrtBehind);


	allocator.unlink();

	// Stuff I do understand

	this->scale = (Vec){width, height * 0.1, 1.0};
	this->topScale = (Vec){width, 1.0, 1.0};

	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>

	this->pos.z = 0.0;

	this->speed.x = 0.0;
	this->speed.y = 0.0;

	float w = 3.25 * 16 * width * 0.75;

	ActivePhysics::Info aPhysicsInfo;
	aPhysicsInfo.xDistToCenter = 0.0;
	aPhysicsInfo.yDistToCenter = 0.0;
	aPhysicsInfo.xDistToEdge = 2 * 16 * width;
	aPhysicsInfo.yDistToEdge = this->topHeight;
	aPhysicsInfo.category1 = 0x3;
	aPhysicsInfo.category2 = 0x16;
	aPhysicsInfo.bitfield1 = 0x2D;
	aPhysicsInfo.bitfield2 = 0x0;
	aPhysicsInfo.unkShort1C = 0x0;
	aPhysicsInfo.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &aPhysicsInfo);


	standOnTopCollider.init(this, 0, 0, this->topHeight + 4, w + 2, -w - 2, this->rot.z, 1);
	standOnTopCollider.type = 0;
	standOnTopCollider._47 = 0; // SandPillar is 0xC
	standOnTopCollider.flags = 0;

	ActivePhysics::Info topAPInfo;
	topAPInfo.xDistToCenter = 0.0;
	topAPInfo.yDistToCenter = this->topHeight + 4;
	topAPInfo.xDistToEdge = w;
	topAPInfo.yDistToEdge = 2;
	topAPInfo.category1 = 0x3;
	topAPInfo.category2 = 0x16;
	topAPInfo.bitfield1 = 0x2D;
	topAPInfo.bitfield2 = 0x0;
	topAPInfo.unkShort1C = 0x0;
	topAPInfo.callback = &WaterGeyserTopCallback;

	this->topAP.initWithStruct(this, &topAPInfo);


	this->topY = this->pos.y;
	this->totalHeight = this->height * 16;
	this->bottomY = this->pos.y - this->totalHeight;

	this->pos.y = this->bottomY;

	this->soundPos = (Vec){this->pos.x, this->pos.y + this->topHeight + 16.0, this->pos.z};

	this->risePerFrame = (this->totalHeight / this->riseDelay);

	switch (this->mode) {
		case TimeControlled:
			if (this->initialRiseDelay > 0) doStateChange(&StateID_InitialWait);
			else doStateChange(&StateID_UpSignal);
			break;

		case EventControlled:
			if (this->invert) {
				if (this->initialRiseDelay > 0) doStateChange(&StateID_InitialWait);
				else doStateChange(&StateID_UpSignal);
			}
			else doStateChange(&StateID_WaitDown);
			break;
	}

	this->onExecute();
	return true;
}

int daWaterGeyser_c::onDelete() {
	this->standOnTopCollider.removeFromList();
	this->topAP.removeFromList();
	if (this->handle.Exists()) this->handle.Stop(0);
	return true;
}

int daWaterGeyser_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	modelBehind._vf1C();
	anmTexSrtBehind.process();

	model._vf1C();
	anmTexSrt.process();

	modelTop._vf1C();
	anmTexSrtTop.process();

	standOnTopCollider.update();

	return true;
}

int daWaterGeyser_c::onDraw() {
	modelBehind.scheduleForDrawing();
	model.scheduleForDrawing();
	modelTop.scheduleForDrawing();
	return true;
}

void daWaterGeyser_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrixBehind.translation(pos.x, pos.y, pos.z);
	matrixBehind.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	modelBehind.setDrawMatrix(matrixBehind);
	modelBehind.setScale(&scale);
	modelBehind.calcWorld(false);


	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);


	matrixTop.translation(pos.x, pos.y + this->topHeight, pos.z + 1.0);
	matrixTop.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	modelTop.setDrawMatrix(matrixTop);
	modelTop.setScale(&topScale);
	modelTop.calcWorld(false);
}

void daWaterGeyser_c::executeSrtAnimations() {
	if (this->anmTexSrt.isEntryAnimationDone(0)) {
		this->anmTexSrt.setCurrentFrame(0.0);
	}
	if (this->anmTexSrtTop.isEntryAnimationDone(0)) {
		this->anmTexSrtTop.setCurrentFrame(0.0);
	}
	if (this->anmTexSrtBehind.isEntryAnimationDone(0)) {
		this->anmTexSrtBehind.setCurrentFrame(0.0);
	}
}

bool daWaterGeyser_c::areAllSrtAnimationsDone() {
	return this->anmTexSrt.isEntryAnimationDone(0) && this->anmTexSrtTop.isEntryAnimationDone(0) && this->anmTexSrtBehind.isEntryAnimationDone(0);
}

void daWaterGeyser_c::doFunsuiStuffWithPlayers(bool force = false) {
	for (int i = 0; i < 4; i++) {
		daPlBase_c *p = this->players[i];
		if (p == NULL) continue;

		if (
			p->aPhysics.right() < this->aPhysics.left() ||
			this->aPhysics.right() < p->aPhysics.left() ||
			p->aPhysics.bottom() > this->aPhysics.top() ||
			this->aPhysics.bottom() > p->aPhysics.top()
		) {
				this->players[i] = NULL;
				FUN_80802d30(this, p);

				if (this->aPhysics.top() <= p->aPhysics.top()) p->speed.y = 5.0;
		}
		else {
			p->speed.y = 2.0;
		}
	}
}

void daWaterGeyser_c::setSoundVolume() {
	setSoundDistance(&this->handle, this->soundPos, 0.75, 1.0, 750.0);
}

// void daWaterGeyser_c::spawnWaterDrops() {
// 	int w = ((int)(this->topAP.info.xDistToEdge / 12)) * 12;

// 	for (int i = -w; i <= w; i += 12) {
// 		Vec efxPos = { this->pos.x + i, this->pos.y + this->topAP.info.yDistToCenter, this->pos.z };
// 		SpawnEffect("", 0, &efxPos, &(S16Vec){0x8000, 0, 0}, &(Vec){this->effectSize, this->effectSize, this->effectSize}); // TODO: find a good effect
// 	}
// }

#define ACTIVATE	1
#define DEACTIVATE	0

bool daWaterGeyser_c::checkForEventTrigger(u8 lookForState) {
	bool eventTriggered = false;

	this->newEvState = 0;
	if (dFlagMgr_c::instance->flags & this->triggeringEventID) this->newEvState = 1;

	if (this->newEvState == this->lastEvState) return false;

	if (this->newEvState == lookForState) {
		this->offState = (this->newEvState == 1) ? 1 : 0;
		eventTriggered = true;
	}

	else this->offState = (this->newEvState == 1) ? 0 : 1;

	this->lastEvState = this->newEvState;

	return eventTriggered;
}

///////////////
// Up Signal State
///////////////
void daWaterGeyser_c::beginState_UpSignal() {
	this->timer = 0;
	SpawnEffect("Wm_en_cmnwater", 0, &this->soundPos, &(S16Vec){0, 0, 0}, &(Vec){this->effectSize, this->effectSize, this->effectSize});
	playSoundDistance(&this->handle, this->soundPos, SE_PLY_SPLASH_OUT, 1.5, 1.0, 750.0);
	playSoundDistance(&this->handle, this->soundPos, SE_BOSS_WENDY_WATER_UP, 0.75, 1.0, 750.0);
}
void daWaterGeyser_c::executeState_UpSignal() {
	if (this->timer >= 30) return doStateChange(&StateID_GoUp);
	setSoundVolume();
	this->timer++;
}
void daWaterGeyser_c::endState_UpSignal() { }

///////////////
// Go Up State
///////////////
void daWaterGeyser_c::beginState_GoUp() {
	this->bindAnimSrt_and_setUpdateRate("water_geyser", &this->model, &this->anmTexSrt, 1.0);
	this->bindAnimSrt_and_setUpdateRate("water_geyser_behind", &this->modelBehind, &this->anmTexSrtBehind, 1.0);
	this->bindAnimSrt_and_setUpdateRate("water_geyser_top", &this->modelTop, &this->anmTexSrtTop, 1.0);

	this->standOnTopCollider.addToList();
	this->topAP.addToList();
	this->aPhysics.addToList();
	this->pos.y = this->bottomY;
	this->timer = 0;
}
void daWaterGeyser_c::executeState_GoUp() {
	this->executeSrtAnimations();
	// this->spawnWaterDrops();

	if (this->timer >= this->riseDelay) return doStateChange(&StateID_Stabilize);
	this->pos.y += this->risePerFrame;
	this->timer++;

	this->doFunsuiStuffWithPlayers();
	setSoundVolume();
}
void daWaterGeyser_c::endState_GoUp() { }

///////////////
// Stabilize State
///////////////
void daWaterGeyser_c::beginState_Stabilize() {
	this->pos.y = this->topY;
	this->timer = 0;
}
void daWaterGeyser_c::executeState_Stabilize() {
	this->executeSrtAnimations();
	// this->spawnWaterDrops();

	if (this->timer >= 150) return doStateChange(&StateID_WaitUp);
	this->pos.y = this->topY + sin(((float)this->timer / 150.0) * 2.0 * M_PI) * 8.0;
	this->timer++;

	this->doFunsuiStuffWithPlayers();
	setSoundVolume();
}
void daWaterGeyser_c::endState_Stabilize() {
	this->pos.y = this->topY;
}

///////////////
// Wait Up State
///////////////
void daWaterGeyser_c::beginState_WaitUp() {
	this->timer = 0;
}
void daWaterGeyser_c::executeState_WaitUp() {
	this->executeSrtAnimations();
	// this->spawnWaterDrops();

	switch (this->mode) {
		case TimeControlled:
			if (this->timer >= this->riseTime) return doStateChange(&StateID_GoDown);
			this->timer++;
			break;

		case EventControlled:
			if (this->checkForEventTrigger(this->invert ? ACTIVATE : DEACTIVATE)) {
				return doStateChange(&StateID_GoDown);
			}
			break;
	}


	this->doFunsuiStuffWithPlayers();
	setSoundVolume();
}
void daWaterGeyser_c::endState_WaitUp() { }

///////////////
// Go Down State
///////////////
void daWaterGeyser_c::beginState_GoDown() {
	this->bindAnimSrt_and_setUpdateRate("water_geyser_end", &this->model, &this->anmTexSrt, 1.0);
	this->bindAnimSrt_and_setUpdateRate("water_geyser_behind_end", &this->modelBehind, &this->anmTexSrtBehind, 1.0);
	this->bindAnimSrt_and_setUpdateRate("water_geyser_top_end", &this->modelTop, &this->anmTexSrtTop, 1.0);

	if (this->handle.Exists()) this->handle.Stop((int)this->anmTexSrt.getFrameForEntry(0));
}
void daWaterGeyser_c::executeState_GoDown() {
	if (this->areAllSrtAnimationsDone()) {
		return doStateChange(&StateID_WaitDown);
	}

	this->doFunsuiStuffWithPlayers();
}
void daWaterGeyser_c::endState_GoDown() {
	this->aPhysics.removeFromList();
	this->standOnTopCollider.removeFromList();
	this->topAP.removeFromList();
	this->doFunsuiStuffWithPlayers(true);
	// FUN_80802b40(this);
	this->pos.y = this->bottomY;
}

///////////////
// Wait Down State
///////////////
void daWaterGeyser_c::beginState_WaitDown() {
	this->doFunsuiStuffWithPlayers(true);
	this->timer = 0;
}
void daWaterGeyser_c::executeState_WaitDown() {
	switch (this->mode) {
			case TimeControlled:
				if (this->timer >= this->riseDelay) return doStateChange(&StateID_UpSignal);
				this->timer++;
				break;

			case EventControlled:
				if (this->checkForEventTrigger(this->invert ? DEACTIVATE : ACTIVATE)) {
					if (this->initialRiseDelay > 0) doStateChange(&StateID_InitialWait);
					else doStateChange(&StateID_UpSignal);
				}
				break;
		}
}
void daWaterGeyser_c::endState_WaitDown() { }

///////////////
// Initial Wait State
///////////////
void daWaterGeyser_c::beginState_InitialWait() {
	this->timer = 0;
}
void daWaterGeyser_c::executeState_InitialWait() {
	if (this->timer >= this->initialRiseDelay) return doStateChange(&StateID_UpSignal);
	this->timer++;
}
void daWaterGeyser_c::endState_InitialWait() { }



//////////////////////////////////////////////////////////////////
// Sand Pillar Related Stuff
//////////////////////////////////////////////////////////////////

extern "C" void FUN_8008fba0(int playerNum, mEf::levelEffect_c *effect, char *effName, uint creatorId, Vec *pos, S16Vec *rot, Vec *scale); // Funsui particle effect
extern "C" void FUN_808025b0(ActivePhysics *apThis, ActivePhysics *apOther); // Sand pillar collision callback

void replaceSandPillarParticles(int playerNum, mEf::levelEffect_c *effect, char *effName, uint creatorId, Vec *pos, S16Vec *rot, Vec *scale) {
	char *newEffName = pillarEffect[lastPillarEffectType[playerNum]];
	return FUN_8008fba0(playerNum, effect, newEffName, creatorId, pos, rot, scale);
}

void newSandPillarCallback(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apOther->owner->stageActorType == apOther->owner->PlayerType) {
		int playerNum = apOther->owner->which_player;
		lastPillarEffectType[playerNum] = 0;
	}
	return FUN_808025b0(apThis, apOther);
}
