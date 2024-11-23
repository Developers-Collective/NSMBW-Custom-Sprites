#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "boss.h"
#include <wakanalib.h>

// Thwimp (koton) sprite by ReaZ0n23

const char* kotonArcList [] = { "koton", NULL };

class dakoton_c : public dEn_c {
public:
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	bool calculateTileCollisions();
	
	ActivePhysics::Info HitMeBaby;

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther); 

	//void _vf148();	// visualDropKill
	void addScoreWhenHit(void *other);
	
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	int timer;
	float baseground;
	float dying;
	int direction; 
	int randRotAnim;
	// Reggie!s
	int color;
	int timewait;
	int jumphight;
	int landSFX;
	int jumpingDistance;
	int firstDirectionReggie;
	int firstDirection;
	int beginDelay;

	USING_STATES(dakoton_c);
	DECLARE_STATE(jumpkoton);
	DECLARE_STATE(waitkoton);
	DECLARE_STATE(fwaitkoton);
	DECLARE_STATE(diekoton);

	static dActor_c* build();
};

dActor_c* dakoton_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dakoton_c));
	return new(buffer) dakoton_c;
}

/////////////////////////////
// more sprites config
const SpriteData kotonSpriteData = { ProfileId::koton, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
// {Sprite's profileID, x pos offset, y pos offset, x spawn offset, y spawn offset, x size offset, y size offset, unk, unk, unk, unk, spawnflag 0}
Profile kotonProfile(&dakoton_c::build, SpriteId::koton, &kotonSpriteData, ProfileId::koton, ProfileId::koton, "koton", kotonArcList, 0x20);
// spawnflag 0x20 : kill after beat level and award scores

/////////////////////////////
// Externs and States
extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
extern "C" void AddScore(int howMany, int color);
extern "C" void DisplayScoreAddPopup(Vec pos, int smallScoreID, int playerID, bool wantToShowThisForever);  // smallScoreID list is here -> https://note.com/reaz0n23/n/nb504e036da6d#143de39b-221f-4909-94f8-cc11e4dbdfdd
CREATE_STATE(dakoton_c, jumpkoton);
CREATE_STATE(dakoton_c, waitkoton);
CREATE_STATE(dakoton_c, fwaitkoton);
CREATE_STATE(dakoton_c, diekoton);


void dakoton_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


/////////////////////////////
// Collision
void dakoton_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {	
	DamagePlayer(this, apThis, apOther);
}
void dakoton_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	return playerCollision(apThis, apOther);
}

bool dakoton_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_BOSS_KOOPA_FIRE_DISAPP);

	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_fireball_hit", 0, &apOther->owner->pos, &nullRot, &oneVec);
	return true;
}
bool dakoton_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_OBJ_EMY_ICEBALL_DISAPP);
	return true;
}
bool dakoton_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	bool wut = dEn_c::collisionCat3_StarPower(apThis, apOther);
	return wut;
}
bool dakoton_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
bool dakoton_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {	
	return collisionCat5_Mario(apThis, apOther);
}
bool dakoton_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}
bool dakoton_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_EMY_DOWN);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &nullRot, &oneVec);

	dEn_c::_vf148();
	doStateChange(&StateID_diekoton);
	return true;
}
bool dakoton_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat5_Mario(apThis, apOther);
}
bool dakoton_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat5_Mario(apThis, apOther);
}
bool dakoton_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat5_Mario(apThis, apOther);
}
bool dakoton_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
	PlaySound(this, SE_EMY_DOWN);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &nullRot, &oneVec);

	Vec thispos = {this->pos.x, this->pos.y + 24, this->pos.z};
	DisplayScoreAddPopup(thispos, 0x1, apOther->owner->which_player, false);
	AddScore(200, apOther->owner->which_player);	// howmany, playerID
	
	dEn_c::_vf148();
	doStateChange(&StateID_diekoton);
	return true;
}
bool dakoton_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

void dakoton_c::addScoreWhenHit(void *other) { }


int dakoton_c::onCreate() {
	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;
	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

	/////////////////////////////
	// Hit me baby
	HitMeBaby.xDistToCenter = 0.0;		// 当たり判定をスプライトの中心からどれだけ離すか(x軸)
	HitMeBaby.yDistToCenter = 9.0;
	HitMeBaby.xDistToEdge = 7.0;		// x軸方向の大きさ
	HitMeBaby.yDistToEdge = 7.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0b1111111110110000101011001011110;	// 右のバイトから、0unk,1ファイア,2アイス,3スター,4unk,5滑り(坂),6unk,7ヒップドロップ,8フェンス,9甲羅,10滑り(ペンギン),11スピン,12unk,13SpinFall(謎),14Fire(No FireBall, but like an explosion),15ヨッシー可食,16ヨッシーが勝手に食うかどうか,17大砲,18持ち上げ,19YoshiBullet,20ファイア(ヨッシー),21アイス(ヨッシー),残りは0

	//HitMeBaby.unkShort1C = 0x0100;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	// 座標ずれを修正するにょ
	pos.x += 8.0;
	pos.y += -18.0;

	// Reggieで設定できるようにするやつ
	this->color = this->settings >> 28 & 0xF;		// 0000 "0"000 0000 0000	// nybble 5
	this->timewait = this->settings >> 24 & 0xF;	// 0000 0"0"00 0000 0000	// nybble 6
	this->jumphight = this->settings >> 20 & 0xF;	// 0000 00"0"0 0000 0000	// nybble 7
	int nybble8 = this->settings >> 16 & 0xF;		// 0000 000"0" 0000 0000	// nybble 8 (for bools)
	//this->[empty] = nybble8 & 0b1000;
	//this->[empty] = nybble8 & 0b100;
	//this->[empty] = nybble8 & 0b10;
	this->firstDirectionReggie = nybble8 & 0b1;		// 0000 000"+1" 0000 0000
	this->landSFX = (247 + (this->settings >> 12 & 0xF));		// 0000 0000 "0"000 0000	// nybble 9
	this->jumpingDistance = this->settings >> 8 & 0xF;	// 0000 0000 0"0"00 0000	// nybble 10
	this->beginDelay = this->settings >> 4 & 0xF;		// 0000 0000 00"0"0 0000	// nybble 11
	// 値の確認用	// Check values
	if (enableDebugMode){
		OSReport("color : %02d\n", this->color);
		OSReport("timewait : %02d\n", this->timewait);
		OSReport("jumphight : %02d\n", this->jumphight);
		OSReport("firstDirectionReggie : %02d\n", this->firstDirectionReggie);
		OSReport("landSFX : %02d\n", this->landSFX);
		OSReport("jumpingDistance : %02d\n", this->jumpingDistance);
	}

	// 初期化ってやつ？
	this->timer = 0;
	this->dying = 0;
	this->baseground = this->pos.y;
	if (this->firstDirectionReggie == 0){
		this->firstDirection = -1;
	}
	else {this->firstDirection = 1;}

	// モデルの読み込み // Model creation
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16]; // バッファサイズを適切に確保
	sprintf(resName, "g3d/t%02d.brres", this->color);
	resName[strlen(resName)] = 0; // これは必要ありません。文字列は既にNULL終端されています。

	resFile.data = getResource("koton", resName);
	//resFile.data = getResource("koton", "g3d/t00.brres");	// Without color option
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("koton");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);
	allocator.unlink();

	// state change
	doStateChange(&StateID_fwaitkoton);
	this->onExecute();
	return true;
}

int dakoton_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
	return true;
}

int dakoton_c::onDelete() { return true; }

int dakoton_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}

/////////////////////////////
// States
// wait
void dakoton_c::beginState_waitkoton() {
	this->timer = 0;
}
void dakoton_c::executeState_waitkoton() {
	if (this->timer >= 10 * this->timewait){
		doStateChange(&StateID_jumpkoton);
	}
	this->timer += 1;
}
void dakoton_c::endState_waitkoton() {}

// Begin wait
void dakoton_c::beginState_fwaitkoton() {
	this->timer = 0;
}
void dakoton_c::executeState_fwaitkoton() {
	if (this->timer >= 10 * this->timewait + this->beginDelay){
		doStateChange(&StateID_jumpkoton);
	}
	this->timer += 1;
}
void dakoton_c::endState_fwaitkoton() {}

// Jump
void dakoton_c::beginState_jumpkoton(){
	// OSReport("begin jump\n");

	this->timer = 0;
	this->baseground = this->pos.y;
	this->rot.z = 0.0;
}
void dakoton_c::executeState_jumpkoton(){
	//if (this->timer < 30){	// jumping
	if (this->jumphight != 0){
		this->pos.y += (2.0f * this->jumphight - ((this->timer * this->timer - this->timer) / (495 / this->jumphight)));	// about 3*n blocks
		this->pos.x += 1.1f * this->firstDirection * this->jumpingDistance;	// about 4*n blocks
	}
	/*}
	else if (this->timer >= 30){	// falling
		this->pos.y += (-1.9f * this->jumphight - ((this->timer * this->timer - this->timer) / (450 / this->jumphight)));
		this->pos.x += 1.1f * this->firstDirection;
	}*/
	
	/*this->randRotAnim = GenerateRandomNumber(19);
	if (20 <= this->timer < 50 && randRotAnim == 0){
		this->rot.z += 0x4000 * -1 * this->firstDirection;
	}*/

	if (this->pos.y < this->baseground){	// get the ground
		// Reset pos.y
		this->pos.y = this->baseground;
		// Reset roy.z
		this->rot.z = 0.0;
		// Appear graphical effect
		Vec tempPos = (Vec){this->pos.x, this->baseground, 5500.0};
		S16Vec nullRot = {0,0,0};
		Vec swiVec = {1.0f, 1.0f, 0.5f};
		Vec smoVec = {1.0f, 1.0f, 0.8f};
		SpawnEffect("Wm_ob_switch", 0, &tempPos, &nullRot, &swiVec);
		SpawnEffect("Wm_mr_2dlandsmoke", 0, &tempPos, &nullRot, &smoVec);
		
		PlaySound(this, this->landSFX);

		// switch the next direction
		if (this->firstDirection == -1){
			this->firstDirection = 1;
		}
		else {this->firstDirection = -1;}

		doStateChange(&StateID_waitkoton);
	}
	this->timer += 1;
}
void dakoton_c::endState_jumpkoton(){}

// die
void dakoton_c::beginState_diekoton(){
	this->removeMyActivePhysics();	// 当たり判定を削除 // remove collision

	this->timer = 0;
	this->dying = 0;
	this->baseground = this->pos.y;
	this->rot.y = 0;
	this->rot.x = 0;
}
void dakoton_c::executeState_diekoton(){
	this->timer += 1;

	this->pos.y = this->baseground + (-0.2 * dying * dying) + 20.0;
	if (this->direction == 0){	// Right
		this->pos.x += 1.5f;
	}
	else{	// Left
		this->pos.x += -1.5f;
	}
	this->rot.y += 0x1000f;
	this->rot.x += 0x1000;

	this->dying += 0.5;

	if (this->timer > 450) {
		OSReport("Killing");
		this->kill();
		this->Delete(this->deleteForever);
	}
}
void dakoton_c::endState_diekoton(){}
