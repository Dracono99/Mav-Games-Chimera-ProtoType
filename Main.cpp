// this is a proto type for my chimera game engine
// will need dod and mt  in final version
// this prototype will be single threaded primarily

// Include the windows header file, this has all the
// Win32 API structures, types, and function declarations
// we need to program Windows.
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include "d3dApp.h"
#include "RawInput.h"
#include "MainGameManager.h"
#include <vld.h>

//--- CRT debug header (must be last)
#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#pragma warning(disable : 4345) //this warning is when using empty () when allocating a new object. Only happens when allocating memory with this debug option, so disabling it
#endif
//#pragma warning(1: 4710)// should cause inlineing things to be shown
// The main window handle.  This is used to identify
// the main window we are going to create.
HWND MainWindowHandle = 0;

// Wraps the code necessary to initialize a windows
// application.  Function returns true if initialization
// was successful, else it returns false.
bool InitWindowsApp(HINSTANCE instanceHandle, int show);

// Wraps the message loop code.
int  Run();

// The window procedure, handles events our window
// receives.
LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT msg,
						 WPARAM wParam,
						 LPARAM lParam);

// Windows equivalant to main()
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   PSTR      pCmdLine,
				   int       nShowCmd)
{
	// got this big weird thing from mantis close program via normal main window or we get mem leaks
	//AllocConsole();

	//HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	//int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
	//FILE* hf_out = _fdopen(hCrt, "w");
	//setvbuf(hf_out, NULL, _IONBF, 1);
	//*stdout = *hf_out;

	//HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	//hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
	//FILE* hf_in = _fdopen(hCrt, "r");
	//setvbuf(hf_in, NULL, _IONBF, 128);
	//*stdin = *hf_in;

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(13258); //break on a specific memory allocation
#endif

	// First we create and initialize our Windows
	// application.  Notice we pass the application
	// hInstance and the nShowCmd from WinMain as
	// parameters.
	RII->instance();
	if(!InitWindowsApp(hInstance, nShowCmd))
	{
		::MessageBoxW(0,  L"Init - Failed", L"Error", MB_OK);
		return 0;
	}
	//gd3dApp = new MainDemoManager(hInstance,MainWindowHandle,D3DDEVTYPE_HAL,D3DCREATE_HARDWARE_VERTEXPROCESSING);
	//gd3dApp = new GameManager(hInstance,MainWindowHandle,D3DDEVTYPE_HAL,D3DCREATE_HARDWARE_VERTEXPROCESSING);
	// Once our application has been created and
	// initialized we enter the message loop.  We
	// stay in the message loop until a WM_QUIT
	// mesage is received, indicating the application
	// should be terminated.
	//return Run(); // enter message loop
	return gd3dApp->Run();
}

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	// The first task to creating a window is to describe
	// its characteristics by filling out a WNDCLASS
	// structure.
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = instanceHandle;
	wc.hIcon         = ::LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = ::LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "Chimera Game Engine Demo";

	// Then we register this window class description
	// with Windows so that we can create a window based
	// on that description.
	if(!::RegisterClass(&wc))
	{
		::MessageBox(0, "RegisterClass - Failed", 0, 0);
		return false;
	}
	// With our window class description registered, we
	// can create a window with the CreateWindow function.
	// Note, this function returns a HWND to the created
	// window, which we save in MainWindowHandle.  Through
	// MainWindowHandle we can reference this particular
	// window we are creating.
	MainWindowHandle = ::CreateWindow(
		"Chimera Game Engine Demo",
		"Chimera Game Engine Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		instanceHandle,
		0);
	if(MainWindowHandle == 0)
	{
		::MessageBox(0, "CreateWindow - Failed", 0, 0);
		return false;
	}
	gd3dApp = new MainGameManager(instanceHandle,MainWindowHandle,D3DDEVTYPE_HAL,D3DCREATE_HARDWARE_VERTEXPROCESSING);

	// Finally we show and update the window we just created.
	// Observe we pass MainWindowHandle to these functions so
	// that these functions know what particular window to
	// show and update.
	::ShowWindow(MainWindowHandle, show);
	::UpdateWindow(MainWindowHandle);
	return true;
}

int Run()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	// Loop until we get a WM_QUIT message.  The
	// function GetMessage will onlybut it return 0 (false)
	// when a WM_QUIT message is received, which
	// effectively exits the loop.
	while(::GetMessage(&msg, 0, 0, 0) )
	{
		// Translate the message, and then dispatch it
		// to the appropriate window procedure.
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND   windowHandle,
						 UINT   msg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	//msgProc(msg,wParam,lParam);
	// Handle some specific messages:
	switch( msg )
	{
	case WM_INPUT:
		{
			RII->GetRawInput(lParam);
			return 0;
		}
	case WM_ACTIVATE:
		{
			gd3dApp->activate(wParam);
			return 0;
		}
		// WM_SIZE is sent when the user resizes the window.
	case WM_SIZE:
		{
			gd3dApp->Size(wParam,lParam);
			return 0;
		}
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		{
			gd3dApp->exitSizeMove();
			return 0;
		}
		// In the case the left mouse button was pressed,
		// then display a message box.
	case WM_LBUTTONDOWN:
		{
			//LeftButtonClicked();
			return 0;
		}
		// WM_CLOSE is sent when the user presses the 'X' button in the
		// caption bar menu.
	case WM_CLOSE:
		{
			DestroyWindow(MainWindowHandle);
			return 0;
		}
	case WM_KEYDOWN:
		{
			if( wParam == VK_ESCAPE )
			{
				gd3dApp->enableFullScreenMode(false);
			}
			else if( wParam == 'F' )
			{
				gd3dApp->enableFullScreenMode(true);
			}
			return 0;
		}
		// In the case of a destroy message, then
		// send a quit message, which will terminate
		// the message loop.
	case WM_DESTROY:
		{
			::PostQuitMessage(0);
			delete gd3dApp;
			//_CrtDumpMemoryLeaks();
			return 0;
		}
	}
	// Forward any other messages we didn't handle
	// above to the default window procedure.
	return ::DefWindowProc(windowHandle,
		msg,
		wParam,
		lParam);
}