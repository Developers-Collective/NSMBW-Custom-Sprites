#include "konbu.h"


const char* KonbuSpawnerArcNameList [] = {
	"kombooS",
	"kombooM",
	NULL
};

class daKonbuSpawner_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();

	bool ranOnce;

	u8 color;
	bool isBig;
	u16 timer;
	u8 tmpCounter;
	u8 maxKombooCount;

	daKonbu_c** spawnedKomboo;

	void spawnKomboo();
	int getSpawnedKombooCount();
	int getNextKombooFreeIndex();

	USING_STATES(daKonbuSpawner_c);
	DECLARE_STATE(WaitForPlayer);
	DECLARE_STATE(SpawnKomboo);
	DECLARE_STATE(WaitForKill);
	DECLARE_STATE(Cooldown);

	public: static dActor_c *build();
};

dActor_c *daKonbuSpawner_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daKonbuSpawner_c));
	return new(buffer) daKonbuSpawner_c;
}

const SpriteData KonbuSpawnerSpriteData = { ProfileId::KonbuSpawner, 8, -16, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile KonbuSpawnerProfile(&daKonbuSpawner_c::build, SpriteId::KonbuSpawner, &KonbuSpawnerSpriteData, ProfileId::KonbuSpawner, ProfileId::KonbuSpawner, "KonbuSpawner", KonbuSpawnerArcNameList, 0);

CREATE_STATE(daKonbuSpawner_c, WaitForPlayer);
CREATE_STATE(daKonbuSpawner_c, SpawnKomboo);
CREATE_STATE(daKonbuSpawner_c, WaitForKill);
CREATE_STATE(daKonbuSpawner_c, Cooldown);


int daKonbuSpawner_c::onCreate() {
	if (!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	this->maxKombooCount = this->settings >> 20 & 0xF;
	if (this->maxKombooCount == 0) this->maxKombooCount = 1;
	this->spawnedKomboo = new daKonbu_c*[this->maxKombooCount];
	for (int i = 0; i < this->maxKombooCount; i++) this->spawnedKomboo[i] = 0;

	this->color = this->settings >> 28 & 0xF;
	this->isBig = this->settings >> 25 & 0x1;

	doStateChange(&StateID_WaitForPlayer);
	this->onExecute();
	return true;
}

int daKonbuSpawner_c::onDelete() {
	return true;
}

int daKonbuSpawner_c::onExecute() {
	acState.execute();
	return true;
}

void daKonbuSpawner_c::spawnKomboo() {
	int index = this->getNextKombooFreeIndex();
	if (index == -1) return;

	int patColor = MakeRandomNumber(3);
	u32 settings = (this->isBig << 25) | (patColor << 26) | (this->color << 28);

	daKonbu_c* komboo = (daKonbu_c*)CreateActor(ProfileId::Konbu, settings, (Vec){(this->pos.x - 32) + MakeRandomNumber(64), this->pos.y, this->pos.z}, 0, currentLayerID);

	this->spawnedKomboo[index] = komboo;
}

int daKonbuSpawner_c::getSpawnedKombooCount() {
	int count = 0;
	for(int i = 0; i < this->maxKombooCount; i++) {
		if (this->spawnedKomboo[i] != 0) {
			if (this->spawnedKomboo[i]->aPhysics.isLinkedIntoList) count++;
			else this->spawnedKomboo[i] = 0;
		}
	}
	return count;
}

int daKonbuSpawner_c::getNextKombooFreeIndex() {
	for(int i = 0; i < this->maxKombooCount; i++) {
		if (this->spawnedKomboo[i] == 0) return i;
		if (!this->spawnedKomboo[i]->aPhysics.isLinkedIntoList) return i;
	}
	return -1;
}

/////////////////////////////
// Wait for player state
/////////////////////////////
void daKonbuSpawner_c::beginState_WaitForPlayer() {
	this->timer = 0;
}
void daKonbuSpawner_c::executeState_WaitForPlayer() {
	Vec2 dist = (Vec2){24.0f, 24.0f};
	Vec2 pos = (Vec2){this->pos.x, this->pos.y};
	dAcPy_c *player = dActor_c::doSearchNearPlayer(&dist, &pos);

	if (player != 0) doStateChange(&StateID_SpawnKomboo);
}
void daKonbuSpawner_c::endState_WaitForPlayer() { }

/////////////////////////////
// Spawn komboo state
/////////////////////////////
void daKonbuSpawner_c::beginState_SpawnKomboo() {
	this->timer = 0;
	this->tmpCounter = 0;
}
void daKonbuSpawner_c::executeState_SpawnKomboo() {
	this->timer++;
	if (this->timer >= 120) {
		this->timer = 0;
		this->tmpCounter++;
		if (this->tmpCounter <= this->maxKombooCount) this->spawnKomboo();
	}

	if (this->tmpCounter >= this->maxKombooCount + 1) {
		doStateChange(&StateID_WaitForKill);
	}
}
void daKonbuSpawner_c::endState_SpawnKomboo() { }

/////////////////////////////
// Wait for kill state
/////////////////////////////
void daKonbuSpawner_c::beginState_WaitForKill() { }
void daKonbuSpawner_c::executeState_WaitForKill() {
	int count = this->getSpawnedKombooCount();
	if (count == 0) doStateChange(&StateID_Cooldown);
}
void daKonbuSpawner_c::endState_WaitForKill() { }

/////////////////////////////
// Cooldown state
/////////////////////////////
void daKonbuSpawner_c::beginState_Cooldown() {
	this->timer = 0;
}
void daKonbuSpawner_c::executeState_Cooldown() {
	this->timer++;
	if (this->timer >= 300) {
		this->timer = 0;
		doStateChange(&StateID_WaitForPlayer);
	}
}
void daKonbuSpawner_c::endState_Cooldown() { }
