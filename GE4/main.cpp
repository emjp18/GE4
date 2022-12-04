// GE4.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "GE4.h"

#include "Engine.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	
	/*_crtBreakAlloc = 242;*/ 	//Use this to find memory leaks if present, set it equal to the leak number.
	Engine engine(hInstance);
	engine.Run();
	
	

#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return 0;
}
