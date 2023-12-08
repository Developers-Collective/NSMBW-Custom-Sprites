#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

const char* SignboardArcNameList [] = {
	"signboard",
	NULL
};

class dSignboard_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;

	m3d::anmChr_c chrAnimation;

	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;

	int color, rotation, arrowDirection;
	bool enableGravity, placeBehindOtherSprites;
	s16 colRot;

	static dSignboard_c *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	USING_STATES(dSignboard_c);
	DECLARE_STATE(Wait);
};

dSignboard_c *dSignboard_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSignboard_c));
	return new(buffer) dSignboard_c;
}

///////////////////////
// Externs and States
///////////////////////
	//FIXME make this dEn_c->used...
	extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);


	CREATE_STATE(dSignboard_c, Wait);


////////////////////////
// Collision Functions
////////////////////////


bool dSignboard_c::calculateTileCollisions() {
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

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
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
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}


/*****************************************************************************/
// Glue Code


void dSignboard_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int dSignboard_c::onCreate() {
	this->color = this->settings >> 28 & 0xF;
	this->arrowDirection = this->settings >> 24 & 0xF;

	// int nybble7 = this->settings >> 20 & 0xF; // unused

	int nybble8 = this->settings >> 16 & 0xF;
	this->enableGravity = nybble8 & 0b1000;
	this->placeBehindOtherSprites = nybble8 & 0b100;
	// bool moveSlightlyHorizontally = nybble8 & 0b10;
	// bool moveSlightlyUp = nybble8 & 0b1;

	this->rotation = this->settings & 0xFFFF;

	// Model creation
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char* resName = "";
	sprintf(resName, "g3d/t%02d.brres", this->color);
	resName[strlen(resName)] = 0;
	resFile.data = getResource("signboard", resName);
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("signboard");
	bodyModel.setup(mdl, &allocator, 0, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0);


	// Animations start here
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("signboard");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();


	// Stuff I do understand
	this->scale = (Vec){1.0, 1.0, 1.0};

	// rotation / 0xF * 0xFFFF == rotation * 0x1111
	// BUT 0x1111 doesn't work with some high values for some reason so I'm using 0xFFF
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0; // Y is horizontal axis
	this->rot.z = (float)(this->rotation); // Z is ... an axis >.> (0xFFF ~= 22.5 degrees)
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 0.8;
	this->x_speed_inc = 0.0;
	this->XSpeed = 0.8;

	Vec2 rotationBalance;
	float angle = ((this->rotation / (float)(0xFFFF)) * 360.0) - 90.0;
	if (angle < 0.0) angle += 360.0;
	rotationBalance.x = cos(angle * M_PI / 180.0);
	rotationBalance.y = sin(angle * M_PI / 180.0);

	// if (moveSlightlyHorizontally) this->pos.x -= ((rotationBalance.x + 1.0) / 2.0) * 2.0;
	this->pos.y += 8.0;

	// if (moveSlightlyUp) this->pos.y += 4.0;

	this->pos.z = placeBehindOtherSprites ? -3500.0 : 0.0;


	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	static const pointSensor_s below(0<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 8<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;


	// State Changers
	bindAnimChr_and_setUpdateRate("signboard", 1, 0.0, 0.0);
	this->chrAnimation.setCurrentFrame(this->arrowDirection);
	this->doStateChange(&StateID_Wait);

	this->onExecute();
	return true;
}

int dSignboard_c::onDelete() {
	return true;
}

int dSignboard_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	return true;
}

int dSignboard_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}

void dSignboard_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


///////////////
// Wait State
///////////////
void dSignboard_c::beginState_Wait() {
	this->max_speed.x = 0;
	this->speed.x = 0;

	if (!this->enableGravity) {
		this->max_speed.y = 0.0;
		this->speed.y = 0.0;
		this->y_speed_inc = 0.0;
	} else {
		this->max_speed.y = -4.0;
		this->speed.y = -4.0;
		this->y_speed_inc = -0.1875;
	}
}
void dSignboard_c::executeState_Wait() {
	if (this->enableGravity) bool ret = calculateTileCollisions();

	bodyModel._vf1C();
}
void dSignboard_c::endState_Wait() { }
