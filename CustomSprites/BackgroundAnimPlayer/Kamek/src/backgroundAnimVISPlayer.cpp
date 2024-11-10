#include <profile.h>
#include <dCourse.h>
#include <background.h>
#include "backgroundAnimPlayer.h"


const char* BackgroundAnimVISPlayerArcNameList [] = {
	NULL
};

class daBackgroundAnimVISPlayer_c : public daBackgroundAnimPlayer_c {
	int onCreate();
	int onDelete();
	int onExecute();

	void eventActivated();
	void eventDeactivated();

	bool ranOnce, eventActive;

	bool doesAnimationExist(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->visAnimation != 0; }
	bool amITheCurrentActor(daFarBG_c::BackgroundModelThing* modelThing) { return modelThing->currentVisActor == this; }
	void setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing);
	void unsetSingleAnimation(daFarBG_c::BackgroundModelThing* modelThing) { if (amITheCurrentActor(modelThing)) modelThing->currentVisActor = 0; }

	public: static dActor_c *build();
};

dActor_c *daBackgroundAnimVISPlayer_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBackgroundAnimVISPlayer_c));
	return new(buffer) daBackgroundAnimVISPlayer_c;
}

const SpriteData BackgroundAnimVISPlayerSpriteData = { ProfileId::BackgroundAnimVISPlayer, 8, -8, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0x2 };
Profile BackgroundAnimVISPlayerProfile(&daBackgroundAnimVISPlayer_c::build, SpriteId::BackgroundAnimVISPlayer, &BackgroundAnimVISPlayerSpriteData, ProfileId::BackgroundAnimVISPlayer, ProfileId::BackgroundAnimVISPlayer, "BackgroundAnimVISPlayer", BackgroundAnimVISPlayerArcNameList, 0);

int daBackgroundAnimVISPlayer_c::onCreate() {
	if (!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	daBackgroundAnimPlayer_c::onCreate();
	this->eventActive = false;

	this->onExecute();
	return true;
}

void daBackgroundAnimVISPlayer_c::setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	modelThing->visAnimation = new m3d::anmVis_c();
	m3d::anmVis_c* animationVis = modelThing->visAnimation;

	nw4r::g3d::ResAnmVis anmVis = resFile->GetResAnmVis(animName);
	animationVis->setup(*mdl, anmVis, &allocator, 0);
	animationVis->bind(model, anmVis, 1);
	model->bindAnim(animationVis, 0.0);
	animationVis->setUpdateRate(1.0);

	modelThing->currentVisActor = this;
}

void daBackgroundAnimVISPlayer_c::bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) {
	m3d::anmVis_c* animationVis = modelThing->visAnimation;
	animationVis->bind(model, resFile->GetResAnmVis(animName), 1);
	model->bindAnim(animationVis, 0.0);
	animationVis->setUpdateRate(1.0);

	modelThing->currentVisActor = this;
}

int daBackgroundAnimVISPlayer_c::onDelete() {
	daBackgroundAnimPlayer_c::onDelete();

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			if (this->bg->backgrounds[j]->currentVisActor == this) this->bg->backgrounds[j]->currentVisActor = 0;
		}
	}

	return true;
}

int daBackgroundAnimVISPlayer_c::onExecute() {
	daBackgroundAnimPlayer_c::onExecute();
	if (!this->eventActive) return true;

	m3d::anmVis_c* animationVis;
	daFarBG_c::BackgroundModelThing* modelThing;

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			modelThing = this->bg->backgrounds[j];

			if (modelThing->visAnimation && amITheCurrentActor(modelThing)) {
				animationVis = modelThing->visAnimation;
				animationVis->process();
				if (animationVis->isAnimationDone() && this->loop) animationVis->setCurrentFrame(0.0);
			}
		}
	}

	if (this->isFirstAnimationDone || !animationVis) return true;
	if (animationVis->isAnimationDone()) this->bindAnimations();

	return true;
}

void daBackgroundAnimVISPlayer_c::eventActivated() {
	this->setupAnimations();
	this->eventActive = true;
}

void daBackgroundAnimVISPlayer_c::eventDeactivated() {
	this->unsetAnimations();
	this->eventActive = false;
}
