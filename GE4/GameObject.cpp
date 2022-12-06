#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(Vector3& scale, Quaternion& rotation, Vector3& position)
{
    m_transform = new Transform();
    m_transform->s.push_back(scale);
    m_transform->r.push_back(rotation);
    m_transform->t.push_back(position);
   
}

void GameObject::SetScale(Vector3& s, int id = 0)
{
    m_transform->s[id] = s;
}

const Matrix& GameObject::GetLocalWorld(int id = 0)
{
   
   Matrix sm = XMMatrixScalingFromVector(m_transform->s[id]);
   Matrix rm = XMMatrixRotationQuaternion(m_transform->r[id]);
   Matrix tm = XMMatrixTranslationFromVector(m_transform->t[id]);
   if (m_omitS)
       sm = XMMatrixIdentity();
   if(m_omitR)
       rm = XMMatrixIdentity();
   if(m_omitT)
       tm = XMMatrixIdentity();
   Matrix world = sm * rm * tm;
   return world;


}


const Matrix& GameObject::GetGlobalWorld(int id = 0)
{
    Matrix sm = XMMatrixScalingFromVector(m_transform->s[id]);
    Matrix rm = XMMatrixRotationQuaternion(m_transform->r[id]);
    Matrix tm = XMMatrixTranslationFromVector(m_transform->t[id]);
    if (m_omitS)
        sm = XMMatrixIdentity();
    if (m_omitR)
        rm = XMMatrixIdentity();
    if (m_omitT)
        tm = XMMatrixIdentity();
    Matrix world = sm * rm * tm;
    world *= GetParent();
    return world;
}

const Matrix& GameObject::GetParent(int id = 0)
{
    Matrix sm = XMMatrixScalingFromVector(m_parenttransform->s[id]);
    Matrix rm = XMMatrixRotationQuaternion(m_parenttransform->r[id]);
    Matrix tm = XMMatrixTranslationFromVector(m_parenttransform->t[id]);
    Matrix world = sm * rm * tm;
    return world;
}

GameObject::~GameObject()
{
    RELEASE(m_transform);
}
