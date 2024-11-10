#include <background.h>
#include <profile.h>

void daFarBGNewDoDelete(daFarBG_c * self) {
    for (int i = 0; i < 27; i++) {
        daFarBG_c::BackgroundModelThing *bgMdl = self->backgrounds[i];
        if (bgMdl == 0) continue;

        if (bgMdl->chrAnimation) {
            bgMdl->currentChrActor = 0;
            delete bgMdl->chrAnimation;
        }
        if (bgMdl->clrAnimation) {
            bgMdl->currentClrActor = 0;
            delete bgMdl->clrAnimation;
        }
        if (bgMdl->srtAnimation) {
            bgMdl->currentSrtActor = 0;
            delete bgMdl->srtAnimation;
        }
        if (bgMdl->patAnimation) {
            bgMdl->currentPatActor = 0;
            delete bgMdl->patAnimation;
        }
        if (bgMdl->visAnimation) {
            bgMdl->currentVisActor = 0;
            delete bgMdl->visAnimation;
        }
    }

    self->doDelete(); // Destroy the heap-allocated stuff
}

void daFarBGNewPrepareIndividualBackgroundObject(daFarBG_c * self, ushort bgId, daFarBG_c::BackgroundModelThing *bgMdl, bool unk) {
    bgMdl->chrAnimation = 0;
    bgMdl->clrAnimation = 0;
    bgMdl->srtAnimation = 0;
    bgMdl->patAnimation = 0;
    bgMdl->visAnimation = 0;

    self->prepareIndividualBackgroundObject(bgId, bgMdl, unk);

    if (bgMdl->chrAnimation) bgMdl->currentChrActor = 0;
    if (bgMdl->clrAnimation) bgMdl->currentClrActor = 0;
    if (bgMdl->srtAnimation) bgMdl->currentSrtActor = 0;
    if (bgMdl->patAnimation) bgMdl->currentPatActor = 0;
    if (bgMdl->visAnimation) bgMdl->currentVisActor = 0;
}

void daFarBGNewUpdateAnim(daFarBG_c * self) {
    self->UpdateAnim();

    for (int i = 0; i < 27; i++) {
        daFarBG_c::BackgroundModelThing *bgMdl = self->backgrounds[i];
        if (bgMdl == 0) continue; // Shouldn't happen, but just in case

        if (bgMdl->chrAnimation && bgMdl->currentChrActor == 0) {
            bgMdl->chrAnimation->process();
            if (bgMdl->chrAnimation->isAnimationDone()) bgMdl->chrAnimation->setCurrentFrame(0.0);
        }
        if (bgMdl->clrAnimation && bgMdl->currentClrActor == 0) {
            bgMdl->clrAnimation->process();
            // if (bgMdl->clrAnimation->isEntryAnimationDone(0)) bgMdl->clrAnimation->setFrameForEntry(0.0, 0);
        }
        if (bgMdl->srtAnimation && bgMdl->currentSrtActor == 0) {
            bgMdl->srtAnimation->process();
            // if (bgMdl->srtAnimation->isEntryAnimationDone(0)) bgMdl->srtAnimation->setFrameForEntry(0.0, 0);
        }
        if (bgMdl->patAnimation && bgMdl->currentPatActor == 0) {
            bgMdl->patAnimation->process();
            if (bgMdl->patAnimation->isEntryAnimationDone(0)) bgMdl->patAnimation->setFrameForEntry(0.0, 0);
        }
        if (bgMdl->visAnimation && bgMdl->currentVisActor == 0) {
            bgMdl->visAnimation->process();
            if (bgMdl->visAnimation->isAnimationDone()) bgMdl->visAnimation->setCurrentFrame(0.0);
        }
    }
}
