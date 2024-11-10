#include <profile.h>
#include <dCourse.h>
#include <background.h>
#include "backgroundAnimPlayer.h"


const char* BackgroundAnimSRTPlayerArcNameList [] = {
	NULL
};

class daBackgroundAnimSRTPlayer_c : public daBackgroundAnimPlayer_c {
	int onCreate();
	int onDelete();
	int onExecute();

	void eventActivated();
	void eventDeactivated();

	bool ranOnce, eventActive;

	bool doesAnimationExist(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->srtAnimation != 0; }
	bool amITheCurrentActor(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->currentSrtActor == this; }
	void setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void unsetSingleAnimation(daFarBG_c::BackgroundModelThing* modelThing) { if (amITheCurrentActor(modelThing)) modelThing->currentSrtActor = 0; }

	public: static dActor_c *build();
};

dActor_c *daBackgroundAnimSRTPlayer_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBackgroundAnimSRTPlayer_c));
	return new(buffer) daBackgroundAnimSRTPlayer_c;
}

const SpriteData BackgroundAnimSRTPlayerSpriteData = { ProfileId::BackgroundAnimSRTPlayer, 8, -8, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0x2 };
Profile BackgroundAnimSRTPlayerProfile(&daBackgroundAnimSRTPlayer_c::build, SpriteId::BackgroundAnimSRTPlayer, &BackgroundAnimSRTPlayerSpriteData, ProfileId::BackgroundAnimSRTPlayer, ProfileId::BackgroundAnimSRTPlayer, "BackgroundAnimSRTPlayer", BackgroundAnimSRTPlayerArcNameList, 0);

int daBackgroundAnimSRTPlayer_c::onCreate() {
	if (!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	daBackgroundAnimPlayer_c::onCreate();
	this->eventActive = false;

	this->onExecute();
	return true;
}

void daBackgroundAnimSRTPlayer_c::setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	modelThing->srtAnimation = new m3d::anmTexSrt_c();
	m3d::anmTexSrt_c* animationTexSrt = modelThing->srtAnimation;

	nw4r::g3d::ResAnmTexSrt anmTexSrt = resFile->GetResAnmTexSrt(animName);
	animationTexSrt->setup(*mdl, anmTexSrt, &allocator, 0, 1);
	animationTexSrt->bindEntry(model, anmTexSrt, 0, 1);
	animationTexSrt->setFrameForEntry(0, 0);
	animationTexSrt->setUpdateRateForEntry(1.0f, 0);
	model->bindAnim(animationTexSrt, 1.0);

	modelThing->currentSrtActor = this;
}

void daBackgroundAnimSRTPlayer_c::bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	m3d::anmTexSrt_c* animationTexSrt = modelThing->srtAnimation;

	nw4r::g3d::ResAnmTexSrt anmTexSrt = resFile->GetResAnmTexSrt(animName);
	animationTexSrt->bindEntry(model, anmTexSrt, 0, 1);
	animationTexSrt->setFrameForEntry(0, 0);
	animationTexSrt->setUpdateRateForEntry(1.0f, 0);

	model->bindAnim(animationTexSrt, 1.0);
}

int daBackgroundAnimSRTPlayer_c::onDelete() {
	daBackgroundAnimPlayer_c::onDelete();

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			if (this->bg->backgrounds[j]->currentSrtActor == this) this->bg->backgrounds[j]->currentSrtActor = 0;
		}
	}

	return true;
}

int daBackgroundAnimSRTPlayer_c::onExecute() {
	daBackgroundAnimPlayer_c::onExecute();
	if (!this->eventActive) return true;

	m3d::anmTexSrt_c* animationTexSrt;
	daFarBG_c::BackgroundModelThing* modelThing;

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			modelThing = this->bg->backgrounds[j];

			if (modelThing->srtAnimation && amITheCurrentActor(modelThing)) {
				animationTexSrt = modelThing->srtAnimation;
				animationTexSrt->process();
				if (animationTexSrt->isEntryAnimationDone(0) && this->loop) animationTexSrt->setFrameForEntry(0, 0);
			}
		}
	}

	if (this->isFirstAnimationDone || !animationTexSrt) return true;
	if (animationTexSrt->isEntryAnimationDone(0)) this->bindAnimations();

	return true;
}

void daBackgroundAnimSRTPlayer_c::eventActivated() {
	this->setupAnimations();
	this->eventActive = true;
}

void daBackgroundAnimSRTPlayer_c::eventDeactivated() {
	this->unsetAnimations();
	this->eventActive = false;
}
