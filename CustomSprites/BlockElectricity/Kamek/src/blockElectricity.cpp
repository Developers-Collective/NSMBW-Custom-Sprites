#include <common.h>
#include <game.h>
#include <profile.h>
#include <sfx.h>
#include "rotatingActivePhysics.h"
#include "light.h"
#include "lightPath.h"

const char *BlockElectricityFileList[] = {"block_electricity", 0};
const u8 BlockElectricityPrepareTime = 30;

class daEnBlockElectricity_c : public daEnBlockMain_c {
private:
	enum BlockElectricityType {
		BEEachXAmountOfTime,
		BEWhenReceivingElectricity,
		BEWhenEventIDIsTriggered,
		BEConstantState,
	};

	bool setReceiveFromObjects();
	bool checkForEventTrigger(u8 lookForState);

	daEnBlockElectricity_c* sendTo[16];
	u8 sendToCount;

	daEnBlockElectricity_c* receiveFrom;
	float signalFromDistance;
	Vec2 signalFromDirection;

	u16 pinRotation;
	bool pinRotationSet;

	bool ranOnce;

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel, pinModel;

	m3d::anmTexPat_c bodyAnimationPat, pinAnimationPat;
	nw4r::g3d::ResAnmTexPat bodyAnmPat, pinAnmPat;

	BlockElectricityType type;
	u8 timeOffset, timeActivated;
	u64 timeDeactivatedOrTriggeringEventID;
	bool isInvisible;

	mEf::es2 lightning;
	mEf::es2 sparkStart, sparkEnd;

	u8 timer;

	bool isInverted;
	char sparkEfxName[32];
	char lightningEfxName[32];
	char prepareEfxName[32];

	u8 lastEvState;
	u8 newEvState;
	u8 offState;

	Physics::Info physicsInfo;
	RotatingActivePhysics rotatingAPhysics;

	Light::dLightPath_c lightPath;

	nw4r::snd::SoundHandle handle; // Sound Handle

	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daEnBlockElectricity_c);
	DECLARE_STATE(Init);
	DECLARE_STATE(WaitForOffset);
	DECLARE_STATE(Wait);
	DECLARE_STATE(WaitForPreparation);
	DECLARE_STATE(ElectricityPreparation);
	DECLARE_STATE(Electricity);

public:
	void refReceiver(daEnBlockElectricity_c *sender);
	void triggerPreparation() { doStateChange(&daEnBlockElectricity_c::StateID_WaitForPreparation); }

	u16 receiverID, senderID;

	static dActor_c *build();
};

dActor_c *daEnBlockElectricity_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnBlockElectricity_c));
	daEnBlockElectricity_c *c = new(buffer) daEnBlockElectricity_c;

	return c;
}

const SpriteData BlockElectricitySpriteData = { ProfileId::BlockElectricity, 8, -8, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0xA };
Profile BlockElectricityProfile(&daEnBlockElectricity_c::build, SpriteId::BlockElectricity, &BlockElectricitySpriteData, ProfileId::BlockElectricity, ProfileId::BlockElectricity, "BlockElectricity", BlockElectricityFileList, 0);


CREATE_STATE(daEnBlockElectricity_c, Init);
CREATE_STATE(daEnBlockElectricity_c, WaitForOffset);
CREATE_STATE(daEnBlockElectricity_c, Wait);
CREATE_STATE(daEnBlockElectricity_c, WaitForPreparation);
CREATE_STATE(daEnBlockElectricity_c, ElectricityPreparation);
CREATE_STATE(daEnBlockElectricity_c, Electricity);


#define ACTIVATE	1
#define DEACTIVATE	0


extern "C" void dAcPy_vf3F4(void* mario, void* other, int t);

void daEnBlockElectricity_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (this->counter_504[apOther->owner->which_player]) dAcPy_vf3F4(apOther->owner, this, 9);
	this->counter_504[apOther->owner->which_player] = 0x20;
}

void daEnBlockElectricity_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
bool daEnBlockElectricity_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
bool daEnBlockElectricity_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
bool daEnBlockElectricity_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
bool daEnBlockElectricity_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
bool daEnBlockElectricity_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
bool daEnBlockElectricity_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }

bool daEnBlockElectricity_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { return false; }
bool daEnBlockElectricity_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) { return false; }

bool daEnBlockElectricity_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daEnBlockElectricity_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daEnBlockElectricity_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}


bool daEnBlockElectricity_c::setReceiveFromObjects()
{
	daEnBlockElectricity_c *obj;

	obj = (daEnBlockElectricity_c *)0x0;
	int sendCount = 0;

	while (obj = (daEnBlockElectricity_c *)fBase_c::searchByProfileId(ProfileId::BlockElectricity, (fBase_c *)obj)) {
		if (obj->senderID != this->receiverID) continue;

		receiveFrom = obj;
		signalFromDirection = (Vec2){ pos.x - obj->pos.x, -(pos.y - obj->pos.y) };
		pinRotation = atan2(signalFromDirection.x, signalFromDirection.y) * (0x8000 / M_PI);
		this->signalFromDistance = sqrtf(signalFromDirection.x * signalFromDirection.x + signalFromDirection.y * signalFromDirection.y);

		ActivePhysics::Info LightningInfo;
		LightningInfo.xDistToCenter = 0.0f;
		LightningInfo.yDistToCenter = this->signalFromDistance / 2.0f;
		LightningInfo.xDistToEdge = 7.0f;
		LightningInfo.yDistToEdge = (this->signalFromDistance / 2.0f) - 8.0f;
		LightningInfo.category1 = 0x3;
		LightningInfo.category2 = 0x9;
		LightningInfo.bitfield1 = 0x4D;
		LightningInfo.bitfield2 = 0x420;
		LightningInfo.unkShort1C = 0;
		LightningInfo.callback = &dEn_c::collisionCallback;

		rotatingAPhysics.initWithStruct(this, &LightningInfo);
		rotatingAPhysics.setRotation(pinRotation);

		this->pinRotationSet = true;
		obj->refReceiver(this);

		Light::dLightPath_c::Info lightInfo;
		lightInfo.distFromCenter = (Vec2){ 0.0f, 0.0f };
		lightInfo.distanceAbove = max(this->signalFromDistance - 8.0f, 8.0f);
		lightInfo.distanceBelow = 8.0f;
		lightInfo.lightType = (u32)Light::Dark;
		lightInfo.scale = 72.0f;
		lightInfo.zPos = pos.z + 500;

		this->lightPath.initWithStruct(this, &lightInfo);
		this->lightPath.setRotation(pinRotation);

		return true;
	};

	return false;
}


void daEnBlockElectricity_c::refReceiver(daEnBlockElectricity_c *sender)
{
	if (this->sendToCount >= 16) return OSReport("daEnBlockElectricity_c::refReceiver: Too many receivers!\n");

	if (!this->pinRotationSet && receiverID == 0) {
		this->signalFromDirection = (Vec2){ pos.x - sender->pos.x, -(pos.y - sender->pos.y) };
		this->signalFromDistance = sqrtf(signalFromDirection.x * signalFromDirection.x + signalFromDirection.y * signalFromDirection.y);
		this->pinRotation = atan2(signalFromDirection.x, signalFromDirection.y) * (0x8000 / M_PI);
		this->pinRotationSet = true;
	}

	this->sendTo[this->sendToCount] = sender;
	this->sendToCount++;
}


int daEnBlockElectricity_c::onCreate() {
	if(!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	this->pinRotationSet = false;
	this->sendToCount = 0;

	this->isInvisible = (this->settings >> 31) & 0x1;

	int color = (this->settings >> 24) & 0xF;
	this->type = (BlockElectricityType)((this->settings >> 28) & 0x3);

	sprintf(this->sparkEfxName, "block_electricity_spark%02d", color);
	this->sparkEfxName[strlen(this->sparkEfxName)] = 0;
	sprintf(this->lightningEfxName, "block_electricity_thunder%02d", color);
	this->lightningEfxName[strlen(this->lightningEfxName)] = 0;
	sprintf(this->prepareEfxName, "block_electricity_prepare%02d", color);
	this->prepareEfxName[strlen(this->prepareEfxName)] = 0;

	this->receiverID = this->eventId2;
	this->senderID = this->eventId1;

	this->timeActivated = ((this->settings >> 16) & 0xFF) * 3;
	if (this->timeActivated == 0) this->timeActivated = 60;

	this->timeDeactivatedOrTriggeringEventID = ((this->settings) & 0xFF) * 3;
	if (this->type != BEWhenEventIDIsTriggered && this->timeDeactivatedOrTriggeringEventID == 0) this->timeDeactivatedOrTriggeringEventID = 60;

	if (this->type == BEWhenEventIDIsTriggered) {
		this->lastEvState = 0xF;
		char eventNum	= (this->settings) & 0xFF;
		this->timeDeactivatedOrTriggeringEventID = (u64)1 << (eventNum - 1);
	}

	this->timeOffset = ((this->settings >> 8) & 0xFF) * 3;

	this->isInverted = (this->settings >> 28) & 0x4;

	if (!this->isInvisible) {
		allocator.link(-1, GameHeaps[0], 0, 0x20);

		char resName[16];
		getSpriteTexResName(resName, color);
		this->resFile.data = getResource("block_electricity", resName);

		nw4r::g3d::ResMdl bodyMdl = this->resFile.GetResMdl("block_electricity");
		bodyModel.setup(bodyMdl, &allocator, 0x227, 1, 0);
		SetupTextures_MapObj(&bodyModel, 0);

		this->bodyAnmPat = this->resFile.GetResAnmTexPat("block_electricity");
		this->bodyAnimationPat.setup(bodyMdl, bodyAnmPat, &this->allocator, 0, 1);
		this->bodyAnimationPat.bindEntry(&this->bodyModel, &bodyAnmPat, 0, 1);
		this->bodyAnimationPat.setFrameForEntry(this->type == BEConstantState ? 1 : 0, 0);
		this->bodyAnimationPat.setUpdateRateForEntry(0.0f, 0);
		this->bodyModel.bindAnim(&this->bodyAnimationPat);

		nw4r::g3d::ResMdl pinMdl = this->resFile.GetResMdl("pin");
		this->pinModel.setup(pinMdl, &allocator, 0x227, 1, 0);
		SetupTextures_MapObj(&this->pinModel, 0);

		this->pinAnmPat = this->resFile.GetResAnmTexPat("pin");
		this->pinAnimationPat.setup(pinMdl, this->pinAnmPat, &this->allocator, 0, 1);
		this->pinAnimationPat.bindEntry(&this->pinModel, &this->pinAnmPat, 0, 1);
		this->pinAnimationPat.setFrameForEntry(this->type == BEConstantState ? 1 : 0, 0);
		this->pinAnimationPat.setUpdateRateForEntry(0.0f, 0);
		this->pinModel.bindAnim(&this->pinAnimationPat);

		allocator.unlink();

		physicsInfo.x1 = -8;
		physicsInfo.y1 = 8;
		physicsInfo.x2 = 8;
		physicsInfo.y2 = -8;

		physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
		physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
		physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

		physics.setup(this, &physicsInfo, 3, currentLayerID);
		physics.flagsMaybe = 0x260;
		physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
		physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
		physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
		physics.addToList();
	}

	doStateChange(&daEnBlockElectricity_c::StateID_Init);

	return true;
}


int daEnBlockElectricity_c::onDelete() {
	if (!this->isInvisible) physics.removeFromList();

	return true;
}


int daEnBlockElectricity_c::onExecute() {
	acState.execute();
	if (!this->isInvisible) physics.update();
	if (this->acState.getCurrentState() == &StateID_Electricity) this->lightPath.execute();

	return true;
}


int daEnBlockElectricity_c::onDraw() {
	if (!this->isInvisible) {
		matrix.translation(pos.x, pos.y, pos.z);
		matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

		if (this->pinRotationSet) {
			mMtx pinMtx;
			s16 rotZ = pinRotation;
			pinMtx.translation(pos.x, pos.y, pos.z - 10);
			pinMtx.applyRotationYXZ(&rot.x, &rot.y, &rotZ);

			pinModel.setDrawMatrix(pinMtx);
			pinModel.setScale(&scale);
			pinModel.calcWorld(false);
			pinModel.scheduleForDrawing();
		}

		bodyModel.setDrawMatrix(matrix);
		bodyModel.setScale(&scale);
		bodyModel.calcWorld(false);
		bodyModel.scheduleForDrawing();
	}

	if (this->acState.getCurrentState() == &StateID_Electricity) this->lightPath.draw();

	return true;
}



bool daEnBlockElectricity_c::checkForEventTrigger(u8 lookForState) {
	bool eventTriggered = false;

	this->newEvState = 0;
	if (dFlagMgr_c::instance->flags & receiveFrom->timeDeactivatedOrTriggeringEventID) this->newEvState = 1;

	if (this->newEvState == this->lastEvState) return false;

	if (this->newEvState == lookForState) {
		this->offState = (this->newEvState == 1) ? 1 : 0;
		eventTriggered = true;
	}

	else this->offState = (this->newEvState == 1) ? 0 : 1;

	this->lastEvState = this->newEvState;

	return eventTriggered;
}



void daEnBlockElectricity_c::beginState_Init() { }

void daEnBlockElectricity_c::executeState_Init() {
	if (this->receiverID == 0) return doStateChange(&daEnBlockElectricity_c::StateID_Wait);

	if (setReceiveFromObjects()) {
		if (receiveFrom->type == BEEachXAmountOfTime) return doStateChange(&daEnBlockElectricity_c::StateID_WaitForOffset);
		else if (receiveFrom->type == BEWhenEventIDIsTriggered && receiveFrom->isInverted) return doStateChange(&daEnBlockElectricity_c::StateID_Electricity);
		else if (receiveFrom->type == BEConstantState && receiveFrom->isInverted) return doStateChange(&daEnBlockElectricity_c::StateID_Electricity);
		else return doStateChange(&daEnBlockElectricity_c::StateID_Wait);
	}
}

void daEnBlockElectricity_c::endState_Init() { }



/////////////////////////////
// Wait For Offset State
/////////////////////////////
void daEnBlockElectricity_c::beginState_WaitForOffset() {
	timer = 0;
}

void daEnBlockElectricity_c::executeState_WaitForOffset() {
	if (receiveFrom == 0) return;

	switch (receiveFrom->type)
	{
	case BEEachXAmountOfTime:
		timer++;
		if (timer >= receiveFrom->timeOffset) return doStateChange(&daEnBlockElectricity_c::StateID_Wait);
		break;

	case BEWhenEventIDIsTriggered:
	case BEConstantState:
	case BEWhenReceivingElectricity:
	default:
		break;
	}
}

void daEnBlockElectricity_c::endState_WaitForOffset() { }



////////////////
// Wait State
////////////////
void daEnBlockElectricity_c::beginState_Wait() {
	timer = 0;
}

void daEnBlockElectricity_c::executeState_Wait() {
	if (receiveFrom == 0) return;

	switch (receiveFrom->type)
	{
	case BEEachXAmountOfTime:
		timer++;
		if (timer >= receiveFrom->timeDeactivatedOrTriggeringEventID) return doStateChange(&daEnBlockElectricity_c::StateID_ElectricityPreparation);
		break;

	case BEWhenEventIDIsTriggered:
		if (this->checkForEventTrigger(receiveFrom->isInverted ? DEACTIVATE : ACTIVATE)) return doStateChange(&daEnBlockElectricity_c::StateID_ElectricityPreparation);
		break;

	case BEConstantState:
	case BEWhenReceivingElectricity:
	default:
		break;
	}
}

void daEnBlockElectricity_c::endState_Wait() { }



/////////////////////////////////////
// Wait For Preparation State
/////////////////////////////////////
void daEnBlockElectricity_c::beginState_WaitForPreparation() {
	timer = 0;
}

void daEnBlockElectricity_c::executeState_WaitForPreparation() {
	if (receiveFrom == 0) return doStateChange(&daEnBlockElectricity_c::StateID_Wait);

	timer++;
	if (timer >= receiveFrom->timeOffset) return doStateChange(&daEnBlockElectricity_c::StateID_ElectricityPreparation);
}

void daEnBlockElectricity_c::endState_WaitForPreparation() { }



/////////////////////////////////////
// Electricity Preparation State
/////////////////////////////////////
void daEnBlockElectricity_c::beginState_ElectricityPreparation() {
	timer = 0;

	for (int i = 0; i < sendToCount; i++) {
		if (sendTo[i] != 0) sendTo[i]->triggerPreparation();
	}
}

void daEnBlockElectricity_c::executeState_ElectricityPreparation() {
	if (receiveFrom == 0) return doStateChange(&daEnBlockElectricity_c::StateID_Wait);

	Vec2 normalizedDirection = (Vec2){ signalFromDirection.x / signalFromDistance, signalFromDirection.y / signalFromDistance };

	float dist = max(signalFromDistance - 16.0, 0.0);
	float dist2 = max(dist - 32.0, 0.0);

	lightning.spawn(
		receiveFrom->prepareEfxName,
		0,
		&(Vec){
			pos.x + (normalizedDirection.x * ((dist2 / 4.0))) - (normalizedDirection.y * 2.0),
			pos.y - (normalizedDirection.y * ((dist2 / 4.0))) - (normalizedDirection.x * 2.0),
			pos.z - 500
		},
		&(S16Vec){0, 0, this->pinRotation},
		&(Vec){0.1, ((0.1 / 3.5) / 16.0) * dist, 0.1}
	);

	timer++;
	u8 maxTimer = (receiveFrom->type == BEWhenEventIDIsTriggered) ? receiveFrom->timeOffset : BlockElectricityPrepareTime;
	if (timer >= maxTimer) return doStateChange(&daEnBlockElectricity_c::StateID_Electricity);
}

void daEnBlockElectricity_c::endState_ElectricityPreparation() { }


/////////////////////////////
// Electricity State
/////////////////////////////
void daEnBlockElectricity_c::beginState_Electricity() {
	if (receiveFrom != 0) {
		playSoundDistance(&this->handle, this->pos, SFX_BLOCK_ELECTRICITY, 1.0, 1.0, 1000.0);
		rotatingAPhysics.addToList();
	}
	timer = 0;
}

void daEnBlockElectricity_c::executeState_Electricity() {
	if (receiveFrom == 0) return doStateChange(&daEnBlockElectricity_c::StateID_Wait);
	if (!this->pinRotationSet) return doStateChange(&daEnBlockElectricity_c::StateID_Wait);

	Vec2 normalizedDirection = (Vec2){ signalFromDirection.x / signalFromDistance, signalFromDirection.y / signalFromDistance };

	float dist = max(signalFromDistance - 24.0, 0.0);

	float sparkMultiplier = 1.0;
	float sparkOffset = 0.0;
	if (this->isInvisible) {
		sparkMultiplier = 1.6;
		sparkOffset = 4.0;
	}

	Vec sparkSize = (Vec){ 0.15 * sparkMultiplier, 0.15 * sparkMultiplier, 0.15 * sparkMultiplier };

	sparkStart.spawn(
		receiveFrom->sparkEfxName,
		0,
		&(Vec){
			pos.x + (normalizedDirection.x * (96.0 + sparkOffset) * (sparkMultiplier)),
			pos.y - (normalizedDirection.y * (96.0 + sparkOffset) * (sparkMultiplier)),
			pos.z + 500
		},
		&(S16Vec){0, 0, this->pinRotation},
		&sparkSize
	);

	sparkEnd.spawn(
		receiveFrom->sparkEfxName,
		0,
		&(Vec){
			pos.x + (normalizedDirection.x * 96.0 * sparkMultiplier) - normalizedDirection.x * dist,
			pos.y - (normalizedDirection.y * 96.0 * sparkMultiplier) + normalizedDirection.y * dist,
			pos.z + 500
		},
		&(S16Vec){0, 0, this->pinRotation},
		&sparkSize
	);

	setSoundDistance(&this->handle, this->pos, 1.0, 1.0, 1000.0);

	lightning.spawn(
		receiveFrom->lightningEfxName,
		0,
		&(Vec){
			pos.x + (normalizedDirection.x * ((signalFromDistance / 4.0))) - (normalizedDirection.y * 2.0),
			pos.y - (normalizedDirection.y * ((signalFromDistance / 4.0))) - (normalizedDirection.x * 2.0),
			pos.z - 500
		},
		&(S16Vec){0, 0, this->pinRotation},
		&(Vec){0.15, ((0.1 / 3.5) / 16.0) * signalFromDistance, 0.1}
	);

	switch (receiveFrom->type)
	{
	case BEEachXAmountOfTime:
	case BEWhenReceivingElectricity:
		timer++;
		if (timer >= receiveFrom->timeActivated) return doStateChange(&daEnBlockElectricity_c::StateID_Wait);
		break;

	case BEWhenEventIDIsTriggered:
		if (this->checkForEventTrigger(receiveFrom->isInverted ? ACTIVATE : DEACTIVATE)) return doStateChange(&daEnBlockElectricity_c::StateID_Wait);
		break;

	case BEConstantState:
	default:
		break;
	}
}

void daEnBlockElectricity_c::endState_Electricity() {
	if (this->handle.Exists()) this->handle.Stop(0);
	rotatingAPhysics.removeFromList();
}
