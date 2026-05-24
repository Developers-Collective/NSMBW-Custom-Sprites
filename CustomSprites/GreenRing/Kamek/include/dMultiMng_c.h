#ifndef MULTI_MNG_C__
#define MULTI_MNG_C__

class dMultiMng_c {
public:
    static dMultiMng_c* instance;

    void initStage();
    void setClapSE();
    void setRest(u32, u32);
    void addScore(u32, u32);
    void incCoin(u32);
    void incEnemyDown(u32);
    void setBattleCoin(u32, u32);
    void setCollectionCoin();
};

class dScoreMng_c {
public:
    static dScoreMng_c* instance;

    void sub_800E2190(dEn_c *entity, float xOffs, float yOffs, int comboCount, int playerId);
};

#endif
