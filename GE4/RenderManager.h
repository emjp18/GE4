#pragma once
#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H
#include "pch.h"

class RenderManager
{

public:
    RenderManager();
    void Startup();
    // Prevents any type of copy or new instance
    RenderManager(const RenderManager&) = delete;
    void operator=(const RenderManager&) = delete;

    static RenderManager& Get()
    {
        static RenderManager instance;
        return instance;
    }
};
#endif
