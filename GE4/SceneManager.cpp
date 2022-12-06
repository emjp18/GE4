#include "pch.h"
#include "SceneManager.h"

SceneManager::SceneManager()
{
}

void SceneManager::Startup()
{
	FileManager::Get().ImportFBX("../Content/testBlendshape.fbx");
	auto info = FileManager::Get().GetFileInfo();
	for (int i = 0; i < info.meshCount; i++)
	{
		GOFile::MESH m = FileManager::Get().GetMeshes()[i];
		/*m_meshes.insert("../Content/testBlendshape.fbx" + (string)m.meshName,m);*/
		m_meshes.push_back(m);
	}
}

SceneManager::~SceneManager()
{
}

void SceneManager::ShutDown()
{
}
