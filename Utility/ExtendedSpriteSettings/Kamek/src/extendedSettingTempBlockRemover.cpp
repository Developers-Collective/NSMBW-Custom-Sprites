#include <game.h>
#include <profile.h>


const char* ExtendedSettingTempBlockRemoverArcNameList [] = {
	NULL
};

class daExtendedSettingTempBlockRemover_c : public dStageActor_c {
	int onCreate();
	int onDelete();
	int onExecute();

	u8 timer;

	public: static dActor_c *build();
};

dActor_c *daExtendedSettingTempBlockRemover_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daExtendedSettingTempBlockRemover_c));
	return new(buffer) daExtendedSettingTempBlockRemover_c;
}

const SpriteData ExtendedSettingTempBlockRemoverSpriteData = { ProfileId::ExtendedSettingTempBlockRemover, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile ExtendedSettingTempBlockRemoverProfile(&daExtendedSettingTempBlockRemover_c::build, 0, &ExtendedSettingTempBlockRemoverSpriteData, ProfileId::ExtendedSettingTempBlockRemover, ProfileId::ExtendedSettingTempBlockRemover, "ExtendedSettingTempBlockRemover", ExtendedSettingTempBlockRemoverArcNameList, 0);



int daExtendedSettingTempBlockRemover_c::onCreate() {
	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	this->timer = 0;

	this->onExecute();
	return true;
}

int daExtendedSettingTempBlockRemover_c::onDelete() {
	ExtendedSettingTempBlocks::clearSlot(this->settings & 0xFF);

	return true;
}

int daExtendedSettingTempBlockRemover_c::onExecute() {
	this->timer++;
	if (this->timer > ExtendedSettingTempBlocks::TimeBeforeDeleting) this->Delete(1);

	return true;
}
