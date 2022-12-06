#pragma once
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
class SceneManager
{
public:
    SceneManager();
    void Startup();
    // Prevents any type of copy or new instance
    SceneManager(const SceneManager&) = delete;
    void operator=(const SceneManager&) = delete;


    static SceneManager& Get()
    {
        static SceneManager instance;
        return instance;
    }
    
    ~SceneManager();
    void ShutDown();
};

#endif //SCENEMANAGER