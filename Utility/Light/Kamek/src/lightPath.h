#ifndef __LIGHT_PATH_H
#define __LIGHT_PATH_H

#include <game.h>
#include "light.h"

namespace Light {
    class dLightPath_c {
        public:
            struct Info {
                Vec2 distFromCenter;
                float distanceBelow;
                float distanceAbove;
                float zPos;
                u32 lightType;
                float scale;
            };

        private:
            dCircleLightMask_c** lightMasks;
            mAllocator_c** allocators;
            float** lightDistances;

            dActor_c* owner;
            Info info;
            u16 rotation;

            u32 lightCount;
            u32 lightType;

            Vec2 rotateVec(Vec2 vec, u16 rotation);

        public:
            dLightPath_c();
            ~dLightPath_c();

            void initWithStruct(dActor_c* owner, Info* info);
            void setRotation(u16 rotation);

            u32 execute();
            u32 draw();
    };
};


Light::dLightPath_c::dLightPath_c() {
    this->lightMasks = NULL;
    this->allocators = NULL;

    this->owner = NULL;
    this->rotation = 0;

    this->lightCount = 0;
    this->lightType = 0;
};


Light::dLightPath_c::~dLightPath_c() {
    for (u32 i = 0; i < this->lightCount; i++) {
        delete this->lightMasks[i];
        delete this->allocators[i];
        delete this->lightDistances[i];
    }
    delete[] this->lightMasks;
    delete[] this->allocators;
    delete[] this->lightDistances;
};


Vec2 Light::dLightPath_c::rotateVec(Vec2 vec, u16 rotation) {
    Vec2 newVec = { 0.0f, 0.0f };

    float rot = rotation * M_PI / 0x8000;

    float s = sin(rot);
    float c = cos(rot);

    newVec.x = vec.x * c - vec.y * s;
    newVec.y = vec.x * s + vec.y * c;

    return newVec;
}


void Light::dLightPath_c::initWithStruct(dActor_c* owner, Info* info) {
    this->owner = owner;
    this->info = *info;

    float distance = this->info.distanceAbove - this->info.distanceBelow;
    this->lightCount = (int)ceil(distance / 16.0f);

    this->lightMasks = new dCircleLightMask_c*[lightCount];
    this->allocators = new mAllocator_c*[lightCount];
    this->lightDistances = new float*[lightCount];

    for (u32 i = 0; i < lightCount; i++) {
        dCircleLightMask_c* l = new dCircleLightMask_c();
        this->lightMasks[i] = l;

        mAllocator_c* a = new mAllocator_c();
        this->allocators[i] = a;

        float* dist = new float();
        this->lightDistances[i] = dist;

        *dist = this->info.distanceBelow + (16.0f * i);

        l->init(a, this->info.lightType);
        l->position.x = this->owner->pos.x;
        l->position.y = this->owner->pos.y;
        l->position.z = this->info.zPos;
        l->scale = this->info.scale;
    }

    this->setRotation(0);
};


void Light::dLightPath_c::setRotation(u16 rotation) {
    this->rotation = rotation;

    for (u32 i = 0; i < lightCount; i++) {
        Vec2 lightPos = { this->info.distFromCenter.x, this->info.distFromCenter.y + *this->lightDistances[i] };
        lightPos = this->rotateVec(lightPos, rotation);

        this->lightMasks[i]->position.x = this->owner->pos.x + lightPos.x;
        this->lightMasks[i]->position.y = this->owner->pos.y + lightPos.y;
    }
};


u32 Light::dLightPath_c::execute() {
    for (u32 i = 0; i < this->lightCount; i++) {
        this->lightMasks[i]->execute();
    }
    return 1;
};


u32 Light::dLightPath_c::draw() {
    for (u32 i = 0; i < this->lightCount; i++) {
        this->lightMasks[i]->draw();
    }
    return 1;
};

#endif
