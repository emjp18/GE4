#pragma once
#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H
#include "pch.h"
#include "Window.h"
class RenderManager
{
private:
	const float m_DEFAULT_BG_COLOR[4] = { 0.6f, 1.0f, 0.2f,1.0f };
	const float m_mask[4] = { 0.0f,0.0f,0.0f,0.0f };
	D3D11_VIEWPORT m_viewport = {};
	bool m_initialized = false;
	WRL::ComPtr<ID3D11Device> m_device;
	WRL::ComPtr<ID3D11DeviceContext> m_context;
	WRL::ComPtr<ID3D11Debug> m_debug;
	WRL::ComPtr<IDXGISwapChain> m_swapchain;
	DXGI_MODE_DESC m_modeDesc = {};
	WRL::ComPtr<IDXGIFactory> m_factory = nullptr;
	WRL::ComPtr<IDXGIAdapter> m_adapter0 = nullptr;
	WRL::ComPtr<IDXGIFactory> m_factory1 = nullptr;
	WRL::ComPtr<IDXGIAdapter> m_adapter1 = nullptr;
	WRL::ComPtr<IDXGIAdapter4> m_adapterFour = nullptr;
	WRL::ComPtr<IDXGIDevice> m_idxgiDevice = nullptr;
	DXGI_QUERY_VIDEO_MEMORY_INFO m_vRamInfo;
	HANDLE m_hProcess;
	DXGI_MODE_DESC* m_supportedModes = nullptr;
	float m_ram;
	float m_vRAM;
	int m_currentModeIndex;
	UINT m_numberOfSupportedModes;
	bool m_vsync = true;
	WRL::ComPtr< ID3D11BlendState> m_blendOn = nullptr;
	WRL::ComPtr< ID3D11BlendState> m_blendOff = nullptr;
	WRL::ComPtr< ID3D11BlendState> m_blendParticle = nullptr;
	WRL::ComPtr<ID3D11RasterizerState> m_shadowRS = nullptr;
	WRL::ComPtr<ID3D11RasterizerState> m_mainRS = nullptr;
	WRL::ComPtr<ID3D11RasterizerState> m_cullNoneRS = nullptr;
	WRL::ComPtr<ID3D11RasterizerState> m_clockWiseRS = nullptr;
	WRL::ComPtr<ID3D11RasterizerState> m_wireframeRS = nullptr;
	WRL::ComPtr<ID3D11DepthStencilState> m_depthDisableDSS = nullptr;
	WRL::ComPtr<ID3D11DepthStencilState> m_mainDSS = nullptr;
	WRL::ComPtr<ID3D11DepthStencilView> m_mainDSV = nullptr;
	WRL::ComPtr<ID3D11RenderTargetView> m_mainRTV = nullptr;
public:
    RenderManager();
    void Startup();
    void ShutDown();
    // Prevents any type of copy or new instance
    RenderManager(const RenderManager&) = delete;
    void operator=(const RenderManager&) = delete;

    static RenderManager& Get()
    {
        static RenderManager instance;
        return instance;
    }
    ID3D11DeviceContext* getContext() { return m_context.Get(); }
    ID3D11Device* getDevice() { return m_device.Get(); }
    ID3D11Debug* getDebug() { return m_debug.Get(); }
    IDXGISwapChain* getswapChain() { return m_swapchain.Get(); }
    BOOL g_pastInFullscreen;
    BOOL g_currentlyInFullscreen;
	void Resize();
	void ChangeResolution();
	void Present();
	bool GetIntitialized() { return m_initialized; }
	void Render();
};
#endif
