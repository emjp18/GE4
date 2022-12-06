#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(const Vector3& scale, const Quaternion& rotation, const Vector3& position)
{
    m_transform = new Transform();
    m_transform->s.push_back(scale);
    m_transform->r.push_back(rotation);
    m_transform->t.push_back(position);
   
}

void GameObject::SetScale(const Vector3& s, int id)
{
    m_transform->s[id] = s;
}

const Matrix& GameObject::GetLocalWorld(int id)
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
   m_world = sm * rm * tm;
   return m_world;


}


const Matrix& GameObject::GetGlobalWorld(int id )
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
    m_world = sm * rm * tm;
    m_world *= GetParent();
    return m_world;
}

const Matrix& GameObject::GetParent(int id )
{
    Matrix sm = XMMatrixScalingFromVector(m_parenttransform->s[id]);
    Matrix rm = XMMatrixRotationQuaternion(m_parenttransform->r[id]);
    Matrix tm = XMMatrixTranslationFromVector(m_parenttransform->t[id]);
    m_parentworld = sm * rm * tm;
    return m_parentworld;
}

GameObject::~GameObject()
{
    RELEASE(m_transform);
}
