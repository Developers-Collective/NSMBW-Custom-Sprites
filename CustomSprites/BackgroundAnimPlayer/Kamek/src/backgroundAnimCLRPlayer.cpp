#include <profile.h>
#include <dCourse.h>
#include <background.h>
#include "backgroundAnimPlayer.h"


const char* BackgroundAnimCLRPlayerArcNameList [] = {
	NULL
};

class daBackgroundAnimCLRPlayer_c : public daBackgroundAnimPlayer_c {
	int onCreate();
	int onDelete();
	int onExecute();

	void eventActivated();
	void eventDeactivated();

	bool ranOnce, eventActive;

	bool doesAnimationExist(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->clrAnimation != 0; }
	bool amITheCurrentActor(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->currentClrActor == this; }
	void setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void unsetSingleAnimation(daFarBG_c::BackgroundModelThing* modelThing) { if (amITheCurrentActor(modelThing)) modelThing->currentClrActor = 0; }

	public: static dActor_c *build();
};

dActor_c *daBackgroundAnimCLRPlayer_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBackgroundAnimCLRPlayer_c));
	return new(buffer) daBackgroundAnimCLRPlayer_c;
}

const SpriteData BackgroundAnimCLRPlayerSpriteData = { ProfileId::BackgroundAnimCLRPlayer, 8, -8, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0x2 };
Profile BackgroundAnimCLRPlayerProfile(&daBackgroundAnimCLRPlayer_c::build, SpriteId::BackgroundAnimCLRPlayer, &BackgroundAnimCLRPlayerSpriteData, ProfileId::BackgroundAnimCLRPlayer, ProfileId::BackgroundAnimCLRPlayer, "BackgroundAnimCLRPlayer", BackgroundAnimCLRPlayerArcNameList, 0);

int daBackgroundAnimCLRPlayer_c::onCreate() {
	if (!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	daBackgroundAnimPlayer_c::onCreate();
	this->eventActive = false;

	this->onExecute();
	return true;
}

void daBackgroundAnimCLRPlayer_c::setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	modelThing->clrAnimation = new m3d::anmClr_c();
	m3d::anmClr_c* animationClr = modelThing->clrAnimation;

	nw4r::g3d::ResAnmClr anmClr = resFile->GetResAnmClr(animName);
	animationClr->setup(*mdl, anmClr, &allocator, 0, 1);
	animationClr->bind(model, anmClr, 0, 1);
	animationClr->setFrameForEntry(0, 0);
	animationClr->setUpdateRateForEntry(1.0f, 0);
	model->bindAnim(animationClr, 1.0);

	modelThing->currentClrActor = this;
}

void daBackgroundAnimCLRPlayer_c::bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	m3d::anmClr_c* animationClr = modelThing->clrAnimation;
	animationClr->bind(model, resFile->GetResAnmClr(animName), 0, 1);
	animationClr->setFrameForEntry(0, 0);
	animationClr->setUpdateRateForEntry(1.0f, 0);
	model->bindAnim(animationClr, 1.0);

	modelThing->currentClrActor = this;
}

int daBackgroundAnimCLRPlayer_c::onDelete() {
	daBackgroundAnimPlayer_c::onDelete();

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			if (this->bg->backgrounds[j]->currentClrActor == this) this->bg->backgrounds[j]->currentClrActor = 0;
		}
	}

	return true;
}

int daBackgroundAnimCLRPlayer_c::onExecute() {
	daBackgroundAnimPlayer_c::onExecute();
	if (!this->eventActive) return true;

	m3d::anmClr_c* animationClr;
	daFarBG_c::BackgroundModelThing* modelThing;

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			modelThing = this->bg->backgrounds[j];

			if (modelThing->clrAnimation && amITheCurrentActor(modelThing)) {
				animationClr = modelThing->clrAnimation;
				animationClr->process();
				if (animationClr->isEntryAnimationDone(0) && this->loop) animationClr->setFrameForEntry(0, 0);
			}
		}
	}

	if (this->isFirstAnimationDone || !animationClr) return true;
	if (animationClr->isEntryAnimationDone(0)) this->bindAnimations();

	return true;
}

void daBackgroundAnimCLRPlayer_c::eventActivated() {
	this->setupAnimations();
	this->eventActive = true;
}

void daBackgroundAnimCLRPlayer_c::eventDeactivated() {
	this->unsetAnimations();
	this->eventActive = false;
}
