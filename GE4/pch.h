// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include <wrl\client.h>
#include <stdio.h>  
#include <d3d11.h>
#include <assert.h> 
#include <dxgi.h>
#include <d3dcompiler.h>
#include <windowsx.h>
#include <DirectXMath.h>
#include <map>
#include <unordered_map>
#include <WICTextureLoader.h>
#include <chrono>
#include <wincodec.h>
#include <DDSTextureLoader.h>
#include <SimpleMath.h>
#include <queue>
#include <string>
#include <sstream>
#include <crtdbg.h>
#include <fstream>
#include <dxgi1_6.h>
#include <psapi.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <Audio.h>
#include <Keyboard.h>
#include <Mouse.h>

#pragma comment( lib, "user32" )     
#pragma comment( lib, "gdi32" )         
#pragma comment( lib, "d3d11" )        
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment(lib, "dxgi")


#define USE_IMGUI TRUE
#if USE_IMGUI
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_win32.h"
#include "Imgui\imgui_impl_dx11.h"
#endif

#include <GO_Header.h>

const size_t g_mediumResX = 800;
const size_t g_mediumResY = 600;
const size_t g_highResX = 1920;
const size_t g_highResY = 1080;
#endif //PCH_H

#define FBXSDK_SHARED
#define NOMINMAX

#ifndef UNICODE
#define UNICODE
#endif
#define RELEASE(ptr) if(ptr) { delete ptr; ptr = nullptr; }
#define RELEASE_ARR(ptrA) if(ptrA) { delete[] ptrA; ptrA = nullptr; }
#define RELEASE_COM(cptr) if(cptr) { cptr->Release(); cptr = nullptr; }
#define RELEASE_U_PTR(ptr) if(ptr.get()){ptr.release();}
#define _CRTDBG_MAP_ALLOC

#define ASSERT_SHADER(RESULT, eBLOB, wTEXT) \
		if (FAILED(RESULT)) { \
			MessageBox(NULL, wTEXT.c_str(), L"Error compiling shaders. Check output for more information.", MB_OK); \
			if(eBLOB){		\
				OutputDebugStringA((char*)eBLOB->GetBufferPointer()); \
				eBLOB->Release(); \
			}} \

#define FULLSCREEN FALSE

using namespace DirectX;
using namespace SimpleMath;
using namespace Microsoft;