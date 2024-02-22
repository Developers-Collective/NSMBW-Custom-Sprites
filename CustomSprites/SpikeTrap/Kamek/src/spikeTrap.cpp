#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>


const char* SpikeTrapArcNameList [] = {
	"spike_trap",
	NULL
};

class daSpikeTrap_c : public dEn_c {
	enum SpikeTrapType {
		STEachXAmountOfTime,
		STWhenEventIDIsTriggered,
		STWhenEventIDIsNotTriggered,
		STAlwaysActivated,
	};

	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	void normalActorCollision();
	bool checkForEventTrigger(u8 lookForState);

	bool ranOnce;

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c model;
	Physics physics;

	u16 timer, timeOffset, timeActivatedOrOffset, timeDeactivatedOrOffset;
	Vec basePos;
	Vec2 dir;

	SpikeTrapType type;

	u8 lastEvState;
	u8 newEvState;
	u8 offState;
	u64 eventID;

	bool disableSFX;

	nw4r::snd::SoundHandle handle;

	void updateModelMatrices();


	USING_STATES(daSpikeTrap_c);
	DECLARE_STATE(WaitForOffset);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Sign);
	DECLARE_STATE(Attack);
	DECLARE_STATE(End);

	public: static dActor_c *build();
};

dActor_c *daSpikeTrap_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daSpikeTrap_c));
	return new(buffer) daSpikeTrap_c;
}

const SpriteData SpikeTrapSpriteData = { ProfileId::SpikeTrap, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0x10 };
Profile SpikeTrapProfile(&daSpikeTrap_c::build, SpriteId::SpikeTrap, &SpikeTrapSpriteData, ProfileId::SpikeTrap, ProfileId::SpikeTrap, "SpikeTrap", SpikeTrapArcNameList, 0);

///////////////////////
// Externs and States
///////////////////////
extern "C" void HurtMarioBecauseOfBeingSquashed(void *mario, dStageActor_c *squasher, int type);

CREATE_STATE(daSpikeTrap_c, WaitForOffset);
CREATE_STATE(daSpikeTrap_c, Wait);
CREATE_STATE(daSpikeTrap_c, Sign);
CREATE_STATE(daSpikeTrap_c, Attack);
CREATE_STATE(daSpikeTrap_c, End);

////////////////////////
// Collision Functions
////////////////////////
static void SpikeTrapPhysBelowCB1(daSpikeTrap_c *one, dStageActor_c *two) {
    if (two->stageActorType != 1 && two->stageActorType != 2)
		return;

	one->_vf220(two);

    if (one->pos_delta.y > 0.0f)
        HurtMarioBecauseOfBeingSquashed(two, one, 1);
    else
        HurtMarioBecauseOfBeingSquashed(two, one, 9);
}

static void SpikeTrapPhysAboveCB1(daSpikeTrap_c *one, dStageActor_c *two) {
    if (two->stageActorType != 1 && two->stageActorType != 2)
		return;

	one->_vf220(two);

    if (one->pos_delta.y < 0.0f)
        HurtMarioBecauseOfBeingSquashed(two, one, 2);
    else
        HurtMarioBecauseOfBeingSquashed(two, one, 10);
}

static void SpikeTrapPhysAjdCB1(daSpikeTrap_c *one, dStageActor_c *two, bool unkMaybeNotBool) {
    if (two->stageActorType != 1 && two->stageActorType != 2)
		return;

	one->_vf220(two);

    if (unkMaybeNotBool) {
        if (one->pos_delta.x > 0.0f)
            HurtMarioBecauseOfBeingSquashed(two, one, 6);
        else
            HurtMarioBecauseOfBeingSquashed(two, one, 12);
    } else {
        if (one->pos_delta.x < 0.0f)
            HurtMarioBecauseOfBeingSquashed(two, one, 5);
        else
            HurtMarioBecauseOfBeingSquashed(two, one, 11);
    }
}

static bool SpikeTrapPhysBelowCB2(daSpikeTrap_c *one, dStageActor_c *two) {
    return (one->pos_delta.y > 0.0f);
}

static bool SpikeTrapPhysAboveCB2(daSpikeTrap_c *one, dStageActor_c *two) {
    return (one->pos_delta.y < 0.0f);
}

static bool SpikeTrapPhysAdjCB2(daSpikeTrap_c *one, dStageActor_c *two, bool unkMaybeNotBool) {
    if (unkMaybeNotBool) {
        if (one->pos_delta.x > 0.0f)
            return true;
    } else {
        if (one->pos_delta.x < 0.0f)
            return true;
    }
    return false;
}



int daSpikeTrap_c::onCreate() {
	if (!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	int size = (this->eventId2 >> 6) & 0x3;
	int color = (this->eventId2) & 0xF;

	this->type = (SpikeTrapType)((this->settings >> 22) & 0x3);

	State *state;

	this->timeOffset = 0;
	this->timeActivatedOrOffset = 0;
	this->timeDeactivatedOrOffset = 0;

	this->disableSFX = (this->eventId2 >> 5) & 0x1;

	char eventNum;

	switch (this->type) {
		case STEachXAmountOfTime:
			this->timeOffset = ((this->settings >> 16) & 0x3F) * 6;
			this->timeActivatedOrOffset = ((this->settings >> 8) & 0xFF) * 3;
			this->timeDeactivatedOrOffset = ((this->settings) & 0xFF) * 3;
			state = this->timeOffset == 0 ? &StateID_Wait : &StateID_WaitForOffset;
			break;

		case STWhenEventIDIsTriggered:
		case STWhenEventIDIsNotTriggered:
			this->lastEvState = 0xF;
			eventNum = (this->settings >> 16) & 0x3F;
			this->eventID = (u64)1 << (eventNum - 1);
			state = this->type == STWhenEventIDIsTriggered ? &StateID_Wait : &StateID_Attack;
			break;

		case STAlwaysActivated:
			this->disableSFX = true;
			state = &StateID_Attack;
			break;
	}

	u16 rotation = this->eventId1 * 256;
	this->dir = (Vec2){0.0, 1.0};
	Vec2 newVec = { 0.0f, 0.0f };
	float r = rotation * M_PI / 0x8000;
	float s = sin(r);
	float c = cos(r);
	newVec.x = dir.x * c - dir.y * s;
	newVec.y = dir.x * s + dir.y * c;
	dir = newVec;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16];
	getSpriteTexResName(resName, color);
	this->resFile.data = getResource("spike_trap", resName);
	char mdlName[16];
	sprintf(mdlName, "spike_trap_%s", size == 0 ? "S" : (size == 1 ? "M" : "L"));
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl(mdlName);
	model.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&model, 0);

	allocator.unlink();


	this->scale = (Vec){1.0, 1.0, 1.0};

	this->rot.x = 0;
	this->rot.y = 0;
	this->rot.z = -atan2(dir.x, dir.y) * 0x8000 / M_PI;


	Physics::Info colliderInfo;

	colliderInfo.x1 = -7.5;
	colliderInfo.y1 = 4.0;
	colliderInfo.x2 = 7.5;
	colliderInfo.y2 = -8.0 - (size == 0 ? 0.0 : 16.0);
	colliderInfo.otherCallback1 = &SpikeTrapPhysBelowCB1;
	colliderInfo.otherCallback2 = &SpikeTrapPhysAboveCB1;
	colliderInfo.otherCallback3 = &SpikeTrapPhysAjdCB1;
	physics.setup(this, &colliderInfo, 3, currentLayerID, 0);
	physics.flagsMaybe = 0x260;
	physics.callback1 = (void*)&SpikeTrapPhysBelowCB2;
	physics.callback2 = (void*)&SpikeTrapPhysAboveCB2;
	physics.callback3 = (void*)&SpikeTrapPhysAdjCB2;
	physics.setPtrToRotation(&rot.z);

	this->pos.x -= 14.0 * dir.x;
	this->pos.y -= 14.0 * dir.y;
	this->pos.z = -300.0;

	this->basePos.x = this->pos.x;
	this->basePos.y = this->pos.y;

	doStateChange(state);

	this->onExecute();
	return true;
}

int daSpikeTrap_c::onDelete() {
	return true;
}

int daSpikeTrap_c::onExecute() {
	acState.execute();
	physics.update();
	updateModelMatrices();
	model._vf1C();

	return true;
}

int daSpikeTrap_c::onDraw() {
	model.scheduleForDrawing();
	return true;
}

void daSpikeTrap_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.

	if (this->frzMgr._mstate == 1)
		matrix.translation(pos.x, pos.y + 2.0, pos.z);
	else
		matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);
}



bool daSpikeTrap_c::checkForEventTrigger(u8 lookForState) {
	bool eventTriggered = false;

	this->newEvState = 0;
	if (dFlagMgr_c::instance->flags & this->eventID) this->newEvState = 1;

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
// Wait For Offset State
///////////////
void daSpikeTrap_c::beginState_WaitForOffset() {
	this->timer = 0;
}
void daSpikeTrap_c::executeState_WaitForOffset() {
	if (this->timer >= this->timeOffset) doStateChange(&StateID_Wait);
	this->timer++;
}
void daSpikeTrap_c::endState_WaitForOffset() { }

///////////////
// Wait State
///////////////
void daSpikeTrap_c::beginState_Wait() {
	this->timer = 0;
}
void daSpikeTrap_c::executeState_Wait() {
	switch (this->type) {
		case STEachXAmountOfTime:
			if (this->timer >= this->timeDeactivatedOrOffset) doStateChange(&StateID_Sign);
			this->timer++;
			break;

		case STWhenEventIDIsTriggered:
			if (this->checkForEventTrigger(1)) doStateChange(&StateID_Sign);
			break;

		case STWhenEventIDIsNotTriggered:
			if (this->checkForEventTrigger(0)) doStateChange(&StateID_Sign);
			break;
	}
}
void daSpikeTrap_c::endState_Wait() {
	this->timer = 0;
}

///////////////
// Sign State
///////////////
void daSpikeTrap_c::beginState_Sign() {
	this->timer = 0;
}
void daSpikeTrap_c::executeState_Sign() {
	if (this->timer < 10) {
		this->pos.x = this->basePos.x + (this->dir.x * (this->timer / 10.0) * 4.0);
		this->pos.y = this->basePos.y + (this->dir.y * (this->timer / 10.0) * 4.0);
	}
	else if (this->timer == 10) {
		this->pos.x = this->basePos.x + (this->dir.x * 6.0);
		this->pos.y = this->basePos.y + (this->dir.y * 6.0);
	}

	if (this->timer >= 60) doStateChange(&StateID_Attack);
	this->timer++;
}
void daSpikeTrap_c::endState_Sign() { }

///////////////
// Attack State
///////////////
void daSpikeTrap_c::beginState_Attack() {
	this->timer = 0;
	physics.addToList();

	if (!this->disableSFX) playSoundDistance(&this->handle, this->pos, SFX_SPIKE_TRAP, 1.0, 1.0, 500.0);

	if (this->type == STAlwaysActivated) {
		this->pos.x = this->basePos.x + (this->dir.x * 16.0);
		this->pos.y = this->basePos.y + (this->dir.y * 16.0);
	}
}
void daSpikeTrap_c::executeState_Attack() {
	if (this->timer < 5 && this->type != STAlwaysActivated) {
		this->pos.x = this->basePos.x + (this->dir.x * 4.0) + (this->dir.x * (this->timer / 5.0) * 12.0);
		this->pos.y = this->basePos.y + (this->dir.y * 4.0) + (this->dir.y * (this->timer / 5.0) * 12.0);
	}
	else if (this->timer == 5) {
		this->pos.x = this->basePos.x + (this->dir.x * 16.0);
		this->pos.y = this->basePos.y + (this->dir.y * 16.0);
	}

	switch (this->type) {
		case STEachXAmountOfTime:
			if (this->timer >= 5 + this->timeActivatedOrOffset) doStateChange(&StateID_End);
			this->timer++;
			break;

		case STWhenEventIDIsTriggered:
		case STWhenEventIDIsNotTriggered:
			if (this->timer <= 5) this->timer++;
			else if (this->checkForEventTrigger(this->type == STWhenEventIDIsTriggered ? 0 : 1)) doStateChange(&StateID_End);
			break;
	}
}
void daSpikeTrap_c::endState_Attack() { }

///////////////
// End State
///////////////
void daSpikeTrap_c::beginState_End() {
	this->timer = 0;
}
void daSpikeTrap_c::executeState_End() {
	if (this->timer < 10) {
		this->pos.x = this->basePos.x + (this->dir.x * ((10.0 - this->timer) / 10.0) * 16.0);
		this->pos.y = this->basePos.y + (this->dir.y * ((10.0 - this->timer) / 10.0) * 16.0);
	}

	if (this->timer >= 10) doStateChange(&StateID_Wait);
	this->timer++;
}
void daSpikeTrap_c::endState_End() {
	this->pos.x = this->basePos.x;
	this->pos.y = this->basePos.y;

	physics.removeFromList();
}
