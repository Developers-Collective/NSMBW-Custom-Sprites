// Original code by Chickensaver (Bahlph#0486), Updated by Synel and Nin0
// Note from Chickensaver: Special thanks to the people of Horizon and NHD that helped me with this!

#include <game.h>
#include <profile.h>

// Create a class for the spawner that inherits from the default actor class.
class dBetterActorSpawner_c : public dStageActor_c {
public:

    //==========//
    // Methods: //
    //==========//

    static dActor_c *build(); // Method to allocate memory for the actor.

    s32 onCreate(); // Called once when the actor is created.
    s32 onExecute(); // Called every frame that the actor is in existence.

    // Spawns the new actor.
    dStageActor_c* spawnActor();

    // Checks if the new actor is alive.
    bool newActorIsAlive();

    //===============================//
    // Compatibility Mode Variables: //
    //===============================//

    // Checks if onCreate() has run once, because evenId1 is not initially set during onCreate().
    bool ranOnce;

    // Compatibility mode also uses newActor.

    //======================================//
    // Better Actor Spawner Mode Variables: //
    //======================================//

    // eventId2 is nybbles 1-2.  It represents the triggering event id.
    // eventId1 is nybbles 3-4.
    // settings is nybbles 5-12.

    // Profile id of the actor to spawn.  Nybbles 5-7.
    u16 spawnedId;

    // If true, the spawned actor respawns automatically.  Nybble 8 bit 1.
    bool automaticRespawn; 

    // If true, the actor is despawned if the triggering event id is turned off.  Nybble 8 bit 2.
    bool despawnWithoutEvent;

    // If true, the actor spawner is moved to the location of the actor when it is despawned via the
    // event being turned off.  Nybble 8 bit 3.
    bool saveDespawnLocation;

    // If true, the actor spawner will not keep track of the actor it spawns, so multiple actors can
    // be spawned.  Nybble 8 bit 4.
    bool doMultiSpawning;

    // If true, the actor is an extended actor.
    bool isExtended;

    // Sets the number of frames of delay before spawning another actor.  When set to 0, the actor
    // must be respawned manually by turning the event back on.  Otherwise, the event always stays
    // on and the spawner just waits that number of frames.  Reinterpretation of nybble 9.
    u16 spawnDelay;
    u16 timer; // Counts up towards spawnDelay.

    // Pointer to the actor that the actor spawner spawns.
    dStageActor_c* newActor;
};


dActor_c* dBetterActorSpawner_c::build() {
    void* buffer = AllocFromGameHeap1(sizeof(dBetterActorSpawner_c));
    return new(buffer) dBetterActorSpawner_c;
}


const char* BetterActorSpawnerArcNameList [] = {
	NULL
};


const SpriteData BetterActorSpawnerSpriteData = { ProfileId::BetterActorSpawner, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile BetterActorSpawnerProfile(&dBetterActorSpawner_c::build, SpriteId::BetterActorSpawner, &BetterActorSpawnerSpriteData, ProfileId::BetterActorSpawner, ProfileId::BetterActorSpawner, "BetterActorSpawner", BetterActorSpawnerArcNameList, 0);


s32 dBetterActorSpawner_c::onCreate() {

    // Return the first time in order to get the values of eventId1 and eventId2.
    if (this->ranOnce == false) {
        this->ranOnce = true;
        return false; // Retry onCreate().
    }

    this->spawnedId = (this->getBlockSettings(0) >> 20) & 0xFFF; // Grab [Block 1] Nybbles 5-7.
    this->automaticRespawn = (this->getBlockSettings(0) >> 19) & 0x1; // Grab [Block 1] Nybble 8 bit 1.
    this->despawnWithoutEvent = (this->getBlockSettings(0) >> 18) & 0x1; // Grab [Block 1] Nybble 8 bit 2.
    this->saveDespawnLocation = (this->getBlockSettings(0) >> 17) & 0x1; // Grab [Block 1] Nybble 8 bit 3.
    this->doMultiSpawning = (this->getBlockSettings(0) >> 16) & 0x1; // Grab [Block 1] Nybble 8 bit 4.
    this->isExtended = (this->eventId1 >> 3) & 0x1; // Grab [Block 0] Nybble 4 bit 1.

    this->spawnDelay = (eventId1 >> 4) & 0xF; // Grab [Block 0] Nybble 3.
    switch (this->spawnDelay) { 
        case 0:  this->spawnDelay = 0;  break;
        case 1:  this->spawnDelay = 1;  break;
        case 2:  this->spawnDelay = 10; break;
        case 3:  this->spawnDelay = 20; break;
        default: this->spawnDelay = (this->spawnDelay - 3) * 30; // 30, 60, 90, 120, etc.
    }
    this->timer = 0; // Start the timer at 0.
    
    return true;
}


s32 dBetterActorSpawner_c::onExecute() {
    // If the event is on:
    if ( dFlagMgr_c::instance->active(this->eventId2 - 1) ) {

        // If the new actor doesn't exist yet:
        if (this->newActor == 0) {

            if (this->doMultiSpawning == false) { // Multi-spawning is off; keep track of the actor.

                this->newActor = this->spawnActor(); // Spawn a new actor and keep track of it.
                return true;

            }

            // Multi-spawning is on, but should it be manual or based on a timer?
            if (this->spawnDelay == 0) { // Spawn Delay is set to the manual option.

                this->spawnActor(); // Spawn a new actor without keeping track of it.
                // Turn off the event.
                dFlagMgr_c::instance->set( // Parameters as defined by game.h:
                    (this->eventId2 - 1),  // number
                    0,                     // delay
                    false,                 // activate
                    false,                 // reverseEffect
                    false                  // makeNoise
                );

            } else { // Spawn Delay is set to a number of frames.

                // Check if the number of frames specified has passed.
                if (this->timer == this->spawnDelay) {
                    this->timer = 0; // Reset the timer.
                    this->spawnActor(); // Spawn a new actor.
                } else {
                    this->timer += 1; // Increment the timer towards spawnDelay.
                }

            }
            return true;

        }

        // If automaticRespawn is on:
        if (automaticRespawn == true) {
            // Return if the actor is alive.
            if ( this->newActorIsAlive() ) {return true;}
            // Otherwise, re-create the new actor.
            this->newActor = this->spawnActor();
        }
    } else { // The event isn't on.
        // Return if despawnWithoutEvent isn't on.
        if (this->despawnWithoutEvent == false) {return true;}
        // Return if the actor doesn't have a pointer to it.
        if (this->newActor == 0) {return true;}
        // Return if the actor isn't alive.
        if ( !(this->newActorIsAlive()) ) {return true;}
        // Otherwise, move the position of the actor spawner if saveDespawnLocation is on.
        if (this->saveDespawnLocation == true) {
            this->pos = this->newActor->pos;
        }
        // And then despawn the actor.
        this->newActor->Delete(1);
        this->newActor = 0;
    }

    return true;
}


dStageActor_c* dBetterActorSpawner_c::spawnActor() {
    dStageActor_c* actor;

    if (this->isExtended) {
        const u32 extendedSettingsCount = this->getBlockSettingCount() - 1;
        u32 *extendedSettings;
        extendedSettings = new u32[extendedSettingsCount];

        for (int i = 0; i < extendedSettingsCount; i++) {
            extendedSettings[i] = this->getBlockSettings(i + 1);
        }

        actor = (dStageActor_c*) dStageActor_c::createExtended(
            (Actors) this->spawnedId,
            extendedSettings,
            &(this->pos),
            0, 0
        );

        delete[] extendedSettings;
    }

    else {
        actor = (dStageActor_c*) dStageActor_c::create(
            (Actors) this->spawnedId,
            this->getBlockSettings(1),
            &(this->pos),
            0, 0
        );
    }

    // Set nybbles 1-4 of the new actor.
    u16 eventSettings = this->getBlockSettings(0) & 0xFFFF;
    actor->eventId2 = (eventSettings >> 8) & 0xFF;
    actor->eventId1 = eventSettings & 0xFF;

    return actor;
}


bool dBetterActorSpawner_c::newActorIsAlive() {
    // Return true if the search doesn't return 0; the actor is alive.
    return fBase_c::search(this->newActor->id) != 0;
}
