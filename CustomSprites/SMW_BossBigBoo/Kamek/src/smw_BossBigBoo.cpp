#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include <profile.h>
#include "boss.h"

class daSMWBBB_c : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c hideModel;
	m3d::mdl_c fogModel;

	nw4r::g3d::ResFile resFile;
	m3d::anmChr_c anmFog;
	m3d::anmChr_c anmA;
	m3d::anmChr_c anmB;

	nw4r::g3d::ResAnmTexSrt resTexSrt;
	m3d::anmTexSrt_c fogSrt;

	ActivePhysics::Info HitMeBaby;

	int timer;
	int timerY;
	int timerX;
	char Hiding;
	char dying;
	Vec2 BasePos;
	bool isMoving;
	int patternY;
	enum enumDir{
		RIGHT = 0,
		LEFT = 1,
	};
	int damage;
	bool realTurn();
	u8 triggerID_ON;
	u8 triggerID_OFF;
	u8 triggerID_DEFEAT;
	int style;
	bool exitType;
	bool isStateHide;
	bool isJustFinishedIntro;

	void activateEvent(int eventID);
	void inactivateEvent(int eventID);
	bool isEventTriggered(int eventID);

	void changeStateLight(int turnTo);

	void bindAnimChr_and_setUpdateRates(const char* name, m3d::anmChr_c &animationChr, m3d::mdl_c &model, float rate);
	void setupModels();
	void updateModelMatrices();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	public: static dActor_c *build();

	USING_STATES(daSMWBBB_c);
	DECLARE_STATE(Intro);
	DECLARE_STATE(Outro);
	DECLARE_STATE(Appear);
	DECLARE_STATE(Hide);
	DECLARE_STATE(Flee);
};

// State and Externs
CREATE_STATE(daSMWBBB_c, Intro);
CREATE_STATE(daSMWBBB_c, Outro);
CREATE_STATE(daSMWBBB_c, Appear);
CREATE_STATE(daSMWBBB_c, Hide);
CREATE_STATE(daSMWBBB_c, Flee);

extern "C" uint m_goalType;


dActor_c *daSMWBBB_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daSMWBBB_c));
	return new(buffer) daSMWBBB_c;
}

// More Sprites
const SpriteData smwBBBData = {ProfileId::SMW_BOSSBIGBOO, 0, 0, 0, 0, 0x1000, 0x1000, 0, 0, 0, 0, 2}; 
Profile smwBBBProfile(&daSMWBBB_c::build, SpriteId::SMW_BOSSBIGBOO, &smwBBBData, ProfileId::SMW_BOSSBIGBOO, ProfileId::SMW_BOSSBIGBOO, "SMW_BOSSBIGBOO", NULL, 0);

// collision
void daSMWBBB_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {}
bool daSMWBBB_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { return false; }
bool daSMWBBB_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {	return false; }
bool daSMWBBB_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) { doStateChange(&StateID_Outro); return false; }
bool daSMWBBB_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { 
	if (apOther->owner->name == 412) { // Check if it's a glow block
		dEn_c *blah = (dEn_c*)apOther->owner;
		if (blah->_12C & 3 || strcmp(blah->acState.getCurrentState()->getName(), "daLightBlock_c::StateID_Throw")) {
			return true;
		}

		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_en_obakedoor_sm", 0, &apOther->owner->pos, &nullRot, &oneVec);
		SpawnEffect("Wm_mr_yoshistep", 0, &apOther->owner->pos, &nullRot, &oneVec);

		apOther->owner->Delete(1);
			
		this->damage += 1;
		if(this->damage >= 3){ doStateChange(&StateID_Outro); }
		else{ doStateChange(&StateID_Flee); }

		return true;
	}

	return false;
}
bool daSMWBBB_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat1_Fireball_E_Explosion(apThis, apOther);
}
bool daSMWBBB_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {	return true; }
bool daSMWBBB_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }

// Model things
void daSMWBBB_c::bindAnimChr_and_setUpdateRates(const char* name, m3d::anmChr_c &animationChr, m3d::mdl_c &model, float rate) {
	nw4r::g3d::ResAnmChr anmChr = resFile.GetResAnmChr(name);
	animationChr.bind(&model, anmChr, 1);
	model.bindAnim(&animationChr, 0.0);
	animationChr.setUpdateRate(rate);
}
void daSMWBBB_c::setupModels() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("teresa", "g3d/teresa.brres");

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("fog");
	this->fogModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->fogModel, 0);

	bool retf;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("fog");
	retf = this->anmFog.setup(mdl, anmChr, &this->allocator, 0);


	nw4r::g3d::ResMdl mdlTeresaA = this->resFile.GetResMdl("teresaA");
	this->bodyModel.setup(mdlTeresaA, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->bodyModel, 0);

	bool retA;
	nw4r::g3d::ResAnmChr anmChrShayA = this->resFile.GetResAnmChr("shay_teresaA");
	retA = this->anmA.setup(mdlTeresaA, anmChrShayA, &this->allocator, 0);

	bool retAA;
	nw4r::g3d::ResAnmChr anmChrCartain = this->resFile.GetResAnmChr("move_cartain");
	retAA = this->anmA.setup(mdlTeresaA, anmChrCartain, &this->allocator, 0);

	bool retAAA;
	nw4r::g3d::ResAnmChr anmChrWaitA1 = this->resFile.GetResAnmChr("wait_otherA1");
	retAAA = this->anmA.setup(mdlTeresaA, anmChrWaitA1, &this->allocator, 0);

	bool retAAAA;
	nw4r::g3d::ResAnmChr anmChrWaitA2 = this->resFile.GetResAnmChr("wait_otherA2");
	retAAAA = this->anmA.setup(mdlTeresaA, anmChrWaitA2, &this->allocator, 0);


	nw4r::g3d::ResMdl mdlTeresaB = this->resFile.GetResMdl("teresaB");
	this->hideModel.setup(mdlTeresaB, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->hideModel, 0);

	bool retB;
	nw4r::g3d::ResAnmChr anmChrShayB = this->resFile.GetResAnmChr("shay_teresaB");
	retB = this->anmB.setup(mdlTeresaB, anmChrShayB, &this->allocator, 0);

	bool retBB;
	nw4r::g3d::ResAnmChr anmChrShayBwait = this->resFile.GetResAnmChr("shay_teresaB_wait");
	retBB = this->anmB.setup(mdlTeresaB, anmChrShayBwait, &this->allocator, 0);


	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("fog");
	this->resTexSrt = anmSrt;
	//setup(ResMdl mdl, ResAnmTexSrt anmSrt, mAllocator* allocator, void* NULL, int count);
	retf = this->fogSrt.setup(mdl, anmSrt, &this->allocator, 0, 1);
	//setEntryByte34(char toSet, int which);
	this->fogSrt.setEntryByte34(0, 0);

	allocator.unlink();
}

/* --------------------------- どうしてココアちゃんがまんげなの？ --------------------------- */

int daSMWBBB_c::onCreate() {
	setupModels();

	this->aPhysics.collisionCheckType = 1;

	HitMeBaby.xDistToCenter = 10.0;
	HitMeBaby.yDistToCenter = 5.0;

	HitMeBaby.xDistToEdge = 45.0;
	HitMeBaby.yDistToEdge = 45.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x80222;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->scale = (Vec){4.5, 4.5, 4.5};
	this->rot.x = 0;
	this->rot.y = 0xD800; // Heading Left
	this->rot.z = 0;
	this->speed.x = 0.0;
	this->pos.z = 0.0;
	this->direction = LEFT; // Heading left.

	this->BasePos = (Vec2){this->pos.x, this->pos.y};
	this->Hiding = 1;
	this->dying = 0;
	this->timerY = 0;
	this->timerX = 0;
	this->patternY = 0;
	this->isMoving = false;
	this->damage = 0;
	this->isStateHide = false;
	this->isJustFinishedIntro = false;

	// Reggie!s
	this->triggerID_ON = ((this->settings >> 4 & 0xFF) << 4) | (this->settings >> 0 & 0xFF);
	this->triggerID_OFF = ((this->settings >> 12 & 0xFF) << 4) | (this->settings >> 8 & 0xFF);
	this->triggerID_DEFEAT = ((this->settings >> 20 & 0xFF) << 4) | (this->settings >> 16 & 0xFF);
	this->style = this->settings >> 24 & 0xF;	// 0000 0"0"00 0000 0000	// nybble 6
	int nybble8 = this->settings >> 28 & 0xF;	// 0000 000"0" 0000 0000	// nybble 5 (for bools)
	//this->[empty] = nybble8 & 0b1000;
	//this->[empty] = nybble8 & 0b100;
	//this->[empty] = nybble8 & 0b10;
	this->exitType = nybble8 & 0b1;	// 0000 000"+1" 0000 0000
	OSReport("triggerIDs: ON->%d, OFF->%d, DEFEAT->%d\nLocal variables: Style->%d, Exit->%d\n", this->triggerID_ON, this->triggerID_OFF, this->triggerID_DEFEAT, this->style, this->exitType);

	activateEvent(triggerID_ON);

	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("fog");
	this->anmFog.bind(&this->fogModel, anmChr, 1);
	this->fogModel.bindAnim(&this->anmFog, 0.0);
	this->anmFog.setUpdateRate(1.0);

	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("fog");
	this->fogSrt.bindEntry(&this->fogModel, anmSrt, 0, 1);
	this->fogModel.bindAnim(&this->fogSrt, 1.0);
	this->fogSrt.setFrameForEntry(1.0, 0);
	this->fogSrt.setUpdateRateForEntry(1.0, 0);

	doStateChange(&StateID_Intro);

	this->onExecute();
	return true;
}

int daSMWBBB_c::onDelete() {
	return true;
}

int daSMWBBB_c::onExecute() {
	acState.execute();

	// Model
	updateModelMatrices();

	this->fogModel._vf1C();

	if(this->anmFog.isAnimationDone())
		this->anmFog.setCurrentFrame(0.0);

	this->fogSrt.process();
	if(this->fogSrt.isEntryAnimationDone(0))
		this->fogSrt.setFrameForEntry(1.0, 0);

	// rotate
	if(isMoving){
		dStageActor_c *Player = GetSpecificPlayerActor(0);
		if(this->pos.x < Player->pos.x){	// this:on right, player:on left
			this->direction = RIGHT;
			this->rot.y = 0x2800;
		}
		else{
			this->direction = LEFT;
			this->rot.y = 0xD800;
		}
	}

	// Move
	if(isMoving){
		// position.x
		// 画面中央を軸に単振動
		this->pos.x = ((this->BasePos.x) + cos(this->timerX * 3.14 / 192) * 100) - 100;
		// position.y
		// sinとcosを行き来してそう
		if(this->patternY == 0){
			if(this->timerY < 192){
				this->pos.y = (this->BasePos.y + cos(this->timerY * 3.14 / 192) * 36) - 36;
			}
			else{
				this->pos.y = (this->BasePos.y + sin(this->timerY * 3.14 / 192) * -72) - 72;
				if(this->timerY == 384){ this->patternY += 1; }
			}
		}
		else if(this->patternY == 1){
			if(this->timerY < 192){
				this->pos.y = (this->BasePos.y + cos(this->timerY * 3.14 / 192) * -36) - 36;
			}
			else{
				this->pos.y = (this->BasePos.y + sin(this->timerY * 3.14 / 192) * 72);
				if(this->timerY == 384){ this->patternY -= 1; }
			}
		}

		// timer
		if (this->timerX >= 384) { this->timerX = 0; }
		if (this->timerY >= 384) { this->timerY = 0; }
		this->timerX += 1;
		this->timerY += 1;
		// OSReport("now timerY: %d\n", this->timerY);
	}

	return true;
}

int daSMWBBB_c::onDraw() {
	fogModel.scheduleForDrawing();

	if (this->Hiding == 0) {
		bodyModel.scheduleForDrawing(); }
	else {
		hideModel.scheduleForDrawing(); }
	return true;
}

bool daSMWBBB_c::realTurn(){
	// this->rot.y: 	L0xD800 : R0x2800;
	// this->playerDir: Right is 0, odd is 1.
	if(this->direction == LEFT && this->rot.y != 0x2800){
		this->rot.y += 0x100;
	}
	if(this->direction == RIGHT && this->rot.y != 0xD800){
		this->rot.y -= 0x100;
	}
}

void daSMWBBB_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x + 10, pos.y, pos.z);	// Fog
	Vec scaleFog = (Vec){3.7, 3.7, 3.7};

	fogModel.setDrawMatrix(matrix);
	fogModel.setScale(&scaleFog);
	fogModel.calcWorld(false);

	matrix.translation(pos.x + 10, pos.y - 40, pos.z + 200.0);	// Boo Main
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	if (this->Hiding == 0) {
		bodyModel.setDrawMatrix(matrix);
		bodyModel.setScale(&scale);
		bodyModel.calcWorld(false); }
	else {
		hideModel.setDrawMatrix(matrix);
		hideModel.setScale(&scale);
		hideModel.calcWorld(false); }
}

// Triggers
bool daSMWBBB_c::isEventTriggered(int eventID) {
	u64 eventFlag = ((u64)1 << (eventID - 1));
	if(dFlagMgr_c::instance->flags & eventFlag) {
		return true;
	}
	return false;
}
void daSMWBBB_c::activateEvent(int eventID) {
	dFlagMgr_c::instance->flags |= ((u64)1 << (eventID - 1));
}
void daSMWBBB_c::inactivateEvent(int eventID) {
    dFlagMgr_c::instance->flags &= ~((u64)1 << (eventID - 1));
}

// Light
void daSMWBBB_c::changeStateLight(int turnTo){
	int timer4Light = 0;
	if(turnTo == 1){	// turn ON
		if(timer4Light == 3){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(timer4Light == 6){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
		else if(timer4Light == 9){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(timer4Light == 18){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
		return;
	}
	else{				// turn OFF
		if(timer4Light == 3){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
		else if(timer4Light == 6){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(timer4Light == 9){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
		else if(timer4Light == 18){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		return;
	}
}

/* --------------------------- ドナルドは、嬉しくなるとついやっちゃうんだ！ --------------------------- */

// Intro State
void daSMWBBB_c::beginState_Intro() {
	this->timer = 0;
	bindAnimChr_and_setUpdateRates("begin_boss_b", anmB, hideModel, 1.0f);
	PlaySound(this, SE_EMY_TERESA);
	MakeMarioEnterDemoMode();

	inactivateEvent(triggerID_ON);
	activateEvent(triggerID_OFF);	// turn OFF light
}
void daSMWBBB_c::executeState_Intro() {
	this->timer += 1;

	if (this->timer == 30) {
		Hiding = false;
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_CS_TERESA_BRING_IT, 1);
		bindAnimChr_and_setUpdateRates("begin_boss", anmA, bodyModel, 1.0f);
	}
	if (this->timer >= 30){
		if(this->timer - 30 == 3){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(this->timer - 30 == 6){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
		else if(this->timer - 30 == 10){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(this->timer - 30 == 20){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
	}

	if (Hiding)
		hideModel._vf1C();
	else
		bodyModel._vf1C();
	if (anmA.isAnimationDone()) {
		Hiding = 0;
		doStateChange(&StateID_Appear);
	}
}
void daSMWBBB_c::endState_Intro() {
	this->BasePos.y = this->pos.y;
	MakeMarioExitDemoMode();
	this->isMoving = true;
	StartBGMMusic();

	// mess to spawn children and create effects for them
	Vec spawnPosChildren = {1.0f, 1.0f, 1.0f};	// size
	S16Vec nullRot = {0,0,0};			// rotation
	Vec oneVec = {1.0f, 1.0f, 1.0f};	// size
	dStageActor_c *spawner;
	
	// I cannot use while
	spawnPosChildren = (Vec){(this->pos.x + -200.0f), (this->pos.y + 25.0f), 3300.0};
	spawner = CreateActor(EN_TERESA, 0, spawnPosChildren, 0, 0);
	SpawnEffect("Wm_en_obakedoor_sm", 0, &spawnPosChildren, &nullRot, &oneVec);

	spawnPosChildren = (Vec){(this->pos.x + -75.0f), (this->pos.y + -25.0f), 3300.0};
	spawner = CreateActor(EN_TERESA, 0, spawnPosChildren, 0, 0);
	SpawnEffect("Wm_en_obakedoor_sm", 0, &spawnPosChildren, &nullRot, &oneVec);

	// prepare first time turn ON light in Appear state
	this->isJustFinishedIntro = true;
}

// Appear State
void daSMWBBB_c::beginState_Appear() {
	this->speed.y = 0;
	this->speed.z = 0;
	this->timer = 0;
	this->isMoving = true;

	if(this->damage < 2){ bindAnimChr_and_setUpdateRates("wait_otherA1", anmA, bodyModel, 1.0f); }
	else if(this->damage == 2){ bindAnimChr_and_setUpdateRates("wait_otherA2", anmA, bodyModel, 1.0f); }
}
void daSMWBBB_c::executeState_Appear() {
	this->bodyModel._vf1C();

	if (this->anmA.isAnimationDone()) {
		this->anmA.setCurrentFrame(0.0); }

	if (this->timer >= 32) { this->timer = 31; }

	this->timer += 1;
}
void daSMWBBB_c::endState_Appear() { }

// Flee State
void daSMWBBB_c::beginState_Flee() {
	this->timer = 0;
	this->isMoving = false;

	this->aPhysics.removeFromList();	// remove HitMeBaby collision

	PlaySound(this, SE_EMY_TERESA_STOP);	// ﾋｭﾝってやつ
}
void daSMWBBB_c::executeState_Flee() {
	if(this->timer >= 3){
		this->Hiding = 1;
		this->hideModel._vf1C();
		if(this->timer == 3){
			bindAnimChr_and_setUpdateRates("shay_teresaB", anmB, hideModel, 1.0f);
		}
	}
	else if(this->timer >= 0){
		this->Hiding = 0;
		this->bodyModel._vf1C();
		if(this->timer == 0){
			bindAnimChr_and_setUpdateRates("shay_teresaA", anmA, bodyModel, 1.0f);
		}
	}

	if (this->timer >= 50){
		if(this->timer - 50 == 3){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(this->timer - 50 == 6){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
		else if(this->timer - 50 == 10){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(this->timer - 50 == 20){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
	}

	if ((this->timer > 60) && (this->anmB.isAnimationDone())) {
		doStateChange(&StateID_Hide);
	}

	this->timer += 1;
}
void daSMWBBB_c::endState_Flee() { }

// Hide State
void daSMWBBB_c::beginState_Hide() {
	inactivateEvent(triggerID_ON);
	activateEvent(triggerID_OFF);	// turn OFF light
	this->timer = 0;
	this->isMoving = true;
	this->Hiding = 1;
	this->isStateHide = true;
}
void daSMWBBB_c::executeState_Hide() {
	if (this->timer >= 300) {
		this->Hiding = 0;
		this->bodyModel._vf1C(); 	// _vf1Cはループ関数に入れるかループさせるようにしないとアニメーションがうまく行かない。恐らく対象のモデルをアクティブ化する関数だと思う。
		
		if(this->timer - 300 == 3){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(this->timer - 300 == 6){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}
		else if(this->timer - 300 == 10){
			inactivateEvent(triggerID_ON);
			activateEvent(triggerID_OFF);	// turn OFF light
		}
		else if(this->timer - 300 == 20){
			inactivateEvent(triggerID_OFF);
			activateEvent(triggerID_ON);	// turn ON light
		}

		if(this->timer == 300){
			this->isMoving = false;
			PlaySound(this, SE_EMY_CS_TERESA_BEAT_YOU);
			bindAnimChr_and_setUpdateRates("move_cartain", anmA, bodyModel, 1.0f);	// issue
		}
	}
	else if (this->timer >= 0){
		this->Hiding = 1;
		this->hideModel._vf1C();

		if(this->timer == 1 || this->timer % 35 == 0){
			bindAnimChr_and_setUpdateRates("shay_teresaB_wait", anmB, hideModel, 1.0f);	// 35Flames
		}
	}

	if(this->anmA.isAnimationDone()){
		doStateChange(&StateID_Appear);
	}

	this->timer += 1;
}
void daSMWBBB_c::endState_Hide() {
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();			// Add HitMeBaby collision
	OSReport("Collision was restored!\n");
	this->isStateHide = false;
	
	// mess for to spawn children and create effects for them
	int randint = GenerateRandomNumber(2);
	Vec spawnPosChildren = {(this->pos.x + ((randint = 0) ? 50 : -50)), (this->pos.y + 25.0f), 3300.0};
	dStageActor_c *spawner = CreateActor(EN_TERESA, 0, spawnPosChildren, 0, 0);

	S16Vec nullRot = {0,0,0};			// rotation
	Vec oneVec = {1.0f, 1.0f, 1.0f};	// size
	SpawnEffect("Wm_en_obakedoor_sm", 0, &spawnPosChildren, &nullRot, &oneVec);
}

// Outro state
void daSMWBBB_c::beginState_Outro() {
	StopBGMMusic();
	bindAnimChr_and_setUpdateRates("DEATH", anmB, hideModel, 1.0f);
	Hiding = 1;

	this->timer = 0;
	this->isMoving = false;

	this->rot.x = 0x0; // X is vertical axis
	this->rot.y = 0xE000; // Y is horizontal axis
	this->rot.z = 0x0; // Z is ... an axis >.>

	inactivateEvent(triggerID_OFF);
	activateEvent(triggerID_ON);	// turn ON light
	activateEvent(triggerID_DEFEAT);

	// Kill children
	dActor_c *iter = 0;
	while (iter = (dActor_c*)dActor_c::searchByBaseType(2, iter)) {
		dStageActor_c *sa = (dStageActor_c*)iter;

		if (sa->name == EN_TERESA) {
			sa->killedByLevelClear();
			sa->Delete(1);	// FIX ME
		}
	}

	OutroSetup(this);
}
void daSMWBBB_c::executeState_Outro() {
	hideModel._vf1C();
	if (this->anmB.isAnimationDone())
		this->anmB.setCurrentFrame(0.0);

	if (this->dying == 1) {
		if (this->timer > 200) {
			if (this->exitType){
				m_goalType = 1;	// ゴールの種類を保存するグローバル変数を強制的に隠しゴール(1)に固定
			}
			ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
		}

		if (this->timer == 60) {
			PlayerVictoryCries(this);
		}

		this->timer = timer + 1;
		return;
	}

	bool ret;
	Vec tempPos = (Vec){this->pos.x + 10.0f, this->pos.y - 40.0f, 5500.0f};
	ret = ShrinkBoss(this, &tempPos, 0.0, this->timer);

	if (timer >= 118) {
		BossExplode(this, &tempPos);
		this->dying = 1;
		this->timer = 0;
	}
	else if (timer == 20) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_BIG_TERESA_DEAD, 1);
	}

	timer = timer + 1;
}
void daSMWBBB_c::endState_Outro() { }


