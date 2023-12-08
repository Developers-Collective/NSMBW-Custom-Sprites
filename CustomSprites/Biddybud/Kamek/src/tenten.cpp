#include <common.h>
#include <game.h>
#include "profile.h"
#include <sfx.h>
#include "path.h"

const char* TentenFileList[] = { "tenten", 0 };


class daEnTenten_c : public dEn_c {
public:
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c model;

	nw4r::g3d::ResAnmTexPat anmPat;

	m3d::anmChr_c animationChr;
	m3d::anmTexPat_c animationPat;

	bool facingRight;
	float wSpeed;
	u8 wDistance;
	int edgeMin, edgeMax;
    u16 amtRotate;

	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;

	bool ranOnce;

	int sfxTimer;
	nw4r::snd::SoundHandle handle; // Sound Handle

	static dActor_c* build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	void updateModelMatrices();
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
	bool calculateTileCollisions();
	bool willWalkOntoSuitableGround();

	void playsound(int id);
	void executePlaysound();
    float getValueOfCurve(float ratio, float curveRatioVar, float minValue);

	USING_STATES(daEnTenten_c);
	DECLARE_STATE(Walk);
    DECLARE_STATE(Turn);
	DECLARE_STATE(DieStomp);
};

const SpriteData TentenSpriteData = { ProfileId::Tenten, 8, -8 , 0 , 0, 0x100, 0x100, 0, 0, 0, 0, 0b10000 };
Profile TentenProfile(&daEnTenten_c::build, SpriteId::Tenten, &TentenSpriteData, ProfileId::Tenten, ProfileId::Tenten, "Tenten", TentenFileList);



u8 hijackMusicWithSongName(const char* songName, int themeID, bool hasFast, int channelCount, int trackCount, int* wantRealStreamID);
extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

CREATE_STATE(daEnTenten_c, Walk);
CREATE_STATE(daEnTenten_c, Turn);
CREATE_STATE(daEnTenten_c, DieStomp);

void daEnTenten_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	u16 name = ((dEn_c*)apOther->owner)->name;

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

void daEnTenten_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	if (hitType == 1) {	// regular jump
		this->_vf260(apOther->owner);
		doStateChange(&StateID_DieStomp);
	}
	else if (hitType == 3) { // spin jump
		this->_vf268(apOther->owner);
		doStateChange(&StateID_DieSmoke);
	}
	else if (hitType == 0) { // sides
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
	}
	else if (hitType == 2) { // mini mario
		bouncePlayer(this, 5.0);
	}
}

void daEnTenten_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	if (hitType == 1 || hitType == 2) {	// regular jump or mini mario
		this->_vf278(apOther->owner);
		doStateChange(&StateID_DieSmoke);
	}
	else if (hitType == 3) { // spin jump
		this->_vf268(apOther->owner);
		doStateChange(&StateID_DieSmoke);
	}
	else if (hitType == 0) { // sides
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
	}
}
bool daEnTenten_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_EMY_DOWN);
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &(S16Vec){0, 0, 0}, &(Vec){1.0, 1.0, 1.0});
	dEn_c::_vf148();
	return true;
}
bool daEnTenten_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daEnTenten_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daEnTenten_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daEnTenten_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daEnTenten_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daEnTenten_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daEnTenten_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
	return this->collisionCatD_Drill(apThis, apOther);
}

bool daEnTenten_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	bool wut = dEn_c::collisionCat3_StarPower(apThis, apOther);
	return wut;
}

bool daEnTenten_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

bool daEnTenten_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
	return this->collisionCatD_Drill(apThis, apOther);
}

// void daEnTenten_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	
// 	dEn_C::collisionCat2_IceBall_15_YoshiIce(apThis, apOther);
// }


// These handle the ice crap
void daEnTenten_c::_vf148() {
	dEn_c::_vf148();
}
void daEnTenten_c::_vf14C() {
	dEn_c::_vf14C();
}

extern "C" void sub_80024C20(void);
extern "C" void __destroy_arr(void*, void(*)(void), int, int);
//extern "C" __destroy_arr(struct DoSomethingCool, void(*)(void), int cnt, int bar);

bool daEnTenten_c::CreateIceActors() {
	struct DoSomethingCool my_struct = { 0, {this->pos.x, this->pos.y - 12.0, this->pos.z}, {1.7, 1.4, 2.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	__destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	this->animationChr.setUpdateRate(0.0f);
	return true;
}


bool daEnTenten_c::calculateTileCollisions() {
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

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0) {
			isBouncing = true;
		}

		speed.y = 0.0f;
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
	if (collMgr.outputMaybe & (0x15 << (!facingRight))) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

bool daEnTenten_c::willWalkOntoSuitableGround() {
	static const float deltas[] = {-2.0f, 2.0f};
	VEC3 checkWhere = {
			pos.x + deltas[(int)facingRight],
			4.0f + pos.y,
			// pos.y - 4.0f,
			pos.z
		};

	u32 props = collMgr.getTileBehaviour2At(checkWhere.x, checkWhere.y, currentLayerID);

	//if (getSubType(props) == B_SUB_LEDGE)
	if (((props >> 16) & 0xFF) == 8)
		return false;

	float someFloat = 0.0f;
	if (collMgr.sub_800757B0(&checkWhere, &someFloat, currentLayerID, 1, -1)) {
		if (someFloat < checkWhere.y && someFloat > (pos.y - 10.0f))
			return true;
	}

	return false;
}


dActor_c* daEnTenten_c::build() {
	void* buffer = AllocFromGameHeap1(sizeof(daEnTenten_c));
	daEnTenten_c* c = new(buffer) daEnTenten_c;

	return c;
}


void daEnTenten_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate)
{
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

int daEnTenten_c::onCreate() {
	if(!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}
	this->sfxTimer = 0;

	int color = (this->eventId1 & 0xF0) >> 4;
    int groupId = (this->settings >> 24) & 0xFF;
	
	this->deleteForever = false; //makes the death state being used in preExecute

	// Model creation
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char* resName = "";
	sprintf(resName, "g3d/t%02d.brres", color);
	resName[strlen(resName)] = 0;

	this->resFile.data = getResource("tenten", resName);
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("tenten");
	model.setup(mdl, &allocator, 0x227, 1, 0);
	SetupTextures_Enemy(&model, 0);

	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	this->anmPat = this->resFile.GetResAnmTexPat("tenten");
	this->animationPat.setup(mdl, anmPat, &this->allocator, 0, 1);
	this->animationPat.bindEntry(&this->model, &anmPat, 0, 1);
	this->animationPat.setFrameForEntry(((this->eventId1 & 0xF) % 5), 0);
	this->animationPat.setUpdateRateForEntry(0.0f, 0);
	this->model.bindAnim(&this->animationPat);

	allocator.unlink();

	ActivePhysics::Info HitMeBaby;

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;

	HitMeBaby.xDistToEdge = 10.0;
	HitMeBaby.yDistToEdge = 8.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;
	HitMeBaby.bitfield2 = 0xffbafffe;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	this->facingRight = eventId2 >> 2 & 1;
	this->wDistance = (settings >> 16 & 0xFF) * 8;
	this->wSpeed = (settings >> 12 & 0xF) * 0.1;

	this->scale = (Vec){0.15, 0.15, 0.15};

	this->rot.x = 0;
	if (facingRight) this->rot.y = 0x2800;
	else this->rot.y = 0xD800;
	this->rot.z = 0;

	this->edgeMin = this->pos.x - this->wDistance;
    this->edgeMax = this->pos.x + this->wDistance;

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);

	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	static const lineSensor_s below(10<<12, -10<<12, -8<<12);
	static const pointSensor_s above(0<<12, 8<<12);
	static const lineSensor_s adjacent(-7<<12, 7<<12, 10<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;

	if (this->wDistance == 0 || this->wSpeed == 0) this->rot.y = 0;
	doStateChange(&StateID_Walk);

	this->onExecute();
	return true;
}


int daEnTenten_c::onDelete() {
	return true;
}

int daEnTenten_c::onDraw() {
	model.scheduleForDrawing();
	return true;
}


void daEnTenten_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y - 8, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);
}

int daEnTenten_c::onExecute() {
	acState.execute();
	model._vf1C();
	updateModelMatrices();

	return true;
}

void daEnTenten_c::executePlaysound() {
	this->sfxTimer++;

	switch (this->sfxTimer) {
		case 8:
			this->playsound(SFX_TENTEN_STEP_L);
			break;

		case 27:
			this->playsound(SFX_TENTEN_STEP_R);
			break;
	}

	if (this->sfxTimer > 38) this->sfxTimer = 0;
}

void daEnTenten_c::playsound(int id) {
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	Vec2 dist = {
		cwci->screenCenterX - this->pos.x,
		cwci->screenCenterY - this->pos.y
	};
	float volume = max<float>(0.0, (1.0 - (sqrtf(dist.x * dist.x + dist.y * dist.y) / 500.0)) * 1.0);
	if (volume <= 0.0) return;
	else if (volume > 1.0) volume = 1.0;

	PlaySoundWithFunctionB4(SoundRelatedClass, &this->handle, id, 1);
	handle.SetVolume(volume, 1);
}

float daEnTenten_c::getValueOfCurve(float ratio, float curveRatioVar, float minValue) {
    float curveRatio = 1.0 - curveRatioVar;

    if (ratio < curveRatioVar) {
        float r = ratio / curveRatioVar;
        return sin(r * (M_PI / 2.0)) / (1.0 / minValue) + minValue;
    }
    else if (ratio < curveRatio) {
        return 1.0;
    }
    else {
        float r = (ratio - curveRatio) / curveRatioVar;
        return (1 - sin(r * (M_PI / 2.0))) / (1.0 / minValue) + minValue;
    }
}

///////////////
// Walk State
///////////////
void daEnTenten_c::beginState_Walk() {
    this->wSpeed = this->facingRight ? abs(this->wSpeed) : -abs(this->wSpeed);
	this->animationChr.setUpdateRate(1.0f);
}
void daEnTenten_c::executeState_Walk() {
	bool ret = calculateTileCollisions();
	this->executePlaysound();

	if (this->animationChr.getUpdateRate() == 0.0f) this->animationChr.setUpdateRate(1.0f);
	else if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

	if (this->wDistance == 0 || this->wSpeed == 0) return;

	if (ret) {
		doStateChange(&StateID_Turn);
		return;
	}

	if (!willWalkOntoSuitableGround()) {
		pos.x = facingRight ? pos.x - 1.5 : pos.x + 1.5;
		doStateChange(&StateID_Turn);
		return;
	}

    float dist = this->edgeMax - this->edgeMin;
    float distRatio;
    if (dist > 8.0) distRatio = 4.0 / dist;
    else distRatio = 0.5;

    float pos = this->pos.x - this->edgeMin;
    float ratio = pos / dist;
    float ratioCurve = this->getValueOfCurve(ratio, distRatio, 0.5);

    float newSpeed = this->wSpeed * ratioCurve;
	this->pos.x += newSpeed;

    if (this->facingRight) {
        if (this->pos.x > this->edgeMax) {
            this->pos.x = this->edgeMax;
            doStateChange(&StateID_Turn);
        }
    }
    else {
        if (this->pos.x < this->edgeMin) {
            this->pos.x = this->edgeMin;
            doStateChange(&StateID_Turn);
        }
    }
}
void daEnTenten_c::endState_Walk() { }

///////////////
// Turn State
///////////////
void daEnTenten_c::beginState_Turn() {
	this->facingRight = !this->facingRight;
	this->animationChr.setUpdateRate(1.0f);
}
void daEnTenten_c::executeState_Turn() {
	calculateTileCollisions();
	this->executePlaysound();

    if (this->animationChr.getUpdateRate() == 0.0f) this->animationChr.setUpdateRate(1.0f);
	else if (this->animationChr.isAnimationDone()) this->animationChr.setCurrentFrame(0.0);

    u16 amt = (this->facingRight == 1) ? 0x2800 : 0xD800;
    int done = SmoothRotation(&this->rot.y, amt, 0x800);

    if(done) {
        doStateChange(&StateID_Walk);
    }
}
void daEnTenten_c::endState_Turn() { }

///////////////
// Die by Stomp State
///////////////
void daEnTenten_c::beginState_DieStomp() {
	this->removeMyActivePhysics();

	bindAnimChr_and_setUpdateRate("damage", 1, 0.0, 1.0);
	this->rot.y = 0;
	this->rot.x = 0;
}
void daEnTenten_c::executeState_DieStomp() {
	if(this->animationChr.isAnimationDone()) this->Delete(true);
}
void daEnTenten_c::endState_DieStomp() { }
