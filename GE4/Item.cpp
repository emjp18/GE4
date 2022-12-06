#include "pch.h"
#include "Item.h"

Item::Item(const Vector3& s, const Quaternion& r, const Vector3& t)
	:GameObject(s, r, t), GameLogic()
{
}

Item::~Item()
{
}

void Item::AddMesh(string tag)
{
	Game_Object_File file = SceneManager::Get().GetFilePool()[0]; //change this to a map
	
	Mesh mesh;
	mesh.SetParentTransform(m_transform);
	mesh.SetVertices(RenderManager::Get().getDevice(), file.GetSetVertices()[0]);
	mesh.setIndices(RenderManager::Get().getDevice(), file.GetSetIndicies()[0]);
}

void Item::SetRotation(const Quaternion& r, int id)
{
}

void Item::SetTranslation(const Vector3& t, int id)
{
}
