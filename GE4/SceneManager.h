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
    Array<Game_Object_File>& GetFilePool() { return m_fileData; }
    static SceneManager& Get()
    {
        static SceneManager instance;
        return instance;
    }
    
    ~SceneManager();
    void ShutDown();
private: 
    Array<Game_Object_File> m_fileData;
    Map m_gofilemap;
};

#endif //SCENEMANAGER