#include "pch.h"
#include "Item.h"

Item::Item(Vector3& s, Quaternion& r, Vector3& t)
	:GameObject(s, r, t), GameLogic()
{
}

Item::~Item()
{
}

void Item::AddMesh(string path)
{
	GOFile::MESH m = SceneManager::Get().GetMeshPool()[0];
	
	Mesh mesh(m_transform->s[0], m_transform->r[0], m_transform->t[0]);
	mesh.SetParentTransform(m_transform);
	mesh.SetVertices(RenderManager::Get().getDevice(),)

}

void Item::SetRotation(Quaternion& r, int id)
{
}

void Item::SetTranslation(Vector3& t, int id)
{
}
