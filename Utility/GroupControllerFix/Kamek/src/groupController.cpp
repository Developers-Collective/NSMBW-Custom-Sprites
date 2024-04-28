#include "groupController.h"

extern "C" int groupControllerOnCreate(GroupController *self); // 0x807fc7a0
extern "C" int groupControllerOnExecute(GroupController *self); // 0x807fc860


int groupControllerNewOnCreate(GroupController *self) {
    self->isFirstFrame = true;

    return 1;
}


int groupControllerNewOnExecute(GroupController *self) {
    int ret;

    if (self->isFirstFrame) {
        self->isFirstFrame = false;
        ret = groupControllerOnCreate(self);
    }
    else {
        ret = groupControllerOnExecute(self);
    }

    return ret;
}
