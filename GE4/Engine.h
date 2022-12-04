#pragma once
#ifndef ENGINE_H
#define ENGINE_H
#include "Window.h"
#include "Array.h"
#include "Map.h"
class Engine
{
private:
	

	Window* m_window = nullptr;
public:
	Engine(const HINSTANCE& hinstance);
	~Engine();
	void Run();
	void Resize(const HWND& hwnd);
	void ChangeRes(size_t newResX, size_t newResY, const HWND& hwnd);
	
};

#endif

