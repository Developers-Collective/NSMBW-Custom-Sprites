#include <common.h>
#include <game.h>
#include <profile.h>
#include <sfx.h>

const char *FlipBlockFileList[] = {"block_round", 0};
const int FlipBlockFlipCount = 16;
const int FlipBlockFlipFrameCount = 4;
const int FlipBlockFlipMinFrameCount = 4;

class daEnFlipBlock_c : public daEnBlockMain_c {
public:
	Physics::Info physicsInfo;

	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);
	void resetTimer();
	void playsound();

	bool playerOverlaps();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c leftModel, middleModel, eyesModel, rightModel;

	int width, flipsRemaining, timer, maxTimer, sideOffset;
	float middleXScale;
	nw4r::snd::SoundHandle handle; // Sound Handle

	USING_STATES(daEnFlipBlock_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Flipping);

	static dActor_c *build();
};

dActor_c *daEnFlipBlock_c::build() {

	void *buffer = AllocFromGameHeap1(sizeof(daEnFlipBlock_c));
	daEnFlipBlock_c *c = new(buffer) daEnFlipBlock_c;


	return c;
}

const SpriteData FlipBlockSpriteData = { ProfileId::FlipBlock, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile FlipBlockProfile(&daEnFlipBlock_c::build, SpriteId::FlipBlock, &FlipBlockSpriteData, ProfileId::FlipBlock, ProfileId::FlipBlock, "FlipBlock", FlipBlockFileList, 0);


CREATE_STATE(daEnFlipBlock_c, Wait);
CREATE_STATE(daEnFlipBlock_c, Flipping);


int daEnFlipBlock_c::onCreate() {
	int color = this->settings >> 24 & 0xF;
	width = this->settings >> 28 & 0xF;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16];
	getSpriteTexResName(resName, color);
	this->resFile.data = getResource("block_round", resName);

	nw4r::g3d::ResMdl leftMdl = this->resFile.GetResMdl("left");
	leftModel.setup(leftMdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&leftModel, 0);

	nw4r::g3d::ResMdl middleMdl = this->resFile.GetResMdl("middle");
	middleModel.setup(middleMdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&middleModel, 0);

	nw4r::g3d::ResMdl eyesMdl = this->resFile.GetResMdl("eyes");
	eyesModel.setup(eyesMdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&eyesModel, 0);

	nw4r::g3d::ResMdl rightMdl = this->resFile.GetResMdl("right");
	rightModel.setup(rightMdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&rightModel, 0);

	allocator.unlink();


	middleXScale = (1.0 + (width * 1.25)) * scale.x;
	sideOffset = width * 8;


	blockInit(pos.y);

	physicsInfo.x1 = -8 - sideOffset;
	physicsInfo.y1 = 8;
	physicsInfo.x2 = 8 + sideOffset;
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

	doStateChange(&daEnFlipBlock_c::StateID_Wait);

	return true;
}


int daEnFlipBlock_c::onDelete() {
	physics.removeFromList();

	return true;
}


int daEnFlipBlock_c::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	// now check zone bounds based on state
	if (acState.getCurrentState()->isEqual(&StateID_Wait)) {
		checkZoneBoundaries(0);
	}

	return true;
}


int daEnFlipBlock_c::onDraw() {
	mMtx leftMatrix;
	leftMatrix.translation(pos.x - sideOffset, pos.y, pos.z);
	leftMatrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	leftModel.setDrawMatrix(leftMatrix);
	leftModel.setScale(&scale);
	leftModel.calcWorld(false);
	leftModel.scheduleForDrawing();

	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);
	Vec middleScale = {middleXScale, scale.y, scale.z};

	middleModel.setDrawMatrix(matrix);
	middleModel.setScale(&middleScale);
	middleModel.calcWorld(false);
	middleModel.scheduleForDrawing();

	eyesModel.setDrawMatrix(matrix);
	eyesModel.setScale(&scale);
	eyesModel.calcWorld(false);
	eyesModel.scheduleForDrawing();

	mMtx rightMatrix;
	rightMatrix.translation(pos.x + sideOffset, pos.y, pos.z);
	rightMatrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	rightModel.setDrawMatrix(rightMatrix);
	rightModel.setScale(&scale);
	rightModel.calcWorld(false);
	rightModel.scheduleForDrawing();

	return true;
}


void daEnFlipBlock_c::blockWasHit(bool isDown) {
	pos.y = initialY;

	doStateChange(&StateID_Flipping);
}



void daEnFlipBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void daEnFlipBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}



void daEnFlipBlock_c::beginState_Wait() {
}

void daEnFlipBlock_c::endState_Wait() {
}

void daEnFlipBlock_c::executeState_Wait() {
	int result = blockResult();

	if (result == 0)
		return;

	if (result == 1) {
		// doStateChange(&daEnBlockMain_c::StateID_UpMove);
		anotherFlag = 2;
		isGroundPound = false;
		doStateChange(&StateID_Flipping);
	} else {
		// doStateChange(&daEnBlockMain_c::StateID_DownMove);
		anotherFlag = 1;
		isGroundPound = true;
		doStateChange(&StateID_Flipping);
	}
}



void daEnFlipBlock_c::resetTimer() {
	timer = 0;
	maxTimer = max<int>(((FlipBlockFlipCount + 1) - flipsRemaining) * (FlipBlockFlipFrameCount / 2.0), FlipBlockFlipMinFrameCount);
}

void daEnFlipBlock_c::playsound() {
	float ratio = flipsRemaining / (float)FlipBlockFlipCount;
	playSoundDistance(&this->handle, this->pos, SFX_BLOCK_ROUND_SWAY, 0.5 + (ratio / 2.0), 1.0 + (ratio / 2.0));
}

void daEnFlipBlock_c::beginState_Flipping() {
	flipsRemaining = FlipBlockFlipCount;
	resetTimer();
	playsound();
	physics.removeFromList();
}
void daEnFlipBlock_c::executeState_Flipping() {
	timer++;
	float value = timer / (maxTimer * 2.0);
	if (value > 0.5) value -= 0.5;
	float amount = (sin(value * 2.0 * M_PI - (M_PI / 2.0)) + 1) * 0.5;

	if (isGroundPound)
		rot.x = amount * (0x8000);
	else
		rot.x = (amount * -1.0) * (0x8000);

	if (timer >= maxTimer) {
		flipsRemaining--;
		resetTimer();
		if (flipsRemaining <= 0) {
			flipsRemaining = 0;
			if (!playerOverlaps()) doStateChange(&StateID_Wait);
			else playsound();
		}
		else playsound();
	}
}
void daEnFlipBlock_c::endState_Flipping() {
	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.addToList();
}



bool daEnFlipBlock_c::playerOverlaps() {
	dStageActor_c *player = 0;

	Vec myBL = {pos.x - 8.0f, pos.y - 8.0f, 0.0f};
	Vec myTR = {pos.x + 8.0f, pos.y + 8.0f, 0.0f};

	while ((player = (dStageActor_c*)fBase_c::search(PLAYER, player)) != 0) {
		float centerX = player->pos.x + player->aPhysics.info.xDistToCenter;
		float centerY = player->pos.y + player->aPhysics.info.yDistToCenter;

		float left = centerX - player->aPhysics.info.xDistToEdge;
		float right = centerX + player->aPhysics.info.xDistToEdge;

		float top = centerY + player->aPhysics.info.yDistToEdge;
		float bottom = centerY - player->aPhysics.info.yDistToEdge;

		Vec playerBL = {left, bottom + 0.1f, 0.0f};
		Vec playerTR = {right, top - 0.1f, 0.0f};

		if (RectanglesOverlap(&playerBL, &playerTR, &myBL, &myTR))
			return true;
	}

	return false;
}

