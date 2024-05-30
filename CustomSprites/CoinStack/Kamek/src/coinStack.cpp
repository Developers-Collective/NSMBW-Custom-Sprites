#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>


const char* CoinStackArcNameList [] = {
	"coin_stack",
	NULL
};

class daCoinStack_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c model;
	m3d::anmChr_c animationChr;

	mEf::es2 effect;

	u32 cmgr_returnValue;
	bool isBouncing;
	bool stillFalling;

	daCoinStack_c *coinStackBelow;
	bool isCollected;
	u8 timer;

	nw4r::snd::SoundHandle handle;

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	void calculateTileCollisions();

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

	void kill();
	void addScoreWhenHit(void *other) { };
	void powBlockActivated(bool isNotMPGP);

	void explodeIntoCoins();

	USING_STATES(daCoinStack_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Fall);
	DECLARE_STATE(Land);
	DECLARE_STATE(Collect);

	public: static dActor_c *build();
};

dActor_c *daCoinStack_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daCoinStack_c));
	return new(buffer) daCoinStack_c;
}

const SpriteData CoinStackSpriteData = { ProfileId::CoinStack, 8, -16, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile CoinStackProfile(&daCoinStack_c::build, SpriteId::CoinStack, &CoinStackSpriteData, ProfileId::CoinStack, ProfileId::CoinStack, "CoinStack", CoinStackArcNameList, 0);

///////////////////////
// Externs and States
///////////////////////
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);
extern "C" void daPyMng_c_incCoin(int unk); // 0x80060250


CREATE_STATE(daCoinStack_c, Wait);
CREATE_STATE(daCoinStack_c, Fall);
CREATE_STATE(daCoinStack_c, Land);
CREATE_STATE(daCoinStack_c, Collect);

////////////////////////
// Collision Functions
////////////////////////
void daCoinStack_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apOther->owner->profileId == ProfileId::CoinStack) {
		if (this->pos.y > apOther->owner->pos.y) {
			coinStackBelow = (daCoinStack_c*)apOther->owner;
			this->speed.y = 0.0f;
			this->pos.x = apOther->owner->pos.x;
			this->pos.y = apOther->owner->pos.y + 15.0f;
		}
	}
}

void daCoinStack_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (this->isCollected) return;
	doStateChange(&StateID_Collect);
}

void daCoinStack_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
}

bool daCoinStack_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return false;
}
bool daCoinStack_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daCoinStack_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daCoinStack_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daCoinStack_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daCoinStack_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daCoinStack_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daCoinStack_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}

bool daCoinStack_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}

bool daCoinStack_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

bool daCoinStack_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

void daCoinStack_c::kill() { }

void daCoinStack_c::powBlockActivated(bool isNotMPGP) {
	if (isNotMPGP) this->explodeIntoCoins();
}


void daCoinStack_c::explodeIntoCoins() {
	for (int i = 0; i < 5; i++) {
		Vec2 speed = (Vec2){
			RandFloat(-2.0, 2.0),
			RandFloat(2.0, 4.0)
		};

		dStageActor_c* coin = dStageActor_c::create(EN_COIN_JUGEM, 0, &this->pos, 0, currentLayerID);
		coin->speed.x = speed.x;
		coin->speed.y = speed.y;
	}

	this->Delete(1);
}


void daCoinStack_c::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.
	if (this->coinStackBelow) {
		this->speed.y = 0.0f;
		return; // Don't do anything if there's a coin stack below
	}

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

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

		max_speed.x = 0.0;
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
}

void daCoinStack_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

int daCoinStack_c::onCreate() {
	this->stillFalling = 0;
	this->coinStackBelow = 0;
	this->isCollected = false;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("coin_stack", "g3d/coin_stack.brres");

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("coin_stack");
	this->model.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&this->model, 0);

	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();

	ActivePhysics::Info aPhysicsInfo;
	aPhysicsInfo.xDistToCenter = 0.0;
	aPhysicsInfo.yDistToCenter = 8.0;
	aPhysicsInfo.xDistToEdge = 7.5;
	aPhysicsInfo.yDistToEdge = 8.0;
	aPhysicsInfo.category1 = 0x3;
	aPhysicsInfo.category2 = 0x16;
	aPhysicsInfo.bitfield1 = 0x2D;
	aPhysicsInfo.bitfield2 = 0x0;
	aPhysicsInfo.unkShort1C = 0x0;
	aPhysicsInfo.callback = &dEn_c::collisionCallback;
	this->aPhysics.initWithStruct(this, &aPhysicsInfo);
	this->aPhysics.addToList();

	// Stuff I do understand

	this->scale = (Vec){1.0, 1.0, 1.0};

	this->rot.x = 0; // X is vertical axis
	this->rot.y = ((this->settings >> 29) & 0x1) == 1 ? (0x4000 * ((this->settings >> 30) & 0x3)) : (0x4000 * MakeRandomNumber(4)); // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.

	this->pos.z = 1200.0f;
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 0.0;
	this->x_speed_inc = 0.0;


	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(-5<<12, 5<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
	doStateChange(&StateID_Wait);

	this->onExecute();
	return true;
}

int daCoinStack_c::onDelete() {
	return true;
}

int daCoinStack_c::onExecute() {
	acState.execute();
	if (!this->isCollected) {
		updateModelMatrices();
		model._vf1C();
	}

	this->coinStackBelow = 0;

	return true;
}

int daCoinStack_c::onDraw() {
	if (!this->isCollected) model.scheduleForDrawing();
	return true;
}

void daCoinStack_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);
}


///////////////
// Wait State
///////////////
void daCoinStack_c::beginState_Wait() {
	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
}
void daCoinStack_c::executeState_Wait() {
	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	calculateTileCollisions();
	if (!(this->collMgr.isOnTopOfTile() || this->coinStackBelow)) {
		return doStateChange(&StateID_Fall);
	}
}
void daCoinStack_c::endState_Wait() { }


///////////////
// Fall State
///////////////
void daCoinStack_c::beginState_Fall() {
	bindAnimChr_and_setUpdateRate("fall", 1, 0.0, 1.0);
}
void daCoinStack_c::executeState_Fall() {
	if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	calculateTileCollisions();
	if ((this->collMgr.isOnTopOfTile() || this->coinStackBelow)) {
		return doStateChange(&StateID_Land);
	}
}
void daCoinStack_c::endState_Fall() { }


///////////////
// Land State
///////////////
void daCoinStack_c::beginState_Land() {
	bindAnimChr_and_setUpdateRate("land", 1, 0.0, 1.0);
}
void daCoinStack_c::executeState_Land() {
	if (this->animationChr.isAnimationDone()) doStateChange(&StateID_Wait);
}
void daCoinStack_c::endState_Land() { }


////////////////
// Collect State
////////////////
void daCoinStack_c::beginState_Collect() {
	this->aPhysics.removeFromList();
	this->isCollected = true;
	this->timer = 0;

	for (int i = 0; i < 5; i++) {
		Vec randPos = (Vec){
			pos.x + (MakeRandomNumber(16) - 8.0f),
			pos.y + (MakeRandomNumber(16) - 8.0f),
			pos.z
		};
		SpawnEffect("Wm_ob_coinkira", 0, &randPos, &this->rot, &this->scale);
		daPyMng_c_incCoin(1);
	}
}
void daCoinStack_c::executeState_Collect() {
	this->timer++;
	if (this->timer % 5 == 1) PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_GET_COIN, 1); // Coin sound
	if (this->timer > 25) this->Delete(1);
}
void daCoinStack_c::endState_Collect() { }
