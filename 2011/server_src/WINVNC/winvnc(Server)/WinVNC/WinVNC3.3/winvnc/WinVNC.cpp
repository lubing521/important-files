//  Copyright (C) 1999 AT&T Laboratories Cambridge. All Rights Reserved.
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


// WinVNC.cpp

// 24/11/97		WEZ

// WinMain and main WndProc for the new version of WinVNC

////////////////////////////
// System headers
#include "stdhdrs.h"

////////////////////////////
// Custom headers
#include "VSocket.h"
#include "WinVNC.h"

#include "vncServer.h"
#include "vncMenu.h"
#include "vncInstHandler.h"
#include "vncService.h"
#include "vncPasswd.h"
#include "Shlwapi.h"

#pragma  comment(lib,"Shlwapi.lib")
// Application instance and name
HINSTANCE	hAppInstance;
const char	*szAppName = "WinVNC";
DWORD		mainthreadId;
HANDLE		g_thread = NULL;
UINT       g_ThreadID= 0 ;
// WinMain parses the command line and either calls the main App
// routine or, under NT, the main service routine.

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason,  LPVOID lpvReserved  )		

{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		{	
		
#ifdef _DEBUG
			{
				// Get current flag
				int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

				// Turn on leak-checking bit
				tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

				// Set flag to the new value
				_CrtSetDbgFlag( tmpFlag );
			}
#endif

			hAppInstance = hinstDLL;
			mainthreadId = GetCurrentThreadId();
	

		}
		break;
	case DLL_PROCESS_DETACH:
		{	
		
		}
		break;
	}
	return TRUE;

}
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
	// Save the application instance and main thread id
{

#ifdef _DEBUG
	{
		// Get current flag
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

		// Turn on leak-checking bit
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

		// Set flag to the new value
		_CrtSetDbgFlag( tmpFlag );
	}
#endif

	hAppInstance = hInstance;
	mainthreadId = GetCurrentThreadId();
	g_hExit = CreateEvent(NULL,TRUE,FALSE,NULL);
	// Initialise the VSocket system
	VSocketSystem socksys;
	if (!socksys.Initialised())
	{
		MessageBox(NULL, "Failed to initialise the socket system", szAppName, MB_OK);
		return 0;
	}
	log.Print(LL_STATE, VNCLOG("sockets initialised\r\n"));


return WinVNCAppMain();
}
*/
	
// This is the main routine for WinVNC when running as an application
// (under Windows 95 or Windows NT)
// Under NT, WinVNC can also run as a service.  The WinVNCServerMain routine,
// defined in the vncService header, is used instead when running as a service.

int WinVNCAppMain()
{
	//{	__asm int 3	}
	// Set this process to be the last application to be shut down.
	SetProcessShutdownParameters(0x100, 0);
	
	// Check for previous instances of WinVNC!
	vncInstHandler instancehan;
	if (!instancehan.Init())
	{
		// We don't allow multiple instances!
		MessageBox(NULL, "Another instance of WinVNC is already running", szAppName, MB_OK);
		return 0;
	}

	// CREATE SERVER
	vncServer server;
//	g_psever =  new vncServer;
	// Set the name and port number
	server.SetName(szAppName);
	log.Print(LL_STATE, VNCLOG("server created ok\r\n"));


	// Create tray icon & menu if we're running as an app
	vncMenu *menu = new vncMenu(&server);
	if (menu == NULL)
	{
		log.Print(LL_INTERR, VNCLOG("failed to create tray menu\r\n"));
		PostQuitMessage(0);
	}


	// Now enter the message handling loop until told to quit!
	MSG msg;
	while (GetMessage(&msg, NULL, 0,0)) 
	{
		log.Print(LL_INTINFO, VNCLOG("message %d recieved\r\n"), msg.message);

		TranslateMessage(&msg);  // convert key ups and downs to chars
		DispatchMessage(&msg);
	}
//	WaitForSingleObject(g_hExit,INFINITE);
// 
// 	log.Print(LL_STATE, VNCLOG("shutting down server\n"));

	if (menu != NULL)
		delete menu;

	//return msg.wParam;
	return 1;
};
UINT _stdcall StartThread(LPVOID lparam)
{
	VSocketSystem socksys;
	if (!socksys.Initialised())
	{
		MessageBox(NULL, "Failed to initialise the socket system", szAppName, MB_OK);
		return 0;
	}
	log.Print(LL_STATE, VNCLOG("sockets initialised\r\n"));

	return WinVNCAppMain();

}
extern "C" __declspec(dllexport) BOOL WINAPI StartVNC()
{
	g_thread = (HANDLE)_beginthreadex(NULL,0,StartThread,NULL,0,&g_ThreadID);
	return g_thread != NULL;
}
 extern "C"  __declspec(dllexport)  BOOL WINAPI StopVNC()
{
	while(!PostThreadMessage(g_ThreadID,WM_QUIT,0,0))
	{
		Sleep(10);
	}
	
	
	if(g_thread != NULL)
	{
			WaitForSingleObject(g_thread,INFINITE);
			CloseHandle(g_thread);
			g_thread = NULL;
	}

	return TRUE;
}

