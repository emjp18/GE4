#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "GameObject.h"
class ParticleSystem :
    public GameObject
{
    // Inherited via GameObject
    virtual void SetRotation(Quaternion& r, int id) override;
    virtual void SetTranslation(Vector3& t, int id) override;
};

#endif //PARTICLESYSTEM_H