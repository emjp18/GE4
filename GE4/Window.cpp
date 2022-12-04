#include "pch.h"
#include "Window.h"
bool Window::g_shouldResize = false;
HWND Window::m_hwnd = nullptr;
size_t Window::g_currResX = g_mediumResX;
size_t Window::g_currResY = g_mediumResY;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
Window::Window(const HINSTANCE& hinstance)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASS));
	wndclass.hInstance = m_hInstance;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.lpszClassName = m_className;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.hIcon = LoadIcon(m_hInstance, L"MAINICON");
	RegisterClass(&wndclass);
	RECT windowRect = { 0, 0, (LONG)g_currResX, (LONG)g_currResY };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	int wWidth = windowRect.right - windowRect.left;
	int wHeight = windowRect.bottom - windowRect.top;
	m_hwnd = CreateWindowEx(0, m_className, m_title, WS_OVERLAPPEDWINDOW, windowRect.left, windowRect.top, wWidth, wHeight, nullptr, nullptr, m_hInstance, nullptr);
	SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	ShowWindow(m_hwnd, SW_SHOW);


}

Window::~Window()
{
	UnregisterClass(m_className, m_hInstance);
	DestroyWindow(m_hwnd);
}

void Window::Resize()
{
	g_shouldResize = true;

}

void Window::ChangeRes(size_t newResX, size_t newResY)
{
	g_currResX = newResX;
	g_currResY = newResY;
}

#if USE_IMGUI
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
LRESULT WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
#if USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wParam, lParam))
		return true;
#endif

	Keyboard::ProcessMessage(umsg, wParam, lParam);
	Mouse::ProcessMessage(umsg, wParam, lParam);

	switch (umsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	case WM_WINDOWPOSCHANGED:
	{
		/*if (Renderer::g_isInitiated)
		{

			HRESULT hr = Renderer::getswapChain()->GetFullscreenState(&Renderer::g_currentlyInFullscreen, nullptr);
			assert(SUCCEEDED(hr));
			if (Renderer::g_currentlyInFullscreen != Renderer::g_pastInFullscreen)
			{
				Window::resize();

			}
		}*/
		Window::Resize();
	}
	case WM_INPUT:
	{

		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpb[sizeof(RAWINPUT)];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			/*InputHandler::getInstance().g_mouseX += (int)raw->data.mouse.lLastX;
			InputHandler::getInstance().g_mouseY += (int)raw->data.mouse.lLastY;*/

		}
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
	}
	return 0;
	}

	return DefWindowProc(hwnd, umsg, wParam, lParam);
}
