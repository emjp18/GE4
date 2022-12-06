#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "Array.h"
#include "Map.h"
#include "SceneManager.h"
#include "RenderManager.h"
using namespace CUSTOM_CONTAINER;
using namespace GOFile;
struct Transform
{
	Array<Vector3> s;
	Array<Quaternion> r;
	Array<Vector3> t;
};
enum class PHYSICS_TYPE
{
	SOLID,
	SOLID_ANIMATED,
	DYNAMIC,
	
	
};
class GameObject
{
private:
	Matrix m_world;
	Matrix m_parentworld;
protected:
	Transform* m_transform = 0;
	Transform* m_parenttransform = 0;
	virtual void SetScale(const Vector3& s, int id = 0);
	bool m_omitS = false;
	bool m_omitR = false;
	bool m_omitT = false;
public:
	GameObject(const Vector3& scale, const Quaternion& rotation, const Vector3& position);
	const Matrix& GetLocalWorld(int id = 0);
	const Matrix& GetGlobalWorld(int id = 0);
	const Matrix& GetParent(int id = 0);
	virtual void SetRotation(const Quaternion& r, int id = 0) = 0;
	virtual void SetTranslation(const Vector3& t, int id = 0) = 0;
	Transform* GetTransform() { return m_transform; }
	Transform* GetParentTransform() { return m_parenttransform; }
	void SetParentTransform(Transform* transform) { m_parenttransform = transform; }
	void SetOmitTransforms(bool omits, bool omitr = false, bool omitt = false) { m_omitS = omits; m_omitR = omitr; m_omitT = omitt; }
	virtual ~GameObject();
};
#endif //GAMEOBJECT_H
