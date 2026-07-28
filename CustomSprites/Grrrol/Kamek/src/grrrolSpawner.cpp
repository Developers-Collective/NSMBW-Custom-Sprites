#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <profile.h>
#include <sfx.h>

const char* GrrrolSpawnerArcNameList[] = {
	"grrrol",
	NULL
};

class daGrrrolSpawner_c : public dStageActor_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c capModel;
	m3d::mdl_c pipeModel;
	m3d::anmChr_c capAnimation;
	mMtx matrix;
	Physics physics;
	Physics::Info physicsInfo;

	bool capLoaded;
	bool pipeLoaded;
	bool animLoaded;
	bool physicsAdded;
	u8 spawnDirection;
	u8 grrrolDirection;
	u8 grrrolSpeed;
	u8 maxGrrrolCount;
	u8 pipeLength;
	u16 spawnPeriod;
	u16 spawnTimer;
	u16 spawnAnimFrame;
	bool shotAnimActive;
	u32 childIDs[16];

	void setupModels();
	void setupPhysics();
	void updateModelMatrices();
	void executeSpawner();
	void startShotAnimation();
	bool spawnGrrrol();
	u32 countTrackedGrrrols();

	public: static dActor_c *build();
};

dActor_c *daGrrrolSpawner_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daGrrrolSpawner_c));
	return new(buffer) daGrrrolSpawner_c;
}

const SpriteData GrrrolSpawnerSpriteData = { ProfileId::GrrrolSpawner, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile GrrrolSpawnerProfile(&daGrrrolSpawner_c::build, SpriteId::GrrrolSpawner, &GrrrolSpawnerSpriteData, ProfileId::Grrrol, ProfileId::GrrrolSpawner, "GrrrolSpawner", GrrrolSpawnerArcNameList, 0);

static const u16 GrrrolSpawnerFrequencyTable[4] = {
	30, 180, 120, 60
};

static u16 GrrrolSpawnerGetPeriod(u8 frequency, u8 delay) {
	if (frequency != 0)
		return GrrrolSpawnerFrequencyTable[frequency & 3];

	if (delay == 0)
		return 0;

	return delay;
}

static u16 GrrrolSpawnerGetAnimFrame(u16 period) {
	return period > 44 ? 45 : 45 - period;
}

static float GrrrolSpawnerPipePixels(u8 pipeLength) {
	return ((float)pipeLength + 1.0f) * 16.0f;
}

static void GrrrolSpawnerPhysCallback1(daGrrrolSpawner_c *one, dStageActor_c *two) {
}

static void GrrrolSpawnerPhysCallback2(daGrrrolSpawner_c *one, dStageActor_c *two) {
}

static void GrrrolSpawnerPhysCallback3(daGrrrolSpawner_c *one, dStageActor_c *two, bool unkMaybeNotBool) {
}

static bool GrrrolSpawnerPhysReturnFalse1(daGrrrolSpawner_c *one, dStageActor_c *two) {
	return false;
}

static bool GrrrolSpawnerPhysReturnFalse2(daGrrrolSpawner_c *one, dStageActor_c *two) {
	return false;
}

static bool GrrrolSpawnerPhysReturnFalse3(daGrrrolSpawner_c *one, dStageActor_c *two, bool unkMaybeNotBool) {
	return false;
}

int daGrrrolSpawner_c::onCreate() {
	u8 frequency = (this->settings >> 28) & 0x3;
	this->pipeLength = (this->settings >> 24) & 0xF;
	this->spawnDirection = (this->settings >> 20) & 0x3;
	this->grrrolSpeed = (this->settings >> 16) & 0x3;
	this->grrrolDirection = (this->settings >> 12) & 0x3;
	this->maxGrrrolCount = (this->settings >> 8) & 0xF;
	u8 spawnDelay = this->settings & 0xFF;

	if (this->spawnDirection > 2)
		this->spawnDirection = 0;

	this->spawnPeriod = GrrrolSpawnerGetPeriod(frequency, spawnDelay);
	this->spawnTimer = this->spawnPeriod;
	this->spawnAnimFrame = GrrrolSpawnerGetAnimFrame(this->spawnPeriod);
	this->shotAnimActive = false;
	this->capLoaded = false;
	this->pipeLoaded = false;
	this->animLoaded = false;
	this->physicsAdded = false;
	for (int i = 0; i < 16; i++)
		this->childIDs[i] = 0;

	if (this->spawnDirection == 0) {
		this->rot.z = -0x4000;
		this->pos.x += 8.0f;
		this->pos.y -= 16.0f;
	} else if (this->spawnDirection == 1) {
		this->rot.z = 0x4000;
		this->pos.x += 40.0f;
		this->pos.y -= 16.0f;
	} else {
		this->rot.z = 0;
		this->pos.x += 16.0f;
		this->pos.y -= 32.0f;
	}

	this->scale = (Vec){1.0f, 1.0f, 1.0f};
	this->pos.z = 2216.0f;
	this->setupModels();
	this->setupPhysics();
	this->updateModelMatrices();

	float pipePixels = GrrrolSpawnerPipePixels(this->pipeLength);
	if (this->spawnDirection == 2) {
		this->spriteSomeRectX = 64.0f;
		this->spriteSomeRectY = pipePixels + 32.0f;
		this->_320 = 0.0f;
		this->_324 = (pipePixels + 32.0f) * 0.5f;
	} else {
		this->spriteSomeRectX = pipePixels + 32.0f;
		this->spriteSomeRectY = 32.0f;
		this->_320 = (pipePixels + 32.0f) * 0.5f;
		if (this->spawnDirection == 1)
			this->_320 = -this->_320;
		this->_324 = 16.0f;
	}

	return true;
}

int daGrrrolSpawner_c::onDelete() {
	if (this->physicsAdded) {
		this->physics.removeFromList();
		this->physicsAdded = false;
	}

	return true;
}

int daGrrrolSpawner_c::onExecute() {
	if (this->physicsAdded)
		this->physics.update();

	this->executeSpawner();

	if (this->animLoaded) {
		this->capAnimation.process();
		if (this->shotAnimActive && this->capAnimation.isAnimationDone()) {
			this->capAnimation.setUpdateRate(0.0f);
			this->shotAnimActive = false;
		}
	}

	this->updateModelMatrices();
	return true;
}

int daGrrrolSpawner_c::onDraw() {
	if (this->pipeLoaded)
		this->pipeModel.scheduleForDrawing();
	if (this->capLoaded)
		this->capModel.scheduleForDrawing();
	return true;
}

void daGrrrolSpawner_c::setupModels() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("grrrol", "g3d/gorogoro.brres");
	if (!this->resFile.data) {
		OSReport("GrrrolSpawner: could not load grrrol/g3d/gorogoro.brres\n");
		allocator.unlink();
		return;
	}

	nw4r::g3d::ResMdl capMdl = this->resFile.GetResMdl("gorogoro_machine_0");
	nw4r::g3d::ResMdl pipeMdl = this->resFile.GetResMdl("gorogoro_machine_1");

	if (capMdl.data) {
		this->capModel.setup(capMdl, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&this->capModel, 0);
		this->capLoaded = true;

		nw4r::g3d::ResAnmChr capAnm = this->resFile.GetResAnmChr("gorogoro_machine_0");
		if (capAnm.data) {
			this->capAnimation.setup(capMdl, capAnm, &allocator, 0);
			this->capAnimation.bind(&this->capModel, capAnm, 1);
			this->capAnimation.setCurrentFrame(0.0f);
			this->capAnimation.setUpdateRate(0.0f);
			this->capModel.bindAnim(&this->capAnimation, 0.0f);
			this->animLoaded = true;
		}
	}

	if (pipeMdl.data) {
		this->pipeModel.setup(pipeMdl, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&this->pipeModel, 0);
		this->pipeLoaded = true;
	}

	allocator.unlink();
}

void daGrrrolSpawner_c::setupPhysics() {
	float pipePixels = GrrrolSpawnerPipePixels(this->pipeLength);

	this->physicsInfo.x1 = -16.0f;
	this->physicsInfo.y1 = pipePixels + 24.0f;
	this->physicsInfo.x2 = 16.0f;
	if (this->spawnDirection == 2) {
        this->physicsInfo.y2 = -7.0f; 
    } else {
        this->physicsInfo.y2 = -8.0f; 
    }

	this->physicsInfo.otherCallback1 = (void*)&GrrrolSpawnerPhysCallback1;
	this->physicsInfo.otherCallback2 = (void*)&GrrrolSpawnerPhysCallback2;
	this->physicsInfo.otherCallback3 = (void*)&GrrrolSpawnerPhysCallback3;

	this->physics.setup(this, &this->physicsInfo, 3, this->currentLayerID, 0);
	this->physics.flagsMaybe = 0x260;
	this->physics.callback1 = (void*)&GrrrolSpawnerPhysReturnFalse1;
	this->physics.callback2 = (void*)&GrrrolSpawnerPhysReturnFalse2;
	this->physics.callback3 = (void*)&GrrrolSpawnerPhysReturnFalse3;
	this->physics.setPtrToRotation(&this->rot.z);
	this->physics.addToList();
	this->physicsAdded = true;
}

void daGrrrolSpawner_c::updateModelMatrices() {
	S16Vec modelRot = {0, 0, this->rot.z};
	Vec capPos = this->pos;
	Vec pipePos = this->pos;
	Vec capScale = {1.0f, 1.0f, 1.0f};
	Vec pipeScale = {1.0f, 1.0f, 1.0f};
	float pipeModelScale = (float)this->pipeLength + 1.0f;

	if (this->spawnDirection == 0) {
		capPos.x += 24.0f;
		pipePos.x = capPos.x;
		pipeScale.y = pipeModelScale;
	} else if (this->spawnDirection == 1) {
		capPos.x -= 24.0f;
		pipePos.x = capPos.x;
		pipeScale.y = pipeModelScale;
	} else {
		capPos.y += 24.0f;
		pipePos.y = capPos.y;
		pipeScale.y = pipeModelScale;
	}

	capPos.z = this->pos.z;
	pipePos.z = this->pos.z;

	if (this->capLoaded) {
		this->matrix.translation(capPos.x, capPos.y, capPos.z);
		this->matrix.applyRotationYXZ(&modelRot.x, &modelRot.y, &modelRot.z);
		this->capModel.setDrawMatrix(this->matrix);
		this->capModel.setScale(&capScale);
		this->capModel.calcWorld(false);
	}

	if (this->pipeLoaded) {
		this->matrix.translation(pipePos.x, pipePos.y, pipePos.z);
		this->matrix.applyRotationYXZ(&modelRot.x, &modelRot.y, &modelRot.z);
		this->pipeModel.setDrawMatrix(this->matrix);
		this->pipeModel.setScale(&pipeScale);
		this->pipeModel.calcWorld(false);
	}
}

void daGrrrolSpawner_c::executeSpawner() {
	u32 activeCount = this->countTrackedGrrrols();

	if (activeCount >= this->maxGrrrolCount) {
		this->spawnTimer = this->spawnPeriod;
		return;
	}

	if (this->isOutOfView())
		return;

	if (this->spawnTimer > 0)
		this->spawnTimer--;

	if (this->spawnTimer == this->spawnAnimFrame)
		this->startShotAnimation();

	if (this->spawnTimer == 0) {
		if (this->spawnGrrrol())
			activeCount++;
		this->spawnTimer = this->spawnPeriod;
	}

}

void daGrrrolSpawner_c::startShotAnimation() {
	if (!this->animLoaded)
		return;

	this->capAnimation.setCurrentFrame(0.0f);
	this->capAnimation.setUpdateRate(1.0f);
	this->capModel.bindAnim(&this->capAnimation, 0.0f);
	this->shotAnimActive = true;
}

bool daGrrrolSpawner_c::spawnGrrrol() {
	Vec spawnPos = this->pos;

	if (this->spawnDirection == 0) {
		spawnPos.x += 14.0f;
		spawnPos.y -= 16.1f;
	} else if (this->spawnDirection == 1) {
		spawnPos.x -= 14.0f;
		spawnPos.y -= 16.1f;
	} else {
		spawnPos.y -= 5.0f;
	}

	spawnPos.z = 1500.0f;

	u8 directionSetting = this->grrrolDirection;
	if (this->spawnDirection == 0)
		directionSetting = 0;
	else if (this->spawnDirection == 1)
		directionSetting = 1;

	u32 childSettings = this->grrrolSpeed | (directionSetting << 4) | (this->spawnDirection << 6) | 0x1000 | 0x40000000;

	dStageActor_c *child = dStageActor_c::create(ProfileId::Grrrol, childSettings, &spawnPos, 0, this->currentLayerID);
	if (!child)
		return false;
	PlaySound(this, SE_EMY_KILLER_SHOT);
	for (int i = 0; i < 16; i++) {
		if (this->childIDs[i] == 0) {
			this->childIDs[i] = child->id;
			break;
		}
	}

	return true;
}

u32 daGrrrolSpawner_c::countTrackedGrrrols() {
	u32 count = 0;

	for (int i = 0; i < 16; i++) {
		u32 childID = this->childIDs[i];
		if (childID == 0)
			continue;

		fBase_c *child = fBase_c::searchById(childID);
		if (child && !child->isDeleted && child->profileId == ProfileId::Grrrol) {
			this->childIDs[count] = childID;
			count++;
		}
	}

	for (int i = count; i < 16; i++)
		this->childIDs[i] = 0;

	return count;
}