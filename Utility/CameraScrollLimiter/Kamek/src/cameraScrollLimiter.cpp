#include <game.h>


const BgGmBase::limitLineEntry_s DisabledCameraScrollLimiter = {
	-999999.0, -999999.0,
	-999999.0, -999999.0,
	-999999.0, -999999.0,
	-999999.0, -999999.0,
	-999999.0, -999999.0,
	-999999.0, -999999.0,
	-999999.0, -999999.0,
	-999999.0, -999999.0,
	BgGmBase::UNK_4000
};

const char* CameraScrollLimiterArcNameList [] = {
	NULL
};

const float CameraScrollLimiterBuffer = 96.0;
const float CameraScrollLimiterOffset = 32.0;

class daCameraScrollLimiter_c : public dStageActor_c {
	enum Side {
		CSLStaysOnTop = 0,
		CSLStaysOnBottom = 1,
		CSLStaysOnLeft = 2,
		CSLStaysOnRight = 3
	};

	int onCreate();
	int onDelete();
	int onExecute();

	bool isPosInZone(Vec2 pos, Vec2 tl, Vec2 br) {
		return pos.x >= tl.x && pos.x <= br.x && pos.y <= tl.y && pos.y >= br.y;
	}
	bool isPosInActiveTriggerZone(Vec2 pos);
	bool isActive();
	bool isOnActiveSide();

	float getTargetValue(bool active);
	float* getActualValue();

	bool ranOnce;
	bool permeable, disabled;
	Side side;
	float moveSpeed;
	bool wasActiveLastFrame;
	BgGmBase::limitLineEntry_s lineLimitBackup;
	BgGmBase::limitLineEntry_s *lineLimit;
	Vec2 activeTriggerZoneTL, activeTriggerZoneBR;

	public:
		static dActor_c *build();

		void enable();
		void disable();
};

dActor_c *daCameraScrollLimiter_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daCameraScrollLimiter_c));
	return new(buffer) daCameraScrollLimiter_c;
}

const SpriteData CameraScrollLimiterSpriteData = { ProfileId::CameraScrollLimiter, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0x2 };
Profile CameraScrollLimiterProfile(&daCameraScrollLimiter_c::build, SpriteId::CameraScrollLimiter, &CameraScrollLimiterSpriteData, ProfileId::CameraScrollLimiter, ProfileId::CameraScrollLimiter, "CameraScrollLimiter", CameraScrollLimiterArcNameList, 0);



bool daCameraScrollLimiter_c::isPosInActiveTriggerZone(Vec2 pos) {
	Vec2 tl = this->activeTriggerZoneTL;
	Vec2 br = this->activeTriggerZoneBR;

	ClassWithCameraInfo* camera = ClassWithCameraInfo::instance;

	switch (this->side)
	{
		case CSLStaysOnTop:
			tl.y = br.y + (camera->screenHeight / 2.0) + CameraScrollLimiterBuffer;
			break;

		case CSLStaysOnBottom:
			br.y = tl.y - (camera->screenHeight / 2.0) - CameraScrollLimiterBuffer;
			break;

		case CSLStaysOnLeft:
			tl.x = br.x - (camera->screenWidth / 2.0) - CameraScrollLimiterBuffer;
			break;

		case CSLStaysOnRight:
			br.x = tl.x + (camera->screenWidth / 2.0) + CameraScrollLimiterBuffer;
			break;

		default:
			break;
	}

	return this->isPosInZone(pos, tl, br);
}


bool daCameraScrollLimiter_c::isActive() {
	if (!this->permeable) return true;

	int nbPlayers = GetActivePlayerCount();
	for (int i = 0; i < nbPlayers; i++) {
		dAcPy_c* player = dAcPy_c::findByID(i);
		if (!player) continue;

		if (isPosInActiveTriggerZone((Vec2){player->pos.x, player->pos.y})) {
			return true;
		}
	}

	return false;
}

bool daCameraScrollLimiter_c::isOnActiveSide() {
	if (!this->permeable) return true;

	int nbPlayers = GetActivePlayerCount();
	for (int i = 0; i < nbPlayers; i++) {
		dAcPy_c* player = dAcPy_c::findByID(i);
		if (!player) continue;

		switch (this->side)
		{
			case CSLStaysOnTop:
				if (player->pos.y >= this->pos.y) return true;
				break;

			case CSLStaysOnBottom:
				if (player->pos.y <= this->pos.y) return true;
				break;

			case CSLStaysOnLeft:
				if (player->pos.x <= this->pos.x) return true;
				break;

			case CSLStaysOnRight:
				if (player->pos.x >= this->pos.x) return true;
				break;
			
			default:
				break;
		}
	}

	return false;
}



float daCameraScrollLimiter_c::getTargetValue(bool active) {
	ClassWithCameraInfo* camera = ClassWithCameraInfo::instance;
	float targetValue = 0;

	switch (this->side) {
		case CSLStaysOnTop:
		{
			targetValue = active ? (-lineLimit->spriteYPos) : camera->screenTop - CameraScrollLimiterBuffer - camera->screenHeight - moveSpeed;
			if (targetValue < -lineLimit->zoneBottom) targetValue = -lineLimit->zoneBottom;
			targetValue *= -1.0;
			break;
		}

		case CSLStaysOnBottom:
		{
			targetValue = active ? (-lineLimit->spriteYPos) : camera->screenTop + CameraScrollLimiterBuffer + moveSpeed;
			if (targetValue > -lineLimit->zoneTop) targetValue = -lineLimit->zoneTop;
			targetValue *= -1.0;
			break;
		}

		case CSLStaysOnLeft:
		{
			targetValue = active ? lineLimit->spriteXPos : camera->screenLeft + CameraScrollLimiterBuffer + camera->screenWidth + moveSpeed;
			if (targetValue > lineLimit->zoneRight) targetValue = lineLimit->zoneRight;
			break;
		}

		case CSLStaysOnRight:
		{
			targetValue = active ? lineLimit->spriteXPos : camera->screenLeft - CameraScrollLimiterBuffer - moveSpeed;
			if (targetValue < lineLimit->zoneLeft) targetValue = lineLimit->zoneLeft;
			break;
		}

		default:
			break;
	}

	return targetValue;
}

float* daCameraScrollLimiter_c::getActualValue() {
	ClassWithCameraInfo* camera = ClassWithCameraInfo::instance;
	float *actualValue = 0;

	switch (this->side) {
		case CSLStaysOnTop:
		{
			actualValue = &(lineLimit->yLimitBottom);
			break;
		}

		case CSLStaysOnBottom:
		{
			actualValue = &(lineLimit->yLimitTop);
			break;
		}

		case CSLStaysOnLeft:
		{
			actualValue = &(lineLimit->xLimitRight);
			break;
		}

		case CSLStaysOnRight:
		{
			actualValue = &(lineLimit->xLimitLeft);
			break;
		}

		default:
			break;
	}

	return actualValue;
}



int daCameraScrollLimiter_c::onCreate() {
	if(!this->ranOnce) {
		this->ranOnce = true;
		return false;
	}

	this->disabled = false;

	dCourse_c *area = dCourseFull_c::instance->get(GetAreaNum());
	dCourse_c::zone_s* zone = area->getZoneByID(this->currentZoneID, 0);
	if (!zone) {
		this->Delete(1);
		return true;
	}

	this->side = (Side)((this->settings >> 28) & 0x3);
	this->moveSpeed = ((this->settings >> 8) & 0xFF) * 0.05;
	if (this->moveSpeed == 0.0) this->moveSpeed = 4.0;
	float distance = (((this->settings >> 16) & 0xFFF) * 16.0) + 16.0;
	this->permeable = ((this->settings >> 30) & 0x1) == 0;

	ClassWithCameraInfo* camera = ClassWithCameraInfo::instance;
	bool active = this->permeable;

	BgGmBase::limitLineEntry_s lineLimitLocal;

	lineLimitLocal.zoneTop = zone->yPos;
	lineLimitLocal.zoneBottom = zone->yPos + zone->ySize;
	lineLimitLocal.zoneLeft = zone->xPos;
	lineLimitLocal.zoneRight = zone->xPos + zone->xSize;

	lineLimitLocal.spriteXPos = this->pos.x;
	lineLimitLocal.spriteYPos = this->pos.y * (-1.0);

	switch (side) {
		case CSLStaysOnTop:
		{
			lineLimitLocal.yLimitBottom = lineLimitLocal.spriteYPos;
			lineLimitLocal.yLimitTop = lineLimitLocal.zoneTop;
			lineLimitLocal.topLimitTriggerPos = lineLimitLocal.yLimitTop;
			lineLimitLocal.flags = (u16)(BgGmBase::BOTTOM | BgGmBase::NOT_PERMEABLE_2 | BgGmBase::UNK_1000);
			lineLimitLocal.leftX = this->pos.x;
			lineLimitLocal.rightX = this->pos.x + distance;

			this->activeTriggerZoneTL.x = lineLimitLocal.leftX;
			this->activeTriggerZoneTL.y = lineLimitLocal.zoneTop;
			this->activeTriggerZoneBR.x = lineLimitLocal.rightX;
			this->activeTriggerZoneBR.y = lineLimitLocal.yLimitBottom;

			lineLimit = dBgGm_c::instance->addLineLimitButBetter(&lineLimitLocal, this->currentZoneID);
			active &= this->isActive();

			lineLimit->yLimitBottom = active ? (-lineLimit->spriteYPos) : camera->screenTop - CameraScrollLimiterBuffer - camera->screenHeight - moveSpeed;
			if (lineLimit->yLimitBottom < -lineLimit->zoneBottom) lineLimit->yLimitBottom = -lineLimit->zoneBottom;
			lineLimit->yLimitBottom *= -1.0;

			break;
		}

		case CSLStaysOnBottom:
		{
			lineLimitLocal.yLimitTop = lineLimitLocal.spriteYPos;
			lineLimitLocal.yLimitBottom = lineLimitLocal.zoneBottom;
			lineLimitLocal.topLimitTriggerPos = lineLimitLocal.yLimitBottom;
			lineLimitLocal.flags = (u16)(BgGmBase::TOP | BgGmBase::NOT_PERMEABLE_1 | BgGmBase::UNK_1000);
			lineLimitLocal.leftX = this->pos.x;
			lineLimitLocal.rightX = this->pos.x + distance;

			this->activeTriggerZoneTL.x = lineLimitLocal.leftX;
			this->activeTriggerZoneTL.y = lineLimitLocal.yLimitTop;
			this->activeTriggerZoneBR.x = lineLimitLocal.rightX;
			this->activeTriggerZoneBR.y = lineLimitLocal.zoneBottom;

			lineLimit = dBgGm_c::instance->addLineLimitButBetter(&lineLimitLocal, this->currentZoneID);
			active &= this->isActive();

			lineLimit->yLimitTop = active ? (-lineLimit->spriteYPos) : camera->screenTop + CameraScrollLimiterBuffer + moveSpeed;
			if (lineLimit->yLimitTop > -lineLimit->zoneTop) lineLimit->yLimitTop = -lineLimit->zoneTop;
			lineLimit->yLimitTop *= -1.0;

			break;
		}

		case CSLStaysOnLeft:
		{
			lineLimitLocal.xLimitLeft = lineLimitLocal.spriteXPos;
			lineLimitLocal.xLimitRight = lineLimitLocal.zoneRight;
			lineLimitLocal.leftLimitTriggerPos = lineLimitLocal.xLimitLeft;
			lineLimitLocal.flags = (u16)(BgGmBase::RIGHT | BgGmBase::NOT_PERMEABLE_2 | BgGmBase::UNK_1000);
			lineLimitLocal.topY = this->pos.y;
			lineLimitLocal.bottomY = this->pos.y - distance;

			this->activeTriggerZoneTL.x = lineLimitLocal.zoneLeft;
			this->activeTriggerZoneTL.y = -lineLimitLocal.topY;
			this->activeTriggerZoneBR.x = lineLimitLocal.xLimitLeft - CameraScrollLimiterOffset;
			this->activeTriggerZoneBR.y = -lineLimitLocal.bottomY;

			lineLimit = dBgGm_c::instance->addLineLimitButBetter(&lineLimitLocal, this->currentZoneID);
			active &= this->isActive();

			lineLimit->xLimitRight = active ? lineLimit->spriteXPos : camera->screenLeft + CameraScrollLimiterBuffer + camera->screenWidth + moveSpeed;
			if (lineLimit->xLimitRight > lineLimit->zoneRight) lineLimit->xLimitRight = lineLimit->zoneRight;

			break;
		}

		case CSLStaysOnRight:
		{
			lineLimitLocal.xLimitLeft = lineLimitLocal.zoneLeft;
			lineLimitLocal.xLimitRight = lineLimitLocal.spriteXPos;
			lineLimitLocal.leftLimitTriggerPos = lineLimitLocal.xLimitRight;
			lineLimitLocal.flags = (u16)(BgGmBase::LEFT | BgGmBase::NOT_PERMEABLE_1 | BgGmBase::UNK_1000);
			lineLimitLocal.topY = this->pos.y;
			lineLimitLocal.bottomY = this->pos.y - distance;

			this->activeTriggerZoneTL.x = lineLimitLocal.xLimitRight + CameraScrollLimiterOffset;
			this->activeTriggerZoneTL.y = -lineLimitLocal.topY;
			this->activeTriggerZoneBR.x = lineLimitLocal.zoneRight;
			this->activeTriggerZoneBR.y = -lineLimitLocal.bottomY;

			lineLimit = dBgGm_c::instance->addLineLimitButBetter(&lineLimitLocal, this->currentZoneID);
			active &= this->isActive();

			lineLimit->xLimitLeft = active ? lineLimit->spriteXPos : camera->screenLeft - CameraScrollLimiterBuffer - moveSpeed;
			if (lineLimit->xLimitLeft < lineLimit->zoneLeft) lineLimit->xLimitLeft = lineLimit->zoneLeft;

			break;
		}

		default:
			this->Delete(1);
			return true;
	}

	this->activeTriggerZoneTL.y *= -1.0;
	this->activeTriggerZoneBR.y *= -1.0;

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 99999.0f;
	spriteSomeRectY = 99999.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	this->onExecute();
	return true;
}

int daCameraScrollLimiter_c::onDelete() {
	return true;
}

int daCameraScrollLimiter_c::onExecute() {
	bool active = this->isActive();
	ClassWithCameraInfo* camera = ClassWithCameraInfo::instance;

	if (active && !wasActiveLastFrame) this->enable();

	if (disabled) {
		wasActiveLastFrame = active;
		return true;
	}

	float *actualValue = this->getActualValue();
	float targetValue = this->getTargetValue(active);

	wasActiveLastFrame = active;

	if (actualValue == 0) return true;
	float diff = targetValue - (*actualValue);
	float s = sign(diff);

	if (abs(diff) < moveSpeed) {
		*actualValue = targetValue;
		if (!active && !disabled) this->disable();
		return true;
	}

	*actualValue += moveSpeed * s;
	
	return true;
}


void daCameraScrollLimiter_c::enable() {
	if (!this->disabled) return;
	this->disabled = false;

	memcpy(lineLimit, &lineLimitBackup, sizeof(BgGmBase::limitLineEntry_s));

	ClassWithCameraInfo* camera = ClassWithCameraInfo::instance;

	float targetValue = this->getTargetValue(!this->isOnActiveSide());
	float *actualValue = this->getActualValue();

	if (actualValue == 0) return;

	*actualValue = targetValue;
}

void daCameraScrollLimiter_c::disable() {
	if (this->disabled) return;
	this->disabled = true;

	memcpy(&lineLimitBackup, lineLimit, sizeof(BgGmBase::limitLineEntry_s));

	memcpy(lineLimit, &DisabledCameraScrollLimiter, sizeof(BgGmBase::limitLineEntry_s));
}
