#include "pch.h"
#include "RenderManager.h"


RenderManager::RenderManager()
{
}

void RenderManager::Startup()
{
	UINT swapchainFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	swapchainFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
	/*swapchainFlags |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT;*/
#ifdef  _DEBUG
	swapchainFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, swapchainFlags, NULL, 0, D3D11_SDK_VERSION, m_device.GetAddressOf(), featureLevel, m_context.GetAddressOf());
	assert(SUCCEEDED(hr));
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 2;
	scd.OutputWindow =Window::GetHWND();
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	IDXGIOutput* output = nullptr;
	hr = (m_device.Get())->QueryInterface(m_idxgiDevice.GetAddressOf());
	assert(SUCCEEDED(hr));
	hr = (m_idxgiDevice.Get())->GetAdapter(m_adapter0.GetAddressOf());
	assert(SUCCEEDED(hr));
	hr = (m_adapter0.Get())->GetParent(__uuidof(IDXGIFactory), (void**)m_factory.GetAddressOf());
	assert(SUCCEEDED(hr));
	hr = (m_factory.Get())->CreateSwapChain(m_device.Get(), &scd, m_swapchain.GetAddressOf());
	assert(SUCCEEDED(hr));
	m_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(m_debug.GetAddressOf()));
	hr = (m_swapchain.Get())->GetContainingOutput(&output);
	assert(SUCCEEDED(hr));
	hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &m_numberOfSupportedModes, NULL);
	assert(SUCCEEDED(hr));
	m_supportedModes = new DXGI_MODE_DESC[m_numberOfSupportedModes];
	ZeroMemory(m_supportedModes, sizeof(DXGI_MODE_DESC) * m_numberOfSupportedModes);
	hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &m_numberOfSupportedModes, m_supportedModes);
	assert(SUCCEEDED(hr));
	output->Release();
	bool supportedMode = false;
	for (unsigned int i = 0; i < m_numberOfSupportedModes; i++)
	{
		if ((UINT)Window::g_currResX == m_supportedModes[sizeof(DXGI_MODE_DESC)*i].Width &&
			(UINT)Window::g_currResY == m_supportedModes[sizeof(DXGI_MODE_DESC) * i].Height)
		{
			supportedMode = true;
			m_modeDesc = m_supportedModes[sizeof(DXGI_MODE_DESC) * i];
			m_currentModeIndex = i;
			break;
		}
	}
	if (!supportedMode)
	{
		m_currentModeIndex = 0;
		m_modeDesc = m_supportedModes[0];
	}
	ID3D11Texture2D* backBufferPtr = nullptr;
	HRESULT getBackbufferResult = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	assert(SUCCEEDED(getBackbufferResult));
	m_device->CreateRenderTargetView(backBufferPtr, nullptr, m_mainRTV.GetAddressOf());
	D3D11_TEXTURE2D_DESC bbTexDesc;
	backBufferPtr->GetDesc(&bbTexDesc);

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDsc = {};
	depthStencilStateDsc.DepthEnable = true;
	depthStencilStateDsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDsc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilStateDsc.StencilEnable = false;
	depthStencilStateDsc.StencilReadMask = 0xFF;
	depthStencilStateDsc.StencilWriteMask = 0xFF;
	depthStencilStateDsc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDsc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDsc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilStateDsc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilStateDsc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDsc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDsc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilStateDsc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HRESULT createDepthStencilResult = m_device->CreateDepthStencilState(&depthStencilStateDsc, m_mainDSS.GetAddressOf());
	assert(SUCCEEDED(createDepthStencilResult));
	m_context->OMSetDepthStencilState(m_mainDSS.Get(), 1);
	depthStencilStateDsc.DepthEnable = false;
	depthStencilStateDsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = m_device->CreateDepthStencilState(&depthStencilStateDsc, m_depthDisableDSS.GetAddressOf());
	assert(SUCCEEDED(hr));

	CD3D11_TEXTURE2D_DESC textureDesc = {};
	ID3D11Texture2D* textureD = nullptr;
	textureDesc.Width = (UINT)bbTexDesc.Width;
	textureDesc.Height = (UINT)bbTexDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	RELEASE_COM(backBufferPtr);
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = textureDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = m_device->CreateTexture2D(&textureDesc, NULL, &textureD);
	assert(SUCCEEDED(hr));
	hr = m_device->CreateDepthStencilView(textureD, &dsvDesc, m_mainDSV.GetAddressOf());
	assert(SUCCEEDED(hr));
	RELEASE_COM(textureD);
	D3D11_RASTERIZER_DESC rasterizerDescription = {};
	rasterizerDescription.CullMode = D3D11_CULL_BACK;
	rasterizerDescription.FillMode = D3D11_FILL_SOLID;
	rasterizerDescription.DepthClipEnable = true;
	rasterizerDescription.FrontCounterClockwise = true;
	hr = m_device->CreateRasterizerState(&rasterizerDescription, m_mainRS.GetAddressOf());
	assert(SUCCEEDED(hr));
	rasterizerDescription.CullMode = D3D11_CULL_NONE;
	hr = m_device->CreateRasterizerState(&rasterizerDescription, m_cullNoneRS.GetAddressOf());
	assert(SUCCEEDED(hr));
	rasterizerDescription.CullMode = D3D11_CULL_BACK;
	rasterizerDescription.FrontCounterClockwise = false;
	hr = m_device->CreateRasterizerState(&rasterizerDescription, m_clockWiseRS.GetAddressOf());
	assert(SUCCEEDED(hr));
	rasterizerDescription.FrontCounterClockwise = true;
	rasterizerDescription.FillMode = D3D11_FILL_WIREFRAME;
	hr = m_device->CreateRasterizerState(&rasterizerDescription, m_wireframeRS.GetAddressOf());
	assert(SUCCEEDED(hr));
	rasterizerDescription.CullMode = D3D11_CULL_BACK;
	rasterizerDescription.FillMode = D3D11_FILL_SOLID;
	rasterizerDescription.DepthClipEnable = true;
	rasterizerDescription.FrontCounterClockwise = true;
	rasterizerDescription.DepthBias = 112500;
	rasterizerDescription.DepthBiasClamp = 0.0f;
	rasterizerDescription.SlopeScaledDepthBias = 1.0f;
	hr = m_device->CreateRasterizerState(&rasterizerDescription, m_shadowRS.GetAddressOf());
	assert(SUCCEEDED(hr));
	D3D11_BLEND_DESC blendDescOn;
	ZeroMemory(&blendDescOn, sizeof(D3D11_BLEND_DESC));
	blendDescOn.AlphaToCoverageEnable = TRUE;
	blendDescOn.IndependentBlendEnable = TRUE;
	blendDescOn.RenderTarget[0].BlendEnable = TRUE;
	blendDescOn.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescOn.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescOn.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescOn.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescOn.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDescOn.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescOn.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	D3D11_BLEND_DESC blendDescOff;
	ZeroMemory(&blendDescOff, sizeof(D3D11_BLEND_DESC));
	blendDescOff.RenderTarget[0].BlendEnable = false;
	blendDescOff.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState(&blendDescOn, m_blendOn.GetAddressOf());
	assert(SUCCEEDED(hr));
	hr = m_device->CreateBlendState(&blendDescOff, m_blendOff.GetAddressOf());
	assert(SUCCEEDED(hr));
	blendDescOn.AlphaToCoverageEnable = true;
	blendDescOn.IndependentBlendEnable = false;	 // can be true 
	blendDescOn.RenderTarget[0].BlendEnable = true;
	blendDescOn.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescOn.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDescOn.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescOn.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDescOn.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDescOn.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescOn.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	hr = m_device->CreateBlendState(&blendDescOn, m_blendParticle.GetAddressOf());
	assert(SUCCEEDED(hr));
#if USE_IMGUI

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(Window::GetHWND());
	ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
	ImGui::StyleColorsDark();

#endif



#if _DEBUG
	WRL::ComPtr<ID3D11InfoQueue> d3dInfoQueue;
	hr = m_debug.As(&d3dInfoQueue);
	if (SUCCEEDED(hr))
	{
		D3D11_MESSAGE_ID hide[] =
		{

			D3D11_MESSAGE_ID::D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET

		};
		D3D11_INFO_QUEUE_FILTER filter;
		memset(&filter, 0, sizeof(filter));
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif
	HRESULT ret_code = ::CreateDXGIFactory(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(m_factory1.GetAddressOf()));
	if (SUCCEEDED(ret_code))
	{

		if (SUCCEEDED(m_factory1->EnumAdapters(0, m_adapter1.GetAddressOf())))
		{
			if (SUCCEEDED(m_adapter1->QueryInterface(__uuidof(IDXGIAdapter4), (void**)m_adapterFour.GetAddressOf())))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO info;

				if (SUCCEEDED(m_adapterFour->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					m_vRAM = float(info.CurrentUsage / 1024.0 / 1024.0); //MB

				}


			}

		}

	}
	DWORD currentProcessID = GetCurrentProcessId();

	m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, currentProcessID);

	PROCESS_MEMORY_COUNTERS pmc{};

	if (GetProcessMemoryInfo(m_hProcess, &pmc, sizeof(pmc)))
	{

		m_ram = float(pmc.PagefileUsage / 1024.0 / 1024.0); //MB
	}
#if FULLSCREEN
	resize(true);
	g_pastInFullscreen = 1;
#else
	g_pastInFullscreen = 0;
#endif

	m_initialized = true;
}

void RenderManager::ShutDown()
{
	RELEASE_ARR(m_supportedModes);
	CloseHandle(m_hProcess);
#if USE_IMGUI
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
#endif
}

void RenderManager::Resize()
{
	HRESULT hr;
	DXGI_MODE_DESC zeroRefreshRate = {};
	zeroRefreshRate = m_modeDesc;
	zeroRefreshRate.RefreshRate.Numerator = 0;
	zeroRefreshRate.RefreshRate.Denominator = 0;
	hr = m_swapchain->GetFullscreenState(&g_currentlyInFullscreen, nullptr);
	assert(SUCCEEDED(hr));
	if (g_currentlyInFullscreen != g_pastInFullscreen)
	{

		if (g_currentlyInFullscreen)
		{
			hr = m_swapchain->ResizeTarget(&zeroRefreshRate);
			assert(SUCCEEDED(hr));
			hr = m_swapchain->SetFullscreenState(true, nullptr);
			assert(SUCCEEDED(hr));
		}
		else
		{
			hr = m_swapchain->SetFullscreenState(false, nullptr);
			assert(SUCCEEDED(hr));

			RECT rect = { 0, 0, (long)m_modeDesc.Width,  (long)m_modeDesc.Height };
			AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
			SetWindowPos(Window::GetHWND(), HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);

		}
		g_pastInFullscreen = !g_pastInFullscreen;
	}
	hr = m_swapchain->ResizeTarget(&zeroRefreshRate);
	assert(SUCCEEDED(hr));
	UINT swapchainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	m_context->ClearState();
	m_mainRTV = nullptr;
	m_mainDSV = nullptr;
	hr = m_swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, swapchainFlags);
	assert(SUCCEEDED(hr));
	ID3D11Texture2D* backBufferPtr = nullptr;
	HRESULT getBackbufferResult = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	assert(SUCCEEDED(getBackbufferResult));
	m_device->CreateRenderTargetView(backBufferPtr, nullptr, m_mainRTV.GetAddressOf());
	D3D11_TEXTURE2D_DESC bbTexDesc = {};
	backBufferPtr->GetDesc(&bbTexDesc);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	ID3D11Texture2D* textureD = nullptr;
	textureDesc.Width = (UINT)bbTexDesc.Width;
	textureDesc.Height = (UINT)bbTexDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = textureDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = (float)Window::g_currResX;
	m_viewport.Height = (float)Window::g_currResY;
	m_viewport.MaxDepth = 1;
	m_viewport.MinDepth = 0;
	RELEASE_COM(backBufferPtr);
	hr = m_device->CreateTexture2D(&textureDesc, NULL, &textureD);
	assert(SUCCEEDED(hr));
	hr = m_device->CreateDepthStencilView(textureD, &dsvDesc, m_mainDSV.GetAddressOf());
	assert(SUCCEEDED(hr));
	RELEASE_COM(textureD);
	m_context->OMSetRenderTargets(1, m_mainRTV.GetAddressOf(), m_mainDSV.Get());
	m_context->RSSetViewports(1, &m_viewport);
}

void RenderManager::ChangeResolution()
{
	bool supportedMode = false;
	for (unsigned int i = 0; i < m_numberOfSupportedModes; i++)
	{
		if ((UINT)Window::g_currResX == m_supportedModes[sizeof(DXGI_MODE_DESC)*i].Width &&
			(UINT)Window::g_currResY == m_supportedModes[sizeof(DXGI_MODE_DESC) * i].Height)
		{

			supportedMode = true;
			m_modeDesc = m_supportedModes[i];
			m_currentModeIndex = i;
			break;
		}
	}
	if (!supportedMode)
	{
		if (Window::g_oldResX > Window::g_currResX)
		{
			if (m_currentModeIndex < (int)m_numberOfSupportedModes - 1)
			{

				m_currentModeIndex++;
			}
		}

		if (Window::g_oldResX < Window::g_currResX)
		{

			if (m_currentModeIndex > 0)
			{

				m_currentModeIndex--;
			}
		}

		m_modeDesc = m_supportedModes[m_currentModeIndex];
	}
	Window::g_oldResX = Window::g_currResX;
	Window::g_oldResY = Window::g_currResY;
	Resize();
}

void RenderManager::Present()
{
	if (m_vsync)
	{
		HRESULT hr = m_swapchain->Present(1, 0);
		assert(SUCCEEDED(hr));


	}
	else
	{
		HRESULT hr = m_swapchain->Present(0, 0);
		assert(SUCCEEDED(hr));
	}
}

void RenderManager::Render()
{
	
	m_context->RSSetViewports(1, &m_viewport);
	m_context->OMSetRenderTargets(1, m_mainRTV.GetAddressOf(), m_mainDSV.Get());
	m_context->ClearRenderTargetView(m_mainRTV.Get(), m_DEFAULT_BG_COLOR);
	m_context->ClearDepthStencilView(m_mainDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_context->OMSetDepthStencilState(m_mainDSS.Get(), 1);
	m_context->OMSetBlendState(m_blendOff.Get(), m_mask, 0xffffffff);
}
