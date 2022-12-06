#pragma once
#ifndef ITEM_H
#define ITEM_H
#include "GameObject.h"
#include "GameLogic.h"
#include "Mesh.h"
class Item :
    public GameObject, public GameLogic
{
public:
    Item(const Vector3& s, const Quaternion& r, const Vector3& t);
    virtual ~Item();
    void AddMesh(string tag);
    // Inherited via GameObject
    virtual void SetRotation(const Quaternion& r, int id = 0) override;
    virtual void SetTranslation(const Vector3& t, int id = 0) override;
private:
    Array<Mesh> m_meshes;
};

#endif //ITEM_H