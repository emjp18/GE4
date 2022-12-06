#pragma once
#ifndef ENGINE_H
#define ENGINE_H
#include "Window.h"
#include "FileManager.h"
#include "RenderManager.h"
class Engine
{
private:
	FileManager* m_filemanager = 0;
	RenderManager* m_rendermanager = 0;
	
	Window* m_window = nullptr;
public:
	Engine(const HINSTANCE& hinstance);
	~Engine();
	void Run();
	void Resize(const HWND& hwnd);
	void ChangeRes(size_t newResX, size_t newResY, const HWND& hwnd);
	
};

#endif

