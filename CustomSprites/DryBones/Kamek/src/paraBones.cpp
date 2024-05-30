#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>



const char* ParaBonesArcNameList [] = {
	"karon_wing",
	NULL
};


class daParaBones_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	void kill();
	void addScoreWhenHit(void *other) { };

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c model;

	m3d::anmChr_c animationChr;

	mEf::es2 effect;

	u32 cmgr_returnValue;
	bool isBouncing;
	bool stillFalling;
	u16 timer;
	u8 size;
	Vec2 basePos;

	lineSensor_s below;
	pointSensor_s above;
	lineSensor_s adjacent;

	dAcPy_c *target;
	bool movesVertical, startsDownLeft;
	float movingDistance;

	nw4r::snd::SoundHandle handle;

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	// bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();

	float sizeEffectMultiplier() { return 1.0 - ((float)this->size * 0.15); }
	bool searchForTarget();

	USING_STATES(daParaBones_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(FlyPattern);
	DECLARE_STATE(ReturnToBase);
	DECLARE_STATE(FoundPlayer);
	DECLARE_STATE(ChasePlayer);
	DECLARE_STATE(AttackHit);
	DECLARE_STATE(Turn);
	DECLARE_STATE(BreakDown);
	DECLARE_STATE(BreakDownFall);
	DECLARE_STATE(BreakGroundHit);
	DECLARE_STATE(Down);
	DECLARE_STATE(DownHit);
	DECLARE_STATE(RecoverSign);
	DECLARE_STATE(Recover);

	public: static dActor_c *build();
};

dActor_c *daParaBones_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daParaBones_c));
	return new(buffer) daParaBones_c;
}

const SpriteData ParaBonesSpriteData = { ProfileId::ParaBones, 8, -12, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile ParaBonesProfile(&daParaBones_c::build, SpriteId::ParaBones, &ParaBonesSpriteData, ProfileId::ParaBones, ProfileId::ParaBones, "ParaBones", ParaBonesArcNameList, 0);

///////////////////////
// Externs and States
///////////////////////
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

//FIXME make this dEn_c->used...
extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
// extern "C" int SomeStrangeModification(dStageActor_c* actor);
// extern "C" void DoStuffAndMarkDead(dStageActor_c *actor, Vec vector, float unk);
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);
// extern "C" void addToList(StandOnTopCollider *self);

// extern "C" bool HandlesEdgeTurns(dEn_c* actor);


CREATE_STATE(daParaBones_c, Wait);
CREATE_STATE(daParaBones_c, FlyPattern);
CREATE_STATE(daParaBones_c, ReturnToBase);
CREATE_STATE(daParaBones_c, FoundPlayer);
CREATE_STATE(daParaBones_c, ChasePlayer);
CREATE_STATE(daParaBones_c, AttackHit);
CREATE_STATE(daParaBones_c, Turn);
CREATE_STATE(daParaBones_c, BreakDown);
CREATE_STATE(daParaBones_c, BreakDownFall);
CREATE_STATE(daParaBones_c, BreakGroundHit);
CREATE_STATE(daParaBones_c, Down);
CREATE_STATE(daParaBones_c, DownHit);
CREATE_STATE(daParaBones_c, RecoverSign);
CREATE_STATE(daParaBones_c, Recover);

////////////////////////
// Collision Functions
////////////////////////
void daParaBones_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (this->acState.getCurrentState() == &StateID_Recover) return;

	u16 name = ((dEn_c*)apOther->owner)->profileId;

	if (
		name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
		|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND
		|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
		|| name == EN_JUMPDAI || name == EN_ITEM || name == EN_TERESA || name == EN_BIG_TERESA
	) return;

	if (acState.getCurrentState() == &StateID_Wait) {

		pos.x = ((pos.x - ((dEn_c*)apOther->owner)->pos.x) > 0) ? pos.x + 1.5 : pos.x - 1.5;
		doStateChange(&StateID_Turn);
	}

	dEn_c::spriteCollision(apThis, apOther);
}

void daParaBones_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	if (hitType == 1) {	// regular jump
		doStateChange(&StateID_BreakDown);
	}
	else if (hitType == 3) { // spin jump
		this->_vf268(apOther->owner);
		doStateChange(&StateID_BreakDown);
	}
	else if (hitType == 0 && this->acState.getCurrentState() != &StateID_Recover) { // sides
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
		if (this->acState.getCurrentState() == &StateID_ChasePlayer) this->doStateChange(&StateID_AttackHit);
	}
	else if (hitType == 2) { // mini mario
		bouncePlayer(this, 5.0);
	}
}

void daParaBones_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	if (hitType == 1 || hitType == 2) {	// regular jump or mini mario
		doStateChange(&StateID_BreakDown);
	}
	else if (hitType == 3) { // spin jump
		this->_vf268(apOther->owner);
		doStateChange(&StateID_BreakDown);
	}
	else if (hitType == 0 && this->acState.getCurrentState() != &StateID_Recover) { // sides
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
		if (this->acState.getCurrentState() == &StateID_ChasePlayer) this->doStateChange(&StateID_AttackHit);
	}
}
bool daParaBones_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	doStateChange(&StateID_BreakDown);
	return true;
}
bool daParaBones_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daParaBones_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daParaBones_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daParaBones_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daParaBones_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daParaBones_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daParaBones_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_EMY_DOWN);
	StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &(S16Vec){0, 0, 0}, &(Vec){1.0, 1.0, 1.0});
	dEn_c::_vf14C();
	return true;
}

bool daParaBones_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	bool wut = dEn_c::collisionCat3_StarPower(apThis, apOther);
	return wut;
}

bool daParaBones_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

bool daParaBones_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

// void daParaBones_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	
// 	dEn_C::collisionCat2_IceBall_15_YoshiIce(apThis, apOther);
// }

// These handle the ice crap
void daParaBones_c::_vf148() {
	dEn_c::_vf148();
}
void daParaBones_c::_vf14C() {
	dEn_c::_vf14C();
}

extern "C" void sub_80024C20(void);
extern "C" void __destroy_arr(void*, void(*)(void), int, int);
//extern "C" __destroy_arr(struct DoSomethingCool, void(*)(void), int cnt, int bar);

bool daParaBones_c::CreateIceActors() {
	if (this->handle.Exists()) this->handle.Stop(0);
	float sizef = 1.0 + (this->size * 0.5);
	struct DoSomethingCool my_struct = { 0, (Vec){this->pos.x, this->pos.y + 8, this->pos.z}, {1.5 * sizef, 1.5 * sizef, 1.5 * sizef}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	__destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	this->animationChr.setUpdateRate(0.0f);
	return true;
}

bool daParaBones_c::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta > 0.0f)
		direction = 0;
	else if (xDelta < 0.0f)
		direction = 1;

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

void daParaBones_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

int daParaBones_c::onCreate() {
	this->stillFalling = 0;
	int color = this->settings >> 28 & 0xF;
	this->size = (this->settings >> 26) & 3;
	float sizef = 1.0f + (this->size * 0.5);

	this->movesVertical = (this->settings >> 25) & 1;
	this->startsDownLeft = (this->settings >> 24) & 1;
	this->movingDistance = ((this->settings >> 16) & 0xFF) * 8.0f;

	this->target = 0;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16];
	getSpriteTexResName(resName, color);
	this->resFile.data = getResource("karon_wing", resName);
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("karon_wing");
	model.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&model, 0);


	// Animations start here
	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();

	// Stuff I do understand

	this->scale = (Vec){0.15 * sizef, 0.15 * sizef, 0.15 * sizef};

	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;


	ActivePhysics::Info HitMeBaby;

	// Note: if this gets changed, also change the point where the default
	// values are assigned after de-ballooning
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 12.0 * sizef;
	HitMeBaby.xDistToEdge = 8.0 * sizef;
	HitMeBaby.yDistToEdge = 12.0 * sizef;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;
	HitMeBaby.bitfield2 = 0xffbafffe;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	below = lineSensor_s(-((int)(5 * sizef))<<12, ((int)(5 * sizef))<<12, 0<<12);
	above = pointSensor_s(5<<12, ((int)(12 * sizef))<<12);
	adjacent = lineSensor_s(((int)(6 * sizef))<<12, ((int)(12 * sizef))<<12, ((int)(6 * sizef))<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	isBouncing = true;
	this->basePos = (Vec2){this->pos.x, this->pos.y};

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
	doStateChange(&StateID_Wait);

	this->onExecute();
	return true;
}

int daParaBones_c::onDelete() {
	if (this->handle.Exists()) this->handle.Stop(0);
	return true;
}

int daParaBones_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	model._vf1C();

	return true;
}

int daParaBones_c::onDraw() {
	model.scheduleForDrawing();
	return true;
}

void daParaBones_c::updateModelMatrices() {
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

void daParaBones_c::kill() {
	if (
		this->acState.getCurrentState() == &StateID_Wait ||
		this->acState.getCurrentState() == &StateID_Turn ||
		this->acState.getCurrentState() == &StateID_Recover
	) return doStateChange(&StateID_BreakDown);

	if (
		this->acState.getCurrentState() == &StateID_Down
	) return doStateChange(&StateID_DownHit);
}


bool daParaBones_c::searchForTarget() {
	dAcPy_c *closestPlayer = dAcPy_c::findNearestPlayer((Vec2){this->pos.x, this->pos.y}, 96.0);

	if (closestPlayer) {
		this->target = closestPlayer;
		this->doStateChange(&StateID_FoundPlayer);
		return true;
	}

	return false;
}


///////////////
// Wait State
///////////////
void daParaBones_c::beginState_Wait() {
	this->animationChr.setUpdateRate(1.0);

	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	this->speed.x = 0.0;

	this->timer = 0;
	this->target = 0;
}
void daParaBones_c::executeState_Wait() {
	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	this->timer++;
	if (this->timer > 120) {
		if (this->searchForTarget()) return;

		Vec2 distance = (Vec2){this->basePos.x - this->pos.x, this->basePos.y - this->pos.y};

		if (abs(distance.x) <= 1.0 && abs(distance.y) <= 1.0) this->doStateChange(&StateID_FlyPattern);
		else this->doStateChange(&StateID_ReturnToBase);
	}
}
void daParaBones_c::endState_Wait() { }


///////////////
// FlyPattern State
///////////////
void daParaBones_c::beginState_FlyPattern() {
	this->animationChr.setUpdateRate(1.0);

	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	this->speed.x = 0.0;

	this->target = 0;
	this->timer = 0;
}
void daParaBones_c::executeState_FlyPattern() {
	if (this->searchForTarget()) return;
	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	if (this->timer > 0) {
		this->timer--;
		return;
	}

	if (this->movesVertical) {
		if (this->startsDownLeft) {
			if (this->pos.y > this->basePos.y - this->movingDistance) this->speed.y = -0.5;
			else {
				this->timer = 30;
				this->startsDownLeft = false;
			}
		}
	
		else {
			if (this->pos.y < this->basePos.y + this->movingDistance) this->speed.y = 0.5;
			else {
				this->timer = 30;
				this->startsDownLeft = true;
			}
		}
	}

	else {
		if (this->startsDownLeft) {
			if (this->pos.x > this->basePos.x - this->movingDistance) {
				this->speed.x = -0.5;
				if (this->direction == 0) return this->doStateChange(&StateID_Turn);
			}
			else {
				this->timer = 30;
				this->startsDownLeft = false;
			}
		}
	
		else {
			if (this->pos.x < this->basePos.x + this->movingDistance) {
				this->speed.x = 0.5;
				if (this->direction == 1) return this->doStateChange(&StateID_Turn);
			}
			else {
				this->timer = 30;
				this->startsDownLeft = true;
			}
		}
	}

	calculateTileCollisions();
}
void daParaBones_c::endState_FlyPattern() { }


///////////////
// ReturnToBase State
///////////////
void daParaBones_c::beginState_ReturnToBase() {
	this->animationChr.setUpdateRate(1.0);

	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	this->speed.x = 0.0;

	this->target = 0;
}
void daParaBones_c::executeState_ReturnToBase() {
	if (this->searchForTarget()) return;
	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	Vec2 distance = (Vec2){this->basePos.x - this->pos.x, this->basePos.y - this->pos.y};

	if (distance.x > 1.0) {
		this->speed.x = 0.5;
		if (this->direction == 1) return this->doStateChange(&StateID_Turn);
	} else if (distance.x < -1.0) {
		this->speed.x = -0.5;
		if (this->direction == 0) return this->doStateChange(&StateID_Turn);
	} else {
		this->speed.x = 0.0;
	}

	if (distance.y > 1.0) {
		this->speed.y = 0.5;
	} else if (distance.y < -1.0) {
		this->speed.y = -0.5;
	} else {
		this->speed.y = 0.0;
	}

	calculateTileCollisions();

	if (abs(distance.x) <= 1.0 && abs(distance.y) <= 1.0) this->doStateChange(&StateID_FlyPattern);
}
void daParaBones_c::endState_ReturnToBase() { }


///////////////
// FoundPlayer State
///////////////
void daParaBones_c::beginState_FoundPlayer() {
	bindAnimChr_and_setUpdateRate("find", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_FIND, 1.0, this->sizeEffectMultiplier());

	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_FoundPlayer() {
	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_ChasePlayer);
}
void daParaBones_c::endState_FoundPlayer() { }


///////////////
// ChasePlayer State
///////////////
void daParaBones_c::beginState_ChasePlayer() {
	bindAnimChr_and_setUpdateRate("fly_chase", 1, 0.0, 1.0);
	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_ChasePlayer() {
	if (!this->target) doStateChange(&StateID_ReturnToBase);
	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	Vec2 distance = (Vec2){this->target->pos.x - this->pos.x, this->target->pos.y - this->pos.y};
	float dist = sqrt(distance.x * distance.x + distance.y * distance.y);

	if (dist > 192.0) {
		bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
		this->doStateChange(&StateID_Wait);
		return;
	}

	if (distance.x > 1.0) {
		this->speed.x = 0.5;
		if (this->direction == 1) return this->doStateChange(&StateID_Turn);
	} else if (distance.x < -1.0) {
		this->speed.x = -0.5;
		if (this->direction == 0) return this->doStateChange(&StateID_Turn);
	} else {
		this->speed.x = 0.0;
	}

	if (distance.y > 1.0) {
		this->speed.y = 0.5;
	} else if (distance.y < -1.0) {
		this->speed.y = -0.5;
	} else {
		this->speed.y = 0.0;
	}

	calculateTileCollisions();
}
void daParaBones_c::endState_ChasePlayer() { }


///////////////
// AttackHit State
///////////////
void daParaBones_c::beginState_AttackHit() {
	bindAnimChr_and_setUpdateRate("attack_hit", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_ATTACK_HIT, 1.0, this->sizeEffectMultiplier());

	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_AttackHit() {
	if (this->animationChr.isAnimationDone()) {
		bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
		this->doStateChange(&StateID_Wait);
	}
}
void daParaBones_c::endState_AttackHit() { }


///////////////
// Turn State
///////////////
void daParaBones_c::beginState_Turn() {
	this->direction ^= 1;

	this->speed.y = 0.0;
	this->y_speed_inc = 0.0;
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_Turn() {
	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if(done) {
		if (this->acState.getPreviousState() != &StateID_Turn)
			this->doStateChange(this->acState.getPreviousState());
		else
			this->doStateChange(&StateID_Wait);
	}
}
void daParaBones_c::endState_Turn() {
	this->rot.y = (this->direction == 0) ? 0x2800 : 0xD800;
}


///////////////
// BreakDown State
///////////////
void daParaBones_c::beginState_BreakDown() {
	this->aPhysics.removeFromList();
	bindAnimChr_and_setUpdateRate("break_down", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_BREAK, 1.0, this->sizeEffectMultiplier());

	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->y_speed_inc = -0.1875;
}
void daParaBones_c::executeState_BreakDown() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_BreakDownFall);
}
void daParaBones_c::endState_BreakDown() {
	this->timer = 0;
}


///////////////
// BreakDownFall State
///////////////
void daParaBones_c::beginState_BreakDownFall() {
	bindAnimChr_and_setUpdateRate("break", 1, 0.0, 1.0);
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_BreakDownFall() {
	calculateTileCollisions();

	if (this->collMgr.isOnTopOfTile()) {
		this->doStateChange(&StateID_BreakGroundHit);
	}
}
void daParaBones_c::endState_BreakDownFall() { }


///////////////
// BreakGroundHit State
///////////////
void daParaBones_c::beginState_BreakGroundHit() {
	bindAnimChr_and_setUpdateRate("break_ground_hit", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_DOWN, 1.0, this->sizeEffectMultiplier());
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_BreakGroundHit() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_Down);
}
void daParaBones_c::endState_BreakGroundHit() { }


///////////////
// Down State
///////////////
void daParaBones_c::beginState_Down() {
	bindAnimChr_and_setUpdateRate("down", 1, 0.0, 1.0);
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_Down() {
	calculateTileCollisions();

	this->timer++;
	if (this->timer > 300) this->doStateChange(&StateID_RecoverSign);
}
void daParaBones_c::endState_Down() { }


///////////////
// DownHit State
///////////////
void daParaBones_c::beginState_DownHit() {
	bindAnimChr_and_setUpdateRate("break_reaction", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_BREAK_DOWN, 1.0, this->sizeEffectMultiplier());
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_DownHit() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_Down);
}
void daParaBones_c::endState_DownHit() { }


///////////////
// RecoverSign State
///////////////
void daParaBones_c::beginState_RecoverSign() {
	this->timer = 0;
	bindAnimChr_and_setUpdateRate("recover_sign", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_RECOVER_SIGN, 1.0, this->sizeEffectMultiplier());
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_RecoverSign() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);
	this->timer++;
	if (this->timer > 60) {
		this->doStateChange(&StateID_Recover);
	}
}
void daParaBones_c::endState_RecoverSign() {
	if (this->handle.Exists()) this->handle.Stop(0);
}


///////////////
// Recover State
///////////////
void daParaBones_c::beginState_Recover() {
	bindAnimChr_and_setUpdateRate("recover", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_RECOVER, 1.0, this->sizeEffectMultiplier());
	this->aPhysics.addToList();
	this->speed.x = 0.0;
}
void daParaBones_c::executeState_Recover() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_ReturnToBase);
}
void daParaBones_c::endState_Recover() {
	bindAnimChr_and_setUpdateRate("fly", 1, 0.0, 1.0);
}
