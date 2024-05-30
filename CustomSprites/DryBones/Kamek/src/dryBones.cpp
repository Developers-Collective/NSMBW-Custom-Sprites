#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>



const char* DryBonesArcNameList [] = {
	"karon_new",
	NULL
};


class daDryBones_c : public dEn_c {
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

	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	bool stillFalling;
	u16 timer;
	u8 size;

	lineSensor_s below;
	pointSensor_s above;
	lineSensor_s adjacent;

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

	bool willWalkOntoSuitableGround();
	float sizeEffectMultiplier() { return 1.0 - ((float)this->size * 0.15); }

	USING_STATES(daDryBones_c);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(BreakDown);
	DECLARE_STATE(Down);
	DECLARE_STATE(DownHit);
	DECLARE_STATE(RecoverSign);
	DECLARE_STATE(Recover);
	DECLARE_STATE(Shake);

	public: static dActor_c *build();
};

dActor_c *daDryBones_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daDryBones_c));
	return new(buffer) daDryBones_c;
}

const SpriteData DryBonesSpriteData = { ProfileId::DryBones, 8, -12, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile DryBonesProfile(&daDryBones_c::build, SpriteId::DryBones, &DryBonesSpriteData, ProfileId::DryBones, ProfileId::DryBones, "DryBones", DryBonesArcNameList, 0);

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


CREATE_STATE(daDryBones_c, Walk);
CREATE_STATE(daDryBones_c, Turn);
CREATE_STATE(daDryBones_c, BreakDown);
CREATE_STATE(daDryBones_c, Down);
CREATE_STATE(daDryBones_c, DownHit);
CREATE_STATE(daDryBones_c, RecoverSign);
CREATE_STATE(daDryBones_c, Recover);
CREATE_STATE(daDryBones_c, Shake);

////////////////////////
// Collision Functions
////////////////////////
void daDryBones_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (this->acState.getCurrentState() == &StateID_Recover) return;

	u16 name = ((dEn_c*)apOther->owner)->profileId;

	if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
		|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND
		|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
		|| name == EN_JUMPDAI || name == EN_ITEM)
		{ return; }

	if (acState.getCurrentState() == &StateID_Walk) {

		pos.x = ((pos.x - ((dEn_c*)apOther->owner)->pos.x) > 0) ? pos.x + 1.5 : pos.x - 1.5;
		doStateChange(&StateID_Turn);
	}

	dEn_c::spriteCollision(apThis, apOther);
}

void daDryBones_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	if (hitType == 1) {	// regular jump
		if (this->size == 0) doStateChange(&StateID_BreakDown);
		else {
			if (
				this->acState.getCurrentState() == &StateID_Walk ||
				this->acState.getCurrentState() == &StateID_Turn ||
				this->acState.getCurrentState() == &StateID_Shake
			) doStateChange(&StateID_Shake);
			else doStateChange(&StateID_BreakDown);
		}
	}
	else if (hitType == 3) { // spin jump
		this->_vf268(apOther->owner);
		doStateChange(&StateID_BreakDown);
	}
	else if (hitType == 0 && this->acState.getCurrentState() != &StateID_Recover) { // sides
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
	}
	else if (hitType == 2) { // mini mario
		bouncePlayer(this, 5.0);
	}
}

void daDryBones_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	if (hitType == 1 || hitType == 2) {	// regular jump or mini mario
		if (this->size == 0) doStateChange(&StateID_BreakDown);
		else {
			if (
				this->acState.getCurrentState() == &StateID_Walk ||
				this->acState.getCurrentState() == &StateID_Turn ||
				this->acState.getCurrentState() == &StateID_Shake
			) doStateChange(&StateID_Shake);
			else doStateChange(&StateID_BreakDown);
		}
	}
	else if (hitType == 3) { // spin jump
		this->_vf268(apOther->owner);
		doStateChange(&StateID_BreakDown);
	}
	else if (hitType == 0 && this->acState.getCurrentState() != &StateID_Recover) { // sides
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
	}
}
bool daDryBones_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	doStateChange(&StateID_BreakDown);
	return true;
}
bool daDryBones_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daDryBones_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daDryBones_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daDryBones_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daDryBones_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daDryBones_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daDryBones_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_EMY_DOWN);
	StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &(S16Vec){0, 0, 0}, &(Vec){1.0, 1.0, 1.0});
	dEn_c::_vf14C();
	return true;
}

bool daDryBones_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	bool wut = dEn_c::collisionCat3_StarPower(apThis, apOther);
	return wut;
}

bool daDryBones_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

bool daDryBones_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

// void daDryBones_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	
// 	dEn_C::collisionCat2_IceBall_15_YoshiIce(apThis, apOther);
// }

// These handle the ice crap
void daDryBones_c::_vf148() {
	dEn_c::_vf148();
}
void daDryBones_c::_vf14C() {
	dEn_c::_vf14C();
}

extern "C" void sub_80024C20(void);
extern "C" void __destroy_arr(void*, void(*)(void), int, int);
//extern "C" __destroy_arr(struct DoSomethingCool, void(*)(void), int cnt, int bar);

bool daDryBones_c::CreateIceActors() {
	if (this->handle.Exists()) this->handle.Stop(0);
	float sizef = 1.0 + (this->size * 0.5);
	struct DoSomethingCool my_struct = { 0, this->pos, {1.35 * sizef, 1.75 * sizef, 1.35 * sizef}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	__destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	this->animationChr.setUpdateRate(0.0f);
	return true;
}

bool daDryBones_c::calculateTileCollisions() {
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

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		speed.y = 0.0f;

		if (this->acState.getCurrentState() == &StateID_Walk || this->acState.getCurrentState() == &StateID_Turn) 
			max_speed.x = (direction == 1) ? -XSpeed : XSpeed;

	} else {
		x_speed_inc = 0.0f;
	}

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

void daDryBones_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

int daDryBones_c::onCreate() {
	this->stillFalling = 0;
	int color = this->settings >> 28 & 0xF;
	this->size = (this->settings >> 26) & 3;
	float sizef = 1.0 + (this->size * 0.5);

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16];
	getSpriteTexResName(resName, color);
	this->resFile.data = getResource("karon_new", resName);
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("karon");
	model.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&model, 0);


	// Animations start here
	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();

	// Stuff I do understand

	this->scale = (Vec){1.0 * sizef, 1.0 * sizef, 1.0 * sizef};

	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 0.5;
	this->x_speed_inc = 0.15;
	this->XSpeed = 0.5;


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
	below = lineSensor_s(-((int)(6 * sizef))<<12, ((int)(6 * sizef))<<12, 0<<12);
	above = pointSensor_s(6<<12, ((int)(14 * sizef))<<12);
	adjacent = lineSensor_s(((int)(7 * sizef))<<12, ((int)(14 * sizef))<<12, ((int)(7 * sizef))<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	isBouncing = true;

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);
	doStateChange(&StateID_Walk);

	this->onExecute();
	return true;
}

int daDryBones_c::onDelete() {
	if (this->handle.Exists()) this->handle.Stop(0);
	return true;
}

int daDryBones_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	model._vf1C();

	return true;
}

int daDryBones_c::onDraw() {
	model.scheduleForDrawing();
	return true;
}

void daDryBones_c::updateModelMatrices() {
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

void daDryBones_c::kill() {
	if (
		this->acState.getCurrentState() == &StateID_Walk ||
		this->acState.getCurrentState() == &StateID_Turn ||
		this->acState.getCurrentState() == &StateID_Recover
	) return doStateChange(&StateID_BreakDown);

	if (
		this->acState.getCurrentState() == &StateID_Down
	) return doStateChange(&StateID_DownHit);
}


bool daDryBones_c::willWalkOntoSuitableGround() {
	static const float deltas[] = {1.5f * scale.x, -1.5f * scale.x};
	VEC3 checkWhere = {
			pos.x + deltas[direction],
			4.0f + pos.y,
			pos.z
		};

	u32 props = collMgr.getTileBehaviour2At(checkWhere.x, checkWhere.y, currentLayerID);

	//if (getSubType(props) == B_SUB_LEDGE)
	if (((props >> 16) & 0xFF) == 8)
		return false;

	float someFloat = 0.0f;
	if (collMgr.sub_800757B0(&checkWhere, &someFloat, currentLayerID, 1, -1)) {
		if (someFloat < checkWhere.y && someFloat > (pos.y - 5.0f))
			return true;
	}

	return false;
}


///////////////
// Walk State
///////////////
void daDryBones_c::beginState_Walk() {
	this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
	this->speed.x = (this->direction) ? -0.5f : 0.5f;
	this->x_speed_inc = 0.15f;

	this->max_speed.y = -4.0;
	this->speed.y = -4.0;
	this->y_speed_inc = -0.1875;

	this->animationChr.setUpdateRate(1.0);
}
void daDryBones_c::executeState_Walk() {
	if (collMgr.isOnTopOfTile()) {
		stillFalling = false;

		if (!willWalkOntoSuitableGround()) {
			pos.x = this->direction ? pos.x + 1.5 : pos.x - 1.5;
			doStateChange(&StateID_Turn);
		}
	}
	else {
		if (!stillFalling) {
			stillFalling = true;
			pos.x = this->direction ? pos.x + 1.5 : pos.x - 1.5;
			doStateChange(&StateID_Turn);
		}
	}


	bool ret = calculateTileCollisions();
	if (ret) {
		doStateChange(&StateID_Turn);
	}

	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);
}
void daDryBones_c::endState_Walk() { }


///////////////
// Turn State
///////////////
void daDryBones_c::beginState_Turn() {

	this->direction ^= 1;
	this->speed.x = 0.0;
}
void daDryBones_c::executeState_Turn() {
	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if(done) {
		if (this->acState.getPreviousState() != &StateID_Turn)
			this->doStateChange(this->acState.getPreviousState());
		else
			this->doStateChange(&StateID_Walk);
	}
}
void daDryBones_c::endState_Turn() {
	this->rot.y = (this->direction == 0) ? 0x2800 : 0xD800;
}


///////////////
// BreakDown State
///////////////
void daDryBones_c::beginState_BreakDown() {
	this->aPhysics.removeFromList();
	this->speed.x = 0.0;
	this->x_speed_inc = 0.0;
	bindAnimChr_and_setUpdateRate("break_down", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_BREAK_DOWN, 1.0, this->sizeEffectMultiplier());
}
void daDryBones_c::executeState_BreakDown() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_Down);
}
void daDryBones_c::endState_BreakDown() {
	this->timer = 0;
}


///////////////
// Down State
///////////////
void daDryBones_c::beginState_Down() {
	bindAnimChr_and_setUpdateRate("down", 1, 0.0, 1.0);
}
void daDryBones_c::executeState_Down() {
	calculateTileCollisions();

	this->timer++;
	if (this->timer > 300) this->doStateChange(&StateID_RecoverSign);
}
void daDryBones_c::endState_Down() { }


///////////////
// DownHit State
///////////////
void daDryBones_c::beginState_DownHit() {
	bindAnimChr_and_setUpdateRate("break_reaction", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_BREAK_DOWN, 1.0, this->sizeEffectMultiplier());
}
void daDryBones_c::executeState_DownHit() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_Down);
}
void daDryBones_c::endState_DownHit() { }


///////////////
// RecoverSign State
///////////////
void daDryBones_c::beginState_RecoverSign() {
	this->timer = 0;
	bindAnimChr_and_setUpdateRate("recover_sign", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_RECOVER_SIGN, 1.0, this->sizeEffectMultiplier());
}
void daDryBones_c::executeState_RecoverSign() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);
	this->timer++;
	if (this->timer > 60) {
		this->doStateChange(&StateID_Recover);
	}
}
void daDryBones_c::endState_RecoverSign() {
	if (this->handle.Exists()) this->handle.Stop(0);
}


///////////////
// Recover State
///////////////
void daDryBones_c::beginState_Recover() {
	bindAnimChr_and_setUpdateRate("recover", 1, 0.0, this->sizeEffectMultiplier());
	playSoundDistance(&handle, this->pos, SFX_KARON_RECOVER, 1.0, this->sizeEffectMultiplier());
	this->aPhysics.addToList();
}
void daDryBones_c::executeState_Recover() {
	calculateTileCollisions();

	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_Walk);
}
void daDryBones_c::endState_Recover() {
	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);
}


///////////////
// Shake State
///////////////
void daDryBones_c::beginState_Shake() {
	bindAnimChr_and_setUpdateRate("shake", 1, 0.0, this->sizeEffectMultiplier());
	if (this->handle.Exists()) this->handle.Stop(0);
	playSoundDistance(&handle, this->pos, SFX_KARON_RECOVER_SIGN, 1.0, this->sizeEffectMultiplier());
	this->speed.x = 0.0;
	this->x_speed_inc = 0.0;
}
void daDryBones_c::executeState_Shake() {
	calculateTileCollisions();
	if (this->animationChr.isAnimationDone()) this->doStateChange(&StateID_Walk);
}
void daDryBones_c::endState_Shake() {
	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);
	if (this->handle.Exists()) this->handle.Stop(0);
}
