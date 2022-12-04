#pragma once
#ifndef WINDOW_H
#define WINDOW_H


class Window
{
private:


	const LPCWSTR m_className = L"Game Engine";
	const LPCWSTR m_title = L"Game Engine";
	HINSTANCE m_hInstance;
	static HWND m_hwnd;

public:
	Window(const HINSTANCE& hinstance);
	~Window();
	static void Resize();
	void ChangeRes(size_t newResX, size_t newResY);
	static const HWND& GetHWND() { return m_hwnd; }
	static size_t g_currResX;
	static size_t g_currResY;
	static bool g_shouldResize;

};

#endif

