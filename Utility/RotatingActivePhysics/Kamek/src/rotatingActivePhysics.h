#ifndef __ROTATINGACTIVEPHYSICS_H
#define __ROTATINGACTIVEPHYSICS_H

#include <game.h>

const float RotatingAPDiagonalRatio = 0.35;

class RotatingActivePhysics {
    private:
        dActor_c* owner;
        ActivePhysics aPhysics;
        ActivePhysics::Info info;
        u16 currentRotation;
        u16 rotationOffset;

        Vec2 rotateVec(Vec2 point, u16 rotation);
        void setDirection(u16 rotation);
        bool baseIsVertical() { return this->rotationOffset == 0; }
        bool baseIsHorizontal() { return !this->baseIsVertical(); }

    public:
        void initWithStruct(dActor_c *owner, const ActivePhysics::Info *info);

        void addToList() { this->aPhysics.addToList(); }
        void removeFromList() { this->aPhysics.removeFromList(); }

        u16 getCurrentRotation() { return this->currentRotation; }
        void setRotation(u16 rotation);
        void addRotation(u16 rotation) { this->setRotation(this->currentRotation + rotation); }
        void removeRotation(u16 rotation) { this->setRotation(this->currentRotation - rotation); }
        void resetRotation() { this->setRotation(0); }
};

Vec2 RotatingActivePhysics::rotateVec(Vec2 point, u16 rotation) {
    Vec2 newPoint = { 0.0f, 0.0f };

    float rot = rotation * M_PI / 0x8000;

    float s = sin(rot);
    float c = cos(rot);

    newPoint.x = point.x * c - point.y * s;
    newPoint.y = point.x * s + point.y * c;

    return newPoint;
}

void RotatingActivePhysics::initWithStruct(dActor_c *owner, const ActivePhysics::Info *info) {
    this->owner = owner;
    this->info = *info;
    this->currentRotation = 0;
    this->rotationOffset = 0;

    // Set the trapezoid direction and invert the distances to edges if needed (for horizontal layouts)
    if (info->xDistToEdge >= info->yDistToEdge) {
        this->info.xDistToEdge = info->yDistToEdge;
        this->info.yDistToEdge = info->xDistToEdge;

        this->info.xDistToCenter = info->yDistToCenter;
        this->info.yDistToCenter = info->xDistToCenter;

        this->rotationOffset = 0x4000;
    }

    // Build the trapezoid collider
    this->aPhysics.initWithStruct(owner, info);
    this->aPhysics.collisionCheckType = ActivePhysics::VERTICAL_TRAPEZOID;
}

void RotatingActivePhysics::setRotation(u16 rotation) {
    // Apply rotation offset
    this->currentRotation = rotation;
    u16 newRot = rotation + this->rotationOffset;

    // Set the trapezoid direction and invert the distances to edges if needed
    this->setDirection(newRot);

    // Calculate the new distances to edges to make the rotation a lot smoother
    Vec2 ratioVec = rotateVec((Vec2){ 1.0f, 0.0f }, rotation);
    ratioVec.x = abs(ratioVec.x); ratioVec.y = abs(ratioVec.y);

    if (ratioVec.x < 0.5) ratioVec.x = 1.0 - ratioVec.x;
    if (ratioVec.y < 0.5) ratioVec.y = 1.0 - ratioVec.y;

    // Apply the new distances to edges
    this->aPhysics.info.xDistToEdge *= (double)(this->baseIsVertical() ? ratioVec.y : ratioVec.x);
    this->aPhysics.info.yDistToEdge *= (double)(this->baseIsVertical() ? ratioVec.x : ratioVec.y);

    // Calculate the new distances to center
    Vec2 distToCenterOffset = rotateVec((Vec2){ this->info.xDistToCenter, this->info.yDistToCenter }, newRot);

    // Apply the new distances to center
    this->aPhysics.info.xDistToCenter = distToCenterOffset.x;
    this->aPhysics.info.yDistToCenter = distToCenterOffset.y;

    // Rotate each corner of the trapezoid
    Vec2 topLeft = rotateVec((Vec2){ -this->info.xDistToEdge, this->info.yDistToEdge }, newRot);
    Vec2 topRight = rotateVec((Vec2){ this->info.xDistToEdge, this->info.yDistToEdge }, newRot);
    Vec2 bottomLeft = rotateVec((Vec2){ -this->info.xDistToEdge, -this->info.yDistToEdge }, newRot);
    Vec2 bottomRight = rotateVec((Vec2){ this->info.xDistToEdge, -this->info.yDistToEdge }, newRot);

    // Apply the new distances to edges
    if (this->aPhysics.collisionCheckType == ActivePhysics::HORIZONTAL_TRAPEZOID) {
        // If the trapezoid is horizontal and the rotation is between 45 and 135 degrees or 225 and 315, we need to invert the X values
        if (newRot >= 0x2000 && newRot < 0xA000) {
            this->aPhysics.trpValue0 = -topLeft.x;
            this->aPhysics.trpValue1 = -topRight.x;
            this->aPhysics.trpValue2 = -bottomLeft.x;
            this->aPhysics.trpValue3 = -bottomRight.x;
        }
        // Otherwise, we don't need to invert the X values
        else {
            this->aPhysics.trpValue0 = topLeft.x;
            this->aPhysics.trpValue1 = topRight.x;
            this->aPhysics.trpValue2 = bottomLeft.x;
            this->aPhysics.trpValue3 = bottomRight.x;
        }
    }
    else {
        // In all cases, we need to exchange the Y values between the corners as the trapezoid is vertical
        // If the trapezoid is vertical and the rotation is between 45 and 135 degrees or 225 and 315, we don't need to invert the Y values
        if (newRot >= 0x2000 && newRot < 0xA000) {
            this->aPhysics.trpValue0 = topRight.y;
            this->aPhysics.trpValue1 = topLeft.y;
            this->aPhysics.trpValue2 = bottomRight.y;
            this->aPhysics.trpValue3 = bottomLeft.y;
        }
        // Otherwise, we need to invert the Y values
        else {
            this->aPhysics.trpValue0 = -topRight.y;
            this->aPhysics.trpValue1 = -topLeft.y;
            this->aPhysics.trpValue2 = -bottomRight.y;
            this->aPhysics.trpValue3 = -bottomLeft.y;
        }
    }
}

void RotatingActivePhysics::setDirection(u16 rotation) {
    // If the rotation is between 45 and 135 degrees or 225 and 315, the trapezoid is vertical
    if (rotation >= 0x2000 && rotation <= 0x6000) {
        this->aPhysics.collisionCheckType = ActivePhysics::VERTICAL_TRAPEZOID;
        this->aPhysics.info.xDistToEdge = this->info.yDistToEdge;
        this->aPhysics.info.yDistToEdge = this->info.xDistToEdge;
    }
    else if (rotation >= 0xA000 && rotation <= 0xE000) {
        this->aPhysics.collisionCheckType = ActivePhysics::VERTICAL_TRAPEZOID;
        this->aPhysics.info.xDistToEdge = this->info.yDistToEdge;
        this->aPhysics.info.yDistToEdge = this->info.xDistToEdge;
    }
    // Otherwise, the trapezoid is horizontal
    else {
        this->aPhysics.collisionCheckType = ActivePhysics::HORIZONTAL_TRAPEZOID;
        this->aPhysics.info.xDistToEdge = this->info.xDistToEdge;
        this->aPhysics.info.yDistToEdge = this->info.yDistToEdge;
    }
}

#endif // __ROTATINGACTIVEPHYSICS_H
