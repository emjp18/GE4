#pragma once
#include "GameObject.h"
#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H
class AnimatedMesh :
    public GameObject
{
    // Inherited via GameObject
    virtual void SetRotation(const Quaternion& r, int id = 0) override;
    virtual void SetTranslation(const Vector3& t, int id = 0) override;
};

#endif //ANIMATEDMESH_H