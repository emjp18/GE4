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
    Item(Vector3& s, Quaternion& r, Vector3& t);
    virtual ~Item();
    void AddMesh(string path);
    // Inherited via GameObject
    virtual void SetRotation(Quaternion& r, int id = 0) override;
    virtual void SetTranslation(Vector3& t, int id = 0) override;
private:
    Array<Mesh> m_meshes;
};

#endif //ITEM_H