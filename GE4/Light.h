#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include "GameObject.h"
#include "GameLogic.h"
class Light :
    public GameObject, public GameLogic
{
    // Inherited via GameObject
    virtual void SetRotation(const Quaternion& r, int id = 0) override;
    virtual void SetTranslation(const Vector3& t, int id = 0) override;
};

#endif //LIGHT_H