#pragma once
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include "FileManager.h"
class SceneManager
{
public:
    SceneManager();
    void Startup();
    // Prevents any type of copy or new instance
    SceneManager(const SceneManager&) = delete;
    void operator=(const SceneManager&) = delete;
    //Map& GetMeshPool() { return m_meshes; } //by path and name
    Array<GOFile::MESH>& GetMeshPool() { return m_meshes; }
    static SceneManager& Get()
    {
        static SceneManager instance;
        return instance;
    }
    
    ~SceneManager();
    void ShutDown();
private: 
    Array<GOFile::MESH> m_meshes;
    //Map m_meshes;
};

#endif //SCENEMANAGER