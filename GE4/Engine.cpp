#include "pch.h"
#include "Engine.h"
CUSTOM_CONTAINER::Map* CUSTOM_CONTAINER::Map::root = nullptr;
int CUSTOM_CONTAINER::Map::cnt = 0;

Engine::Engine(const HINSTANCE& hinstance)
{
	m_window = new Window(hinstance);

}

Engine::~Engine()
{
	RELEASE(m_window);
	//Renderer::getDebug()->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
}

void Engine::Run()
{
	MSG msg = { };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);


		}
		else
		{

		}
	}
}

void Engine::Resize(const HWND& hwnd)
{
	
}

void Engine::ChangeRes(size_t newResX, size_t newResY, const HWND& hwnd)
{
	m_window->ChangeRes(newResX, newResY);
	
}