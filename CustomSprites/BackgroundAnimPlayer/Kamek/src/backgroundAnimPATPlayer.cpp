#include <profile.h>
#include <dCourse.h>
#include <background.h>
#include "backgroundAnimPlayer.h"


const char* BackgroundAnimPATPlayerArcNameList [] = {
	NULL
};

class daBackgroundAnimPATPlayer_c : public daBackgroundAnimPlayer_c {
	int onCreate();
	int onDelete();
	int onExecute();

	void eventActivated();
	void eventDeactivated();

	bool ranOnce, eventActive;

	bool doesAnimationExist(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->patAnimation != 0; }
	bool amITheCurrentActor(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->currentPatActor == this; }
	void setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void unsetSingleAnimation(daFarBG_c::BackgroundModelThing* modelThing) { if (amITheCurrentActor(modelThing)) modelThing->currentPatActor = 0; }

	public: static dActor_c *build();
};

dActor_c *daBackgroundAnimPATPlayer_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBackgroundAnimPATPlayer_c));
	return new(buffer) daBackgroundAnimPATPlayer_c;
}

const SpriteData BackgroundAnimPATPlayerSpriteData = { ProfileId::BackgroundAnimPATPlayer, 8, -8, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0x2 };
Profile BackgroundAnimPATPlayerProfile(&daBackgroundAnimPATPlayer_c::build, SpriteId::BackgroundAnimPATPlayer, &BackgroundAnimPATPlayerSpriteData, ProfileId::BackgroundAnimPATPlayer, ProfileId::BackgroundAnimPATPlayer, "BackgroundAnimPATPlayer", BackgroundAnimPATPlayerArcNameList, 0);

int daBackgroundAnimPATPlayer_c::onCreate() {
	if (!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	daBackgroundAnimPlayer_c::onCreate();
	this->eventActive = false;

	this->onExecute();
	return true;
}

void daBackgroundAnimPATPlayer_c::setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	modelThing->patAnimation = new m3d::anmTexPat_c();
	m3d::anmTexPat_c* animationTexPat = modelThing->patAnimation;

	nw4r::g3d::ResAnmTexPat anmTexPat = resFile->GetResAnmTexPat(animName);
	animationTexPat->setup(*mdl, anmTexPat, &allocator, 0, 1);
	animationTexPat->bindEntry(model, &anmTexPat, 0, 1);
	animationTexPat->setFrameForEntry(0, 0);
	animationTexPat->setUpdateRateForEntry(1.0f, 0);
	model->bindAnim(animationTexPat, 1.0);

	modelThing->currentPatActor = this;
}

void daBackgroundAnimPATPlayer_c::bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	m3d::anmTexPat_c* animationTexPat = modelThing->patAnimation;

	nw4r::g3d::ResAnmTexPat anmTexPat = resFile->GetResAnmTexPat(animName);
	animationTexPat->bindEntry(model, &anmTexPat, 0, 1);
	animationTexPat->setFrameForEntry(0, 0);
	animationTexPat->setUpdateRateForEntry(1.0f, 0);

	modelThing->currentPatActor = this;
}

int daBackgroundAnimPATPlayer_c::onDelete() {
	daBackgroundAnimPlayer_c::onDelete();

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			if (this->bg->backgrounds[j]->currentPatActor == this) this->bg->backgrounds[j]->currentPatActor = 0;
		}
	}

	return true;
}

int daBackgroundAnimPATPlayer_c::onExecute() {
	daBackgroundAnimPlayer_c::onExecute();
	if (!this->eventActive) return true;

	m3d::anmTexPat_c* animationTexPat;
	daFarBG_c::BackgroundModelThing* modelThing;

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			modelThing = this->bg->backgrounds[j];

			if (modelThing->patAnimation && amITheCurrentActor(modelThing)) {
				animationTexPat = modelThing->patAnimation;
				animationTexPat->process();
				if (animationTexPat->isEntryAnimationDone(0) && this->loop) animationTexPat->setFrameForEntry(0, 0);
			}
		}
	}

	if (this->isFirstAnimationDone || !animationTexPat) return true;
	if (animationTexPat->isEntryAnimationDone(0)) this->bindAnimations();

	return true;
}

void daBackgroundAnimPATPlayer_c::eventActivated() {
	this->setupAnimations();
	this->eventActive = true;
}

void daBackgroundAnimPATPlayer_c::eventDeactivated() {
	this->unsetAnimations();
	this->eventActive = false;
}
