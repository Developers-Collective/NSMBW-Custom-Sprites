#include "smw_DragonCoins.h"

const char* smwDCarcNameList [] = {
	"smwDragoncoin",
	NULL	
};

// externs
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);
extern "C" void GetPositionOfEntrance();

u8 collected[32];
u8 collectedCheckPoint[32];

Vec PlayerPos;

class daSMW_DragonCions_c : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	u64 eventFlag;
	s32 timer;
	u32 delay;

	u32 effect;
	u8 type;

	Vec playerPos;

	u8 currentLevel;

	// Reggie
	int color;		// 6
	u8 numWorld;	// 7
	u8 numLevel;	// 8
	bool isRotate;	// 9 b1
	u8 dCoinID;		// 11

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daSMW_DragonCions_c);

	public: static dActor_c *build();
};

dActor_c *daSMW_DragonCions_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daSMW_DragonCions_c));
	return new(buffer) daSMW_DragonCions_c;
}

const SpriteData smwDCoinSpriteData = { ProfileId::smwDCoin, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile smwDCoinProfile(&daSMW_DragonCions_c::build, SpriteId::smwDCoin, &smwDCoinSpriteData, ProfileId::smwDCoin, ProfileId::smwDCoin, "smwDCoin", smwDCarcNameList, 0);


void daSMW_DragonCions_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	nw4r::snd::SoundHandle GetCoinshandle;

	this->playerPos = GetSpecificPlayerActor(apOther->owner->which_player)->pos;
	Vec goodpos = {this->playerPos.x, this->playerPos.y + 24, this->playerPos.z};
	

	collectedDCoinID[collectionDCoin] = this->dCoinID;				// 獲得したコインのIDを配列に保存	// [0] <- dCoinID + 1
	OSReport("collected coin ID : [%d]\n", dCoinID - 1);	// 獲得したコインのID

	for (int i = 0; i < 16; i++) {
		OSReport("Now Array : [%d]\n", collectedDCoinID[i]);	// Check Array
	}

	collectionDCoin += 1;
	OSReport("collected [%d] coins\n", collectionDCoin);	// 獲得したコインの総量

	OSReport("currentLevel [%d]\n", this->currentLevel);	// The level + world number that placed this sprite
	currentLevelCashe = this->currentLevel;
	OSReport("currentLevelCashe [%d]\n", currentLevelCashe);	// The level + world number that placed this sprite[cache]


	if(collectionDCoin < 5){			// I want to do 1000*2(n-1) ;;
		PlaySoundWithFunctionB4(SoundRelatedClass, &GetCoinshandle, SE_OBJ_GET_DRAGON_COIN, 1);
		if(collectionDCoin == 1){		// 1
			AddScore(1000, apOther->owner->which_player);	// howmany, playerID
			DisplayScoreAddPopup(goodpos, 0x4, apOther->owner->which_player, false);
		}
		else if(collectionDCoin == 2){	// 2
			AddScore(2000, apOther->owner->which_player);
			DisplayScoreAddPopup(goodpos, 0x5, apOther->owner->which_player, false);
		}
		else if(collectionDCoin == 3){	//3
			AddScore(4000, apOther->owner->which_player);
			DisplayScoreAddPopup(goodpos, 0x6, apOther->owner->which_player, false);
		}
		else if(collectionDCoin == 4){	//4
			AddScore(8000, apOther->owner->which_player);
			DisplayScoreAddPopup(goodpos, 0x7, apOther->owner->which_player, false);
		}
	}
	else{
		PlaySoundWithFunctionB4(SoundRelatedClass, &GetCoinshandle, SE_OBJ_GET_DRAGON_COIN_COMPLETE, 1);
		// Apply lives to everyone
		Vec goodpos = {this->playerPos.x, this->playerPos.y + 16, this->playerPos.z};
		oneUPPopup(goodpos);	// Known Issue: The 1UP popup is appear only 1P position. // 既知の問題: 1Pの座標にしか1UPが表示されない。
		for (int i = 0; i < 4; i++) {
			if (Player_Active[i]) {
				addRest(i, 1, true);
			}
		}
	}

	S16Vec nullRot = {0,0,0};
	Vec oneVec = {0.7f, 0.7f, 1.0f};
	SpawnEffect("Wm_ob_starcoinget_gl", 0, &this->pos, &nullRot, &oneVec);

	this->Delete(1);
}
void daSMW_DragonCions_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
}

bool daSMW_DragonCions_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool daSMW_DragonCions_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}
bool daSMW_DragonCions_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool daSMW_DragonCions_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool daSMW_DragonCions_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { 
	return false;
}
bool daSMW_DragonCions_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { 
	return true;
}
bool daSMW_DragonCions_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daSMW_DragonCions_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daSMW_DragonCions_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daSMW_DragonCions_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

int daSMW_DragonCions_c::onCreate() {
	// Reggie settings
	this->color = this->settings >> 24 & 0xF;				// 0000 0"0"00 0000 0000	// nybble 6
	this->numWorld = (this->settings >> 20 & 0xF);			// 0000 00"0"0 0000 0000	// nybble 7
	this->numLevel = (this->settings >> 16 & 0xF);			// 0000 000"0" 0000 0000	// nybble 8
	int nybble9 = this->settings >> 12 & 0xF;				// 0000 000"0" 0000 0000	// nybble 9 (for bools)
	this->isRotate = nybble9 & 0b1;		// 0000 000"+1" 0000 0000
	//this->[empty] = nybble8 & 0b10;
	//this->[empty] = nybble8 & 0b100;
	//this->[empty] = nybble8 & 0b1000;
	this->dCoinID = (this->settings >> 4 & 0xF) + 1;		// 0000 0000 00"0"0 0000	// nybble 11

	// Related models
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16];
	sprintf(resName, "g3d/t%02d.brres", this->color);
	resName[strlen(resName)] = 0;

	resFile.data = getResource("smwDragoncoin", resName);
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("smwDragoncoin");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	allocator.unlink();

	// Load DCoinsInfo
	//FileHandle fh;
	//void *info = LoadFile(&fh, "/NewerRes/DCoinsInfo.bin");
	
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.xDistToEdge = 8.0;
	HitMeBaby.yDistToEdge = 12.5;
	HitMeBaby.category1 = 0x5;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x200;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->pos.x += 0.0;
	this->pos.z = 3000.0;
	
	this->rot.x = 0;
	this->rot.y = 0;
	if(this->color == 0){		// 2D texture
		this->scale.x = 0.8;
		this->scale.y = 0.8;
		this->scale.z = 1.0;

		this->rot.z -= 0x14000;
	}
	else if(this->color == 1){	// By Bukachell
		this->scale.x = 0.22;
		this->scale.y = 0.22;
		this->scale.z = 0.16;

		this->rot.z = 0;
	}
	if(this->color == 2){		// By Microtransagolor
		this->scale.x = 0.8;
		this->scale.y = 0.9;
		this->scale.z = 0.7;

		this->rot.z = 0;
	}

	this->onExecute();


	this->currentLevel = this->numLevel + this->numWorld;

	if(this->currentLevel != currentLevelCashe){
		collectionDCoin = 0;	// 集めた総数の初期化
		for (int i = 0; i < 16; ++i) {	// 配列を全て0に置換。要するに初期化
    		collectedDCoinID[i] = 0;
		}
	}

	OSReport("onCreate currentLevel [%d]\n", this->currentLevel);
	OSReport("onCreate currentLevelCache [%d]\n", currentLevelCashe);

	for (int i = 0; i < 16; i++) {	// コインIDが取得済みのIDリスト内にあるか
		if(this->dCoinID == collectedDCoinID[i]){
			if(this->currentLevel == currentLevelCashe){	// キャッシュに保存したステージデータ(触れた時更新)が現在のステージデータと同じかどうか
				this->Delete(1);	// さーて削除削除削除削除削除削除削除削除削除削除
			}
		}
	}

	return true;
}


int daSMW_DragonCions_c::onDelete() {
	return true;
}

int daSMW_DragonCions_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void daSMW_DragonCions_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

int daSMW_DragonCions_c::onExecute() {
	updateModelMatrices();
	if(this->isRotate){
		this->rot.y += 0x200;
	}

	return true;
}
