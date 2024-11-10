#include <profile.h>
#include <dCourse.h>
#include <background.h>
#include "backgroundAnimPlayer.h"


const char* BackgroundAnimCHRPlayerArcNameList [] = {
	NULL
};

class daBackgroundAnimCHRPlayer_c : public daBackgroundAnimPlayer_c {
	int onCreate();
	int onDelete();
	int onExecute();

	void eventActivated();
	void eventDeactivated();

	bool ranOnce, eventActive;

	bool doesAnimationExist(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->chrAnimation != 0; }
	bool amITheCurrentActor(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->currentChrActor == this; }
	void setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void unsetSingleAnimation(daFarBG_c::BackgroundModelThing* modelThing) { if (amITheCurrentActor(modelThing)) modelThing->currentChrActor = 0; }

	public: static dActor_c *build();
};

dActor_c *daBackgroundAnimCHRPlayer_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBackgroundAnimCHRPlayer_c));
	return new(buffer) daBackgroundAnimCHRPlayer_c;
}

const SpriteData BackgroundAnimCHRPlayerSpriteData = { ProfileId::BackgroundAnimCHRPlayer, 8, -8, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0x2 };
Profile BackgroundAnimCHRPlayerProfile(&daBackgroundAnimCHRPlayer_c::build, SpriteId::BackgroundAnimCHRPlayer, &BackgroundAnimCHRPlayerSpriteData, ProfileId::BackgroundAnimCHRPlayer, ProfileId::BackgroundAnimCHRPlayer, "BackgroundAnimCHRPlayer", BackgroundAnimCHRPlayerArcNameList, 0);

int daBackgroundAnimCHRPlayer_c::onCreate() {
	if (!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	daBackgroundAnimPlayer_c::onCreate();
	this->eventActive = false;

	this->onExecute();
	return true;
}

void daBackgroundAnimCHRPlayer_c::setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	modelThing->chrAnimation = new m3d::anmChr_c();
	m3d::anmChr_c* animationChr = modelThing->chrAnimation;

	nw4r::g3d::ResAnmChr anmChr = resFile->GetResAnmChr(animName);
	animationChr->setup(*mdl, anmChr, &allocator, 0);
	animationChr->bind(model, anmChr, 1);
	model->bindAnim(animationChr, 0.0);
	animationChr->setUpdateRate(1.0);

	modelThing->currentChrActor = this;
}

void daBackgroundAnimCHRPlayer_c::bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	m3d::anmChr_c* animationChr = modelThing->chrAnimation;

	animationChr->bind(model, resFile->GetResAnmChr(animName), 1);
	model->bindAnim(animationChr, 0.0);
	animationChr->setUpdateRate(1.0);

	modelThing->currentChrActor = this;
}

int daBackgroundAnimCHRPlayer_c::onDelete() {
	daBackgroundAnimPlayer_c::onDelete();

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			if (this->bg->backgrounds[j]->currentChrActor == this) this->bg->backgrounds[j]->currentChrActor = 0;
		}
	}

	return true;
}

int daBackgroundAnimCHRPlayer_c::onExecute() {
	daBackgroundAnimPlayer_c::onExecute();
	if (!this->eventActive) return true;

	m3d::anmChr_c* animationChr;
	daFarBG_c::BackgroundModelThing* modelThing;

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			modelThing = this->bg->backgrounds[j];

			if (modelThing->chrAnimation && amITheCurrentActor(modelThing)) {
				animationChr = modelThing->chrAnimation;
				animationChr->process();
				if (animationChr->isAnimationDone() && this->loop) animationChr->setCurrentFrame(0.0);
			}
		}
	}

	if (this->isFirstAnimationDone || !animationChr) return true;
	if (animationChr->isAnimationDone()) this->bindAnimations();

	return true;
}

void daBackgroundAnimCHRPlayer_c::eventActivated() {
	this->setupAnimations();
	this->eventActive = true;
}

void daBackgroundAnimCHRPlayer_c::eventDeactivated() {
	this->unsetAnimations();
	this->eventActive = false;
}
