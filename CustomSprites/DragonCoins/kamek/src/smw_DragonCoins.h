#ifndef SMW_DRAGONCOINS_H
#define SMW_DRAGONCOINS_H 

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

extern "C" void addRest(int playerIDRest, int howMany, bool reallyAddRest);	// reallyAddRest is always true
extern "C" void incRestAll(bool reallyAddRest);	// reallyAddRest is always true
extern "C" void oneUPPopup(Vec pos);   // set the position (x,y)
extern "C" void AddScore(int howMany, int color);
extern "C" void DisplayScoreAddPopup(Vec pos, int smallScoreID, int playerID, bool wantToShowThisForever);  // smallScoreID list is here -> https://note.com/reaz0n23/n/nb504e036da6d#143de39b-221f-4909-94f8-cc11e4dbdfdd

extern "C" int collectionDCoin = 0;
extern "C" int collectedDCoinID[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//extern "C" u8 numDCoinWorldANDLevelArr[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
extern "C" u8 currentLevelCashe = 0;

#endif /* SMW_DRAGONCOINS_H */
