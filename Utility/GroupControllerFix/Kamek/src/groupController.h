#ifndef __GROUP_CONTROLLER_H
#define __GROUP_CONTROLLER_H

#include <game.h>


class GroupController : public dStageActor_c { // dActorGroupIdMng_c
    public:
        u8 unk[4];
        bool isFirstFrame;
};


#endif // __GROUP_CONTROLLER_H
