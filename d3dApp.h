#pragma once

#include "d3dUtil.h"
#include "d3dUtility.h"
#include "FSMnStates.h"
#include <string>
#include "XInput.h"
#include "GfxStats.h"

class MainGameManager;
class ChimeraGameEngine;

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP,HWND windowHandle);
	virtual ~D3DApp();
	bool ClosedApp;

	HINSTANCE getAppInst();
	HWND      getMainWnd();

	// Framework methods.  Derived client class overrides these methods to
	// implement specific application requirements.
	virtual bool checkDeviceCaps()     { return true; }
	virtual void onLostDevice()        {}
	virtual void onResetDevice()       {}
	virtual void updateScene(float dt) {}
	virtual void drawScene()           {}
	virtual void DisplayStats()=0;
	virtual void initialize()		   {}
	virtual void leave()			   {}
	virtual int LeftButtonClicked()   {return 0;}
	virtual int activate( WPARAM wParam);
	virtual int exitSizeMove();
	virtual int Size(WPARAM wParam, LPARAM lParam);
	virtual stateMachine<MainGameManager>* GetFsm()=0;
	virtual ChimeraGameEngine* GetEngine()=0;
	virtual GfxStats* GetStats()=0;
	virtual void SwitchGameState(int stateID){}

	// Override these methods only if you do not like the default window creation,
	// direct3D device creation, window procedure, or message loop.  In general,
	// for the sample programs of this book, we will not need to modify these.
	virtual void initMainWindow();
	virtual void initDirect3D();
	virtual int Run();
	virtual int msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void enableFullScreenMode(bool enable);
	bool isDeviceLost();
	bool isAppPaused(){return mAppPaused;}

protected:
	// Derived client class can modify these data members in the constructor to
	// customize the application.
	std::string mMainWndCaption;
	D3DDEVTYPE  mDevType;
	DWORD       mRequestedVP;

	// Application, Windows, and Direct3D data members.
	HINSTANCE             mhAppInst;
	HWND                  mhMainWnd;
	IDirect3D9*           md3dObject;
	bool                  mAppPaused;
	D3DPRESENT_PARAMETERS md3dPP;
};

// Globals for convenient access.
extern D3DApp* gd3dApp;
extern IDirect3DDevice9* gd3dDevice;