#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "GameObject.h"
class ParticleSystem :
    public GameObject
{
    // Inherited via GameObject
    virtual void SetRotation(const Quaternion& r, int id) override;
    virtual void SetTranslation(const Vector3& t, int id) override;
};

#endif //PARTICLESYSTEM_H