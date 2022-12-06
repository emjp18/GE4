#include "pch.h"
#include "SceneManager.h"

SceneManager::SceneManager()
{
}

void SceneManager::Startup()
{
	FileManager::Get().ImportFBX("../Content/testBlendshape.fbx");
	m_fileData.push_back(FileManager::Get().GetGO_FILE());
	
}

SceneManager::~SceneManager()
{
}

void SceneManager::ShutDown()
{
}
