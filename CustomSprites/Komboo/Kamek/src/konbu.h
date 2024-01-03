#ifndef __KOMBOO_H
#define __KOMBOO_H

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>


const char* KombooArcNameList [] = {
	"kombooS",
	"kombooM",
	NULL
};

class daKonbu_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c model;

	m3d::anmChr_c animationChr;
	m3d::anmTexPat_c animationPat;
	nw4r::g3d::ResAnmTexPat anmPat;

	mEf::es2 effect;

	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	bool stillFalling;
	bool ranOnce;

	bool isBig;

	// StandOnTopCollider giantRider;

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	// bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();

	bool willWalkOntoSuitableGround();
	bool isInLiquid();

	USING_STATES(daKonbu_c);
	DECLARE_STATE(Appear);
	DECLARE_STATE(Sink);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);

	public: static dActor_c *build();
};

#endif // __KOMBOO_H
