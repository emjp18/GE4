#pragma once
#ifndef ITEM_H
#define ITEM_H
#include "GameObject.h"
#include "GameLogic.h"
class Item :
    public GameObject, public GameLogic
{
    // Inherited via GameObject
    virtual void SetRotation(Quaternion& r, int id = 0) override;
    virtual void SetTranslation(Vector3& t, int id = 0) override;
};

#endif //ITEM_H