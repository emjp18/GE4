#include "pch.h"
#include "Engine.h"
CUSTOM_CONTAINER::Map* CUSTOM_CONTAINER::Map::root = nullptr;
int CUSTOM_CONTAINER::Map::cnt = 0;

Engine::Engine(const HINSTANCE& hinstance)
{
	m_window = new Window(hinstance);
	m_filemanager = new FileManager();
	m_rendermanager = new RenderManager();
	m_filemanager->Get().Startup();
	m_rendermanager->Get().Startup();
	
}

Engine::~Engine()
{
	m_rendermanager->Get().ShutDown();
	m_filemanager->Get().ShutDown();

	RELEASE(m_rendermanager);
	RELEASE(m_filemanager);
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
			if (m_rendermanager->Get().GetIntitialized())
			{
				if (m_window->g_shouldResize == true && m_window->g_shouldResizeOld == false)
				{
					HRESULT hr = m_rendermanager->Get().getswapChain()->GetFullscreenState(
						&m_rendermanager->Get().g_currentlyInFullscreen, nullptr);
					assert(SUCCEEDED(hr));
					if (m_rendermanager->Get().g_currentlyInFullscreen != m_rendermanager->Get().g_pastInFullscreen)
					{
						Resize();
					}

					m_window->g_shouldResize = false;
				}
				m_rendermanager->Get().Render();
				m_rendermanager->Get().Present();
			}
			
		}
	}
}

void Engine::Resize()
{
	m_rendermanager->Get().Resize();
}

void Engine::ChangeRes(size_t newResX, size_t newResY)
{
	m_window->ChangeRes(newResX, newResY);
	m_rendermanager->Get().ChangeResolution();
	
}