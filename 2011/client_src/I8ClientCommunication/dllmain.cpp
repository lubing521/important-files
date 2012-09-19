// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "I8ClientCommunication_i.h"
#include "dllmain.h"

CI8ClientCommunicationModule _AtlModule;

// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
