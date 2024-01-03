#include "konbu.h"

dActor_c *daKonbu_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daKonbu_c));
	return new(buffer) daKonbu_c;
}

const SpriteData KonbuSpriteData = { ProfileId::Konbu, 8, -16, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile KonbuProfile(&daKonbu_c::build, SpriteId::Konbu, &KonbuSpriteData, ProfileId::Konbu, ProfileId::Konbu, "Konbu", KombooArcNameList, 0);

///////////////////////
// Externs and States
///////////////////////
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);


CREATE_STATE(daKonbu_c, Appear);
CREATE_STATE(daKonbu_c, Sink);
CREATE_STATE(daKonbu_c, Walk);
CREATE_STATE(daKonbu_c, Turn);

////////////////////////
// Collision Functions
////////////////////////
void daKonbu_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	u16 name = ((dEn_c*)apOther->owner)->profileId;

	if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
		|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND
		|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
		|| name == EN_JUMPDAI || name == EN_ITEM)
		{ return; }

	if (acState.getCurrentState() == &StateID_Walk) {

		pos.x = ((pos.x - ((dEn_c*)apOther->owner)->pos.x) > 0) ? pos.x + 1.5 : pos.x - 1.5;
		// pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
		doStateChange(&StateID_Turn);
	}

	dEn_c::spriteCollision(apThis, apOther);
}

void daKonbu_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
}

void daKonbu_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
}
bool daKonbu_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_EMY_DOWN);
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &(S16Vec){0, 0, 0}, &(Vec){1.0, 1.0, 1.0});
	dEn_c::_vf148();
	return true;
}
bool daKonbu_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return false;
}
bool daKonbu_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat7_GroundPound(apThis, apOther);
}
bool daKonbu_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daKonbu_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat7_GroundPound(apThis, apOther);
}
bool daKonbu_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daKonbu_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daKonbu_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
	return this->collisionCatD_Drill(apThis, apOther);
}

bool daKonbu_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	bool wut = dEn_c::collisionCat3_StarPower(apThis, apOther);
	return wut;
}

bool daKonbu_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

bool daKonbu_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
	return this->collisionCatD_Drill(apThis, apOther);
}

// void daKonbu_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	
// 	dEn_C::collisionCat2_IceBall_15_YoshiIce(apThis, apOther);
// }

// These handle the ice crap
void daKonbu_c::_vf148() {
	dEn_c::_vf148();
}
void daKonbu_c::_vf14C() {
	dEn_c::_vf14C();
}

extern "C" void sub_80024C20(void);
extern "C" void __destroy_arr(void*, void(*)(void), int, int);
extern "C" unsigned char checkWater(float x, float y, u8 layer, float *water_height); // 0x80075274 | 0 = None, 1 = Water, 2 = ???, 3 = Lava, 4 = Poison

bool daKonbu_c::CreateIceActors() {
	struct DoSomethingCool my_struct = { 0, this->pos, {1.25, this->isBig ? 6.5 : 3.5, 1.25}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	__destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	this->animationChr.setUpdateRate(0.0f);
	return true;
}

bool daKonbu_c::calculateTileCollisions() {
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
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

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

void daKonbu_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

int daKonbu_c::onCreate() {
	if(!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	this->stillFalling = 0;
	int color = this->settings >> 28 & 0xF;
	int patColor = this->settings >> 26 & 0x3;
	this->isBig = this->settings >> 25 & 0x1;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16];
	getSpriteTexResName(resName, color);
	char mdlName[16];
	sprintf(mdlName, "komboo%c", this->isBig ? 'M' : 'S');
	mdlName[7] = 0;

	this->resFile.data = getResource(mdlName, resName);
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl(mdlName);
	model.setup(mdl, &allocator, 0x227, 1, 0);
	SetupTextures_Enemy(&model, 0);


	// Animations start here
	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	this->anmPat = this->resFile.GetResAnmTexPat("color");
	this->animationPat.setup(mdl, anmPat, &this->allocator, 0, 1);
	this->animationPat.bindEntry(&this->model, &anmPat, 0, 1);
	this->animationPat.setFrameForEntry(patColor, 0);
	this->animationPat.setUpdateRateForEntry(0.0f, 0);
	this->model.bindAnim(&this->animationPat);

	allocator.unlink();

	// Stuff I do understand

	this->scale = (Vec){0.15, 0.15, 0.15};

	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 0.5;
	this->x_speed_inc = 0.15;
	this->XSpeed = 1.0;


	ActivePhysics::Info HitMeBaby;

	// Note: if this gets changed, also change the point where the default
	// values are assigned after de-ballooning
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = this->isBig ? 58.0 : 28.0;
	HitMeBaby.xDistToEdge = 8.0;
	HitMeBaby.yDistToEdge = this->isBig ? 58.0 : 28.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;
	HitMeBaby.bitfield2 = 0xffbafffe;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);


	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(-5<<12, 5<<12, 0<<12);
	static const lineSensor_s above(-5<<12, 5<<12, ((int)(HitMeBaby.yDistToEdge * 2))<<12);
	static const lineSensor_s adjacent(1<<12, ((int)(HitMeBaby.yDistToEdge * 2) - 1)<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;

	bindAnimChr_and_setUpdateRate("appear", 1, 0.0, 1.0);
	doStateChange(&StateID_Appear);

	this->onExecute();
	return true;
}

int daKonbu_c::onDelete() {
	return true;
}

int daKonbu_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	model._vf1C();

	return true;
}

int daKonbu_c::onDraw() {
	model.scheduleForDrawing();
	return true;
}

void daKonbu_c::updateModelMatrices() {
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


bool daKonbu_c::willWalkOntoSuitableGround() {
	static const float deltas[] = {1.5f, -1.5f};
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

bool daKonbu_c::isInLiquid() {
	u8 newLiquid = checkWater(this->pos.x, this->pos.y, this->currentLayerID, (float*)0x0);
	return newLiquid != 0;
}

///////////////
// Appear State
///////////////
void daKonbu_c::beginState_Appear() {
	this->pos.z = -3000.0;

	this->max_speed.x = 0.0;
	this->speed.x = 0.0;

	this->max_speed.y = 0.0;
	this->speed.y = 0.0;

	this->bindAnimChr_and_setUpdateRate("appear", 1, 0.0, 1.0);
}
void daKonbu_c::executeState_Appear() {
	if (this->animationChr.isAnimationDone()) {
		this->doStateChange(&StateID_Walk);
	}
}
void daKonbu_c::endState_Appear() {
	this->bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);
	this->aPhysics.addToList();
	this->pos.z = 1800.0;
}

////////////////
// Sink State
////////////////
void daKonbu_c::beginState_Sink() {
	this->max_speed.x = 0.0;
	this->speed.x = 0.0;

	this->max_speed.y = 0.0;
	this->speed.y = 0.0;

	this->bindAnimChr_and_setUpdateRate("sink", 1, 0.0, 1.0);

	this->aPhysics.removeFromList();
}
void daKonbu_c::executeState_Sink() {
	if (this->animationChr.isAnimationDone()) {
		this->Delete(1);
	}
}
void daKonbu_c::endState_Sink() { }

///////////////
// Walk State
///////////////
void daKonbu_c::beginState_Walk() {
	this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
	this->speed.x = ((direction) ? -0.5f : 0.5f) * (this->isInLiquid() ? 2.0f : 1.0f);

	this->max_speed.y = -4.0;
	this->speed.y = -4.0;
	this->y_speed_inc = -0.1875;

	this->animationChr.setUpdateRate(1.0);
}
void daKonbu_c::executeState_Walk() {
	if (collMgr.isOnTopOfTile()) {
		stillFalling = false;

		if (!willWalkOntoSuitableGround()) {
			pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
			doStateChange(&StateID_Turn);
		}
	}
	else {
		if (!stillFalling) {
			stillFalling = true;
			pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
			doStateChange(&StateID_Turn);
		}
	}


	bool ret = calculateTileCollisions();
	if (ret) {
		doStateChange(&StateID_Turn);
	}

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}
}
void daKonbu_c::endState_Walk() { }

///////////////
// Turn State
///////////////
void daKonbu_c::beginState_Turn() {

	this->direction ^= 1;
	this->speed.x = 0.0;
}
void daKonbu_c::executeState_Turn() {

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if(done) {
		this->doStateChange(&StateID_Walk);
	}
}
void daKonbu_c::endState_Turn() { }
