#ifndef __BACKGROUNDANIMPLAYER_H
#define __BACKGROUNDANIMPLAYER_H

#include <dCourse.h>
#include <background.h>


class daBackgroundAnimPlayer_c : public dEn_c {
	u8 lastEvState;
	u8 newEvState;
	u8 offState;

	u64 eventID;
	u8 affects;

	USING_STATES(daBackgroundAnimPlayer_c);
	DECLARE_STATE(WaitForEventActivation);
	DECLARE_STATE(WaitForEventDeactivation);

	bool checkForEventTrigger(u8 lookForState);

	protected:
		int onCreate();
		int onDelete();
		int onExecute();

		mHeapAllocator_c allocator;
		daFarBG_c* bg;
		bool loop, isFirstAnimationDone, invert;

		/// @brief Setup the animation for all models.
		void setupAnimations();

		/// @brief Bind the animation for all models.
		void bindAnimations();

		/// @brief Unset the animation for all models.
		void unsetAnimations();

		/// @brief Checks if a slot should be affected by the event.
		/// @param slot Slot to check (0-2).
		/// @return Whether the slot should be affected by the event.
		bool isSlotAffected(u8 slot) { return (this->affects & (1 << (2 - slot))) != 0; }

		/// @brief Loads and binds an animation for a model.
		/// @param index Index of the model (0-8).
		/// @param animName Name of the animation.
		/// @param resFile Resource file.
		/// @param mdl Resource model.
		/// @param model Model to load the animation for.
		/// @param modelThing Model thing.
		virtual void setupSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResMdl* mdl, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) { };

		/// @brief Binds an animation for a model.
		/// @param index Index of the model (0-8).
		/// @param animName Name of the animation.
		/// @param resFile Resource file.
		/// @param model Model to bind the animation for.
		virtual void bindSingleAnimation(const char* animName, nw4r::g3d::ResFile* resFile, m3d::mdl_c* model, daFarBG_c::BackgroundModelThing* modelThing) { };

		/// @brief Called when the event is activated.
		virtual void eventActivated() { };

		/// @brief Called when the event is deactivated.
		virtual void eventDeactivated() { };

		/// @brief Checks if an animation exists.
		/// @param modelThing Background model class.
		/// @return Whether the animation exists.
		virtual bool doesAnimationExist(daFarBG_c::BackgroundModelThing* modelThing) { return false;};

		/// @brief Checks if the actor is the current actor.
		/// @param modelThing Background model class.
		/// @return Whether the actor is the current actor.
		virtual bool amITheCurrentActor(daFarBG_c::BackgroundModelThing* modelThing) { return false; };

		/// @brief Unsets the animation.
		/// @param modelThing Background model class.
		virtual void unsetSingleAnimation(daFarBG_c::BackgroundModelThing* modelThing) { };

	public: static dActor_c *build();
};

dActor_c *daBackgroundAnimPlayer_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBackgroundAnimPlayer_c));
	return new(buffer) daBackgroundAnimPlayer_c;
}

CREATE_STATE(daBackgroundAnimPlayer_c, WaitForEventActivation);
CREATE_STATE(daBackgroundAnimPlayer_c, WaitForEventDeactivation);

bool daBackgroundAnimPlayer_c::checkForEventTrigger(u8 lookForState) {
	bool eventTriggered = false;

	this->newEvState = 0;
	if (dFlagMgr_c::instance->flags & this->eventID) this->newEvState = 1;

	if (this->newEvState == this->lastEvState) return false;

	if (this->newEvState == lookForState) {
		this->offState = (this->newEvState == 1) ? 1 : 0;
		eventTriggered = true;
	}

	else this->offState = (this->newEvState == 1) ? 0 : 1;

	this->lastEvState = this->newEvState;

	return eventTriggered;
}

int daBackgroundAnimPlayer_c::onCreate() {
	char eventNum	= (this->settings >> 24) & 0xFF;
	this->eventID = (u64)1 << (eventNum - 1);

	u8 nybble7 = (this->settings >> 20) & 0xF;
	this->affects = nybble7 & 0x7;

	this->bg = getFarBG((nybble7 >> 3) & 0x1);

	this->loop = (this->settings >> 15) & 0x1;
	this->isFirstAnimationDone = !((this->settings >> 14) & 0x1);
	this->invert = (this->settings >> 13) & 0x1;
	this->lastEvState = (int)(!this->invert);

	doStateChange(&StateID_WaitForEventActivation);

	return true;
}

void daBackgroundAnimPlayer_c::setupAnimations() {
	char animName[4];
	sprintf(animName, "%02d", (this->settings >> 16) & 0xF);

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	daFarBG_c::BackgroundEntry* entry = this->bg->m_background_entries[this->bg->isBGB];

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		char resFileName[12], brresFileName[20];
		short bgID = (i == 0 ? entry->file_id_1 : (i == 1 ? entry->file_id_2 : entry->file_id_3));
		sprintf(resFileName, this->bg->isBGB ? "bgB_%04X" : "bgA_%04X", bgID);
		sprintf(brresFileName, this->bg->isBGB ? "g3d/bgB_%04X.brres" : "g3d/bgA_%04X.brres", bgID);
		nw4r::g3d::ResFile resFile;
		resFile.data = getResource(resFileName, brresFileName);

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			daFarBG_c::BackgroundModelThing* modelThing = this->bg->backgrounds[j];

			m3d::mdl_c* model = modelThing->model;
			nw4r::g3d::ResMdl mdl = resFile.GetResMdl(resFileName);

			if (!doesAnimationExist(modelThing)) {
				this->setupSingleAnimation(animName, &resFile, &mdl, model, modelThing);
				continue;
			}

			this->setupSingleAnimation(animName, &resFile, &mdl, model, modelThing);
		}
	}

	allocator.unlink();
}

void daBackgroundAnimPlayer_c::bindAnimations() {
	char animName[4];
	sprintf(animName, "%02d", (this->settings >> 8) & 0xF);

	daFarBG_c::BackgroundEntry* entry = this->bg->m_background_entries[this->bg->isBGB];

	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		char resFileName[12], brresFileName[20];
		short bgID = (i == 0 ? entry->file_id_1 : (i == 1 ? entry->file_id_2 : entry->file_id_3));
		sprintf(resFileName, this->bg->isBGB ? "bgB_%04X" : "bgA_%04X", bgID);
		sprintf(brresFileName, this->bg->isBGB ? "g3d/bgB_%04X.brres" : "g3d/bgA_%04X.brres", bgID);
		nw4r::g3d::ResFile resFile;
		resFile.data = getResource(resFileName, brresFileName);

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			daFarBG_c::BackgroundModelThing* modelThing = this->bg->backgrounds[j];

			m3d::mdl_c* model = modelThing->model;

			if (!doesAnimationExist(modelThing)) {
				nw4r::g3d::ResMdl mdl = resFile.GetResMdl(resFileName);
				this->setupSingleAnimation(animName, &resFile, &mdl, model, modelThing);
				continue;
			}

			this->bindSingleAnimation(animName, &resFile, model, modelThing);
		}
	}

	this->isFirstAnimationDone = true;
	this->loop = true;
}

void daBackgroundAnimPlayer_c::unsetAnimations() {
	for (int i = 0; i < 3; i++) {
		if (!isSlotAffected(i)) continue;

		for (int j = i * 9; j < (i * 9) + 9; j++) {
			daFarBG_c::BackgroundModelThing* modelThing = this->bg->backgrounds[j];

			if (amITheCurrentActor(modelThing)) {
				unsetSingleAnimation(modelThing);
			}
		}
	}
}

int daBackgroundAnimPlayer_c::onDelete() {
	return true;
}

int daBackgroundAnimPlayer_c::onExecute() {
	this->acState.execute();
	return true;
}


#define ACTIVATE	1
#define DEACTIVATE	0

///////////////////////////////////
// Wait for Event Activation
///////////////////////////////////
void daBackgroundAnimPlayer_c::beginState_WaitForEventActivation() { }
void daBackgroundAnimPlayer_c::executeState_WaitForEventActivation() {
	if (this->checkForEventTrigger(this->invert ? DEACTIVATE : ACTIVATE)) {
		this->eventActivated();
		this->doStateChange(&StateID_WaitForEventDeactivation);
	}
}
void daBackgroundAnimPlayer_c::endState_WaitForEventActivation() { }

///////////////////////////////////
// Wait for Event Deactivation
///////////////////////////////////
void daBackgroundAnimPlayer_c::beginState_WaitForEventDeactivation() { }
void daBackgroundAnimPlayer_c::executeState_WaitForEventDeactivation() {
	if (this->checkForEventTrigger(this->invert ? ACTIVATE : DEACTIVATE)) {
		this->eventDeactivated();
		this->doStateChange(&StateID_WaitForEventActivation);
	}
}
void daBackgroundAnimPlayer_c::endState_WaitForEventDeactivation() { }

#endif // __BACKGROUNDANIMPLAYER_H
