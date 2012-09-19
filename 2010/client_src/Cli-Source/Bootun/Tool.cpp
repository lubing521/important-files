#include "stdafx.h"
#include <comutil.h>
#include "tool.h"

namespace i8desk
{

	DWORD SDG_GetServerIp(BOOL bFind)
	{
		CHAR    szFileName[MAX_PATH] = {0};
		SDG_GetAppPath(szFileName, TRUE);

		lstrcat(szFileName, "gameclient.ini");

 		char ip[MAX_PATH] = {0};
		int ret = 0;
		if (!bFind)
			ret = GetPrivateProfileString("SystemSet", "ServerAddr", "", ip, MAX_PATH, szFileName);

		if (ret == 0 || lstrcmp(ip, "0.0.0.0") == 0)
		{
			//create status report broadcast socket.
			SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
			if (sck == INVALID_SOCKET)	
				return inet_addr(ip);
			sockaddr_in server;
			ZeroMemory(&server, sizeof(server));
			server.sin_family = AF_INET;
			server.sin_port = htons(7918);
			server.sin_addr.s_addr = INADDR_BROADCAST;

			BOOL bBroadcast = TRUE;
			if (SOCKET_ERROR == setsockopt(sck, SOL_SOCKET, SO_BROADCAST, (char*)&bBroadcast, sizeof(BOOL)))
			{
				closesocket(sck);
				return inet_addr(ip);
			}

			char data[1024] = {0};
			CPackageHelper outpackage(data);
			_packageheader header = {0};
			header.StartFlag = 0x5e7d;
			header.Cmd = 0x0501;
			header.Version = SDG_VERSION;
			header.Length = sizeof(_packageheader);
			outpackage.pushPackageHeader(header);
			sendto(sck, data, header.Length, 0, (PSOCKADDR)&server, sizeof(server));

			FD_SET fdset;
			timeval tv = {10, 0};
			FD_ZERO(&fdset);
			FD_SET(sck, &fdset);
			int ret = select(0, &fdset, NULL, NULL, &tv);
			if (ret >0 && recv(sck, data, 1024, 0))
			{
				CPackageHelper inpackage(data);
				if (inpackage.popDWORD() == 0)
					inpackage.popString(ip);
			}
			closesocket(sck);
			return inet_addr(ip);		
		}
		return inet_addr(ip);
	}
	void  SDG_GetAppPath(LPSTR lpszPath, BOOL bAddBackslash)
	{
		GetModuleFileName(NULL, lpszPath, MAX_PATH);
		PathRemoveFileSpec(lpszPath);
		if (bAddBackslash)
			PathAddBackslash(lpszPath);
		else
			PathRemoveBackslash(lpszPath);
	}

	void  SDG_SleepEx(char chDriver, DWORD dwSecond)
	{
		char szFileName[MAX_PATH] = {0};
		DeleteFile(szFileName);

		Sleep(dwSecond);

		DWORD dwTime = GetTickCount();

		HANDLE hFile = INVALID_HANDLE_VALUE;
		while (hFile != INVALID_HANDLE_VALUE)
		{
			hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
			Sleep(1000);
		}
		DWORD dwWriteBytes = 0;
		WriteFile(hFile, szFileName, strlen(szFileName), &dwWriteBytes, NULL);
		CloseHandle(hFile);
		DeleteFile(szFileName);	
		Sleep(dwSecond);
	}

	void  SDG_RebootComputer()
	{
		HANDLE hToken; 
		TOKEN_PRIVILEGES tkp; 

		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
			return ; 

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 

		tkp.PrivilegeCount = 1; 
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 	
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 	

		Sleep(1000);	

		ExitWindowsEx(EWX_REBOOT|EWX_FORCE, 0);
	}

	BOOL SDG_RunExe(string strfile,string strPara)
	{
		HINSTANCE hProcessSnap = NULL;
		PROCESSENTRY32 pe32 = {0};
		hProcessSnap = (HINSTANCE) ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hProcessSnap == (HANDLE) -1) return false;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		DWORD flag = 0;
		string theProcssName = "DBA_BJLDNW.exe";
		if(::Process32First(hProcessSnap,&pe32))
		{
			do
			{
				if (stricmp(theProcssName.c_str(),pe32.szExeFile)==0)
				{
					flag = 1;
					break;
				}
			}
			while(Process32Next(hProcessSnap,&pe32));
		}
		::CloseHandle(hProcessSnap);
		if(!flag)
		{ //如果没有找到
			HDESK hdeskCurrent;
			HDESK hdesk;
			HWINSTA hwinstaCurrent;
			HWINSTA hwinsta;
			hwinstaCurrent = GetProcessWindowStation();
			if (hwinstaCurrent == NULL)
			{
//				LogEvent(_T("get window station err"));
				return false;
			}
			hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
			if (hdeskCurrent == NULL)
			{
//				LogEvent(_T("get window desktop err"));
				return false;
			}
			//打开用户的winsta0
			hwinsta = OpenWindowStation("winsta0", FALSE, 
						WINSTA_ACCESSCLIPBOARD |
						WINSTA_ACCESSGLOBALATOMS |
						WINSTA_CREATEDESKTOP |
						WINSTA_ENUMDESKTOPS |
						WINSTA_ENUMERATE |
						WINSTA_EXITWINDOWS |
						WINSTA_READATTRIBUTES |
						WINSTA_READSCREEN |
						WINSTA_WRITEATTRIBUTES);
			if (hwinsta == NULL)
			{
//				LogEvent(_T("open window station err"));
				return false;
			}
			if (!SetProcessWindowStation(hwinsta))
			{
//				LogEvent(_T("Set window station err"));
				return false;
			}
			//打开desktop
			hdesk = OpenDesktop("default", 0, FALSE, 
					DESKTOP_CREATEMENU |
					DESKTOP_CREATEWINDOW |
					DESKTOP_ENUMERATE |
					DESKTOP_HOOKCONTROL |
					DESKTOP_JOURNALPLAYBACK |
					DESKTOP_JOURNALRECORD |
					DESKTOP_READOBJECTS |
					DESKTOP_SWITCHDESKTOP |
					DESKTOP_WRITEOBJECTS);
			if (hdesk == NULL)
			{
//				LogEvent(_T("Open desktop err"));
				return false;
			}
			SetThreadDesktop(hdesk); 
			::ShellExecute(0,NULL,strfile.c_str(),strPara.c_str(),NULL,SW_MAXIMIZE);
			string msg = "DBA_BJLDNW.exe 被重启动";
//			LogEvent(msg);
			SetProcessWindowStation(hwinstaCurrent);
			SetThreadDesktop(hdeskCurrent);
			CloseWindowStation(hwinsta);
			CloseDesktop(hdesk);
		}
// 		char szFile[MAX_PATH] = {0};
// 		char szBase[MAX_PATH] = {0};
// 		SDG_GetAppPath(szBase, TRUE);
// 		_stprintf(szFile, "%s%s%s", szBase, info.path, info.file);			
// 		ShellExecute(NULL, "open", szFile, NULL, NULL, SW_SHOWNORMAL);
		return TRUE;
	}

	BOOL  SDG_StartService(file_info& info)
	{
		char szServiceName[] = "NetMgrClient";

		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		DWORD dwErrorCode = ERROR_SUCCESS;

		__try 
		{
			// Open the SCM database
			hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
			if (!hSCM) 
			{
				dwErrorCode = GetLastError();
				__leave;
			}

			// Open the specified service
			hService = OpenService(hSCM, szServiceName, SERVICE_START | SERVICE_QUERY_STATUS);
			if (!hService) 
			{
				dwErrorCode = GetLastError();
				__leave;
			}

			SERVICE_STATUS ss;
			// Check the status until the service is no longer start pending. 
			if (!QueryServiceStatus(hService,&ss))
			{
				dwErrorCode = GetLastError();
				__leave;
			}

			if (ss.dwCurrentState == SERVICE_RUNNING)
			{
				dwErrorCode = ERROR_SUCCESS;
				__leave;
			}

			if (ss.dwCurrentState != SERVICE_STOPPED)
			{
				dwErrorCode = E_UNEXPECTED;
				__leave;
			}

			if (!::StartService(hService, 0, NULL))
			{
				dwErrorCode = GetLastError();
				__leave;
			}		
		}
		__finally 
		{
			if (hService)
				CloseServiceHandle(hService);

			if (hSCM)
				CloseServiceHandle(hSCM);
		}
		Sleep(1000);
		return dwErrorCode == ERROR_SUCCESS;
	}

	BOOL  SDG_StopService(file_info& info)
	{
		char szServiceName[] = "NetMgrClient";
		info.needreboot = TRUE;

		BOOL ret = FALSE;
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		DWORD dwErrorCode = ERROR_SUCCESS;
		try 
		{
			hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
			if (hSCM == NULL) 
				throw 1;
			hService = OpenService(hSCM, szServiceName, 
				SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
			if (hService == NULL) 
				throw 1;

			SERVICE_STATUS ss;
			ss.dwWaitHint = 1000;
			if (!QueryServiceStatus(hService,&ss))
				throw 1;

			if (ss.dwCurrentState == SERVICE_STOP)
			{
				ret = TRUE;
			}
			else if( ss.dwCurrentState == SERVICE_RUNNING)
			{
				// 停止服务
				if( ::ControlService( hService,	SERVICE_CONTROL_STOP, &ss) == FALSE)
				{				
					throw 1;
				}
				// 等待服务停止
				while( ::QueryServiceStatus( hService, &ss) == TRUE)
				{
					::Sleep( ss.dwWaitHint);
					if( ss.dwCurrentState == SERVICE_STOPPED)
					{
						ret = TRUE;
						break;
					}
				}
			}
		}
		catch (...)	{ ret = FALSE; }	

		if (hService != NULL)
			CloseServiceHandle(hService);

		if (hSCM != NULL)
			CloseServiceHandle(hSCM);	

		Sleep(1000);
		return ret;
	}

	BOOL  SDG_KillProcess(file_info& info)
	{	
		info.needreboot = TRUE;

		HANDLE			hProcessSnap	= NULL;
		PROCESSENTRY32	pe32			= {0};	
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return FALSE;

		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				if (_tcsicmp(info.file, pe32.szExeFile) == 0)
				{
					BOOL ret = FALSE;
					HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, pe32.th32ProcessID);
					if (hProcess != NULL)
						ret = TerminateProcess(hProcess, 0);
					CloseHandle(hProcessSnap);
					Sleep(2000);
					return ret;
				}
			}
			while (Process32Next(hProcessSnap, &pe32));
		}
		CloseHandle(hProcessSnap);	
		return FALSE;
	}

	void TrimString(std::string& str)
	{
		if (str.size() == 0)
			return ;

		//trime left.
		std::string::iterator it;
		for (it = str.begin(); it != str.end(); it++)
		{
			if (!isspace(*it))
				break;
		}
		if (it != str.begin())
			str.erase(str.begin(), it);

		//trime right.
		for (it = str.end()-1; it != str.begin(); it--)
		{
			if (!isspace(*it))
				break;
		}

		if (it != str.end()-1)
			str.erase(it, str.end());
	}

	BOOL  SDG_DeleteDirectory(LPCSTR lpszDir,__int64* DelSize, LPCSTR pExcludedir/* = NULL*/)
	{
		//parse excludedir list.
		vector<string> dirlist;
		if (pExcludedir != NULL && lstrlen(pExcludedir) != 0)
		{
			splitString(string(pExcludedir), dirlist, '|');
		}
		
		string path(lpszDir);
		path += "*.*";

		WIN32_FIND_DATA wfd = {0};
		HANDLE hFinder = FindFirstFile(path.c_str(), &wfd);
		if (hFinder == INVALID_HANDLE_VALUE)
			return FALSE;
		do 
		{
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				string file(lpszDir);
				file+= wfd.cFileName;
				SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
				*DelSize = *DelSize +  (((__int64)wfd.nFileSizeHigh)<<32)+wfd.nFileSizeLow;
				DeleteFile(file.c_str());
			}
			else
			{
				if (lstrcmp(wfd.cFileName, ".") != 0 && lstrcmp(wfd.cFileName, "..") != 0)
				{
					string dir(lpszDir);
					dir += wfd.cFileName;
					dir += "\\";


					bool bDo = true;
					for (vector<string>::iterator it=dirlist.begin(); it!=dirlist.end(); it++)
					{
						string excludedir = *it;
						TrimString(excludedir);
						if (excludedir.size() && *excludedir.rbegin() != '\\')
							excludedir += "\\";

						if (strcmpi(dir.c_str(), excludedir.c_str()) == 0)
						{
							bDo = false;
							break;
						}
					}
					if (bDo)
						SDG_DeleteDirectory(dir.c_str(),DelSize, pExcludedir);
				}
			}
		} while(FindNextFile(hFinder, &wfd) != 0);
		FindClose(hFinder);

		RemoveDirectory(lpszDir);

		return TRUE;
	}

	BOOL SDG_CreateDirectory(LPCSTR lpszDir, int offset)
	{
		string szPathName(lpszDir);
		int end = szPathName.rfind('\\');
		int pt = szPathName.find('\\', offset);

		if (szPathName[pt-1] == ':')
			pt = szPathName.find('\\', pt+1);
		string path;
		DWORD dwRet = 0;
		while(pt != -1 && pt<=end)		
		{		
			path = szPathName.substr(0, pt);
			dwRet = GetFileAttributes(path.c_str());
			if(dwRet == 0xffffffff)			
				::CreateDirectory(path.c_str(), NULL);		
			pt = szPathName.find('\\', pt+1);		
		}
		return TRUE;
	}

	bool SDG_CompareSysTime(SYSTEMTIME& t1, SYSTEMTIME& t2)
	{
		return ( (t1.wYear == t2.wYear) && 
			(t1.wMonth == t2.wMonth) &&
			(t1.wDay == t2.wDay) &&
			(t1.wHour == t2.wHour) &&
			(t1.wMinute == t2.wMinute) &&

			//(t1.wSecond == t2.wSecond)	
			//--- >
			(abs((t1.wSecond - t2.wSecond)) < 20)
			);
	}

	void SDG_WriteServerIp(char* ip)
	{
		char szFileName[MAX_PATH] = {0};
		SDG_GetAppPath(szFileName, TRUE);
		lstrcat(szFileName, "gameclient.ini");

		WritePrivateProfileString("SystemSet", "ServerAddr", ip, szFileName);
	}

	BOOL SDG_GetLastWriteTime(char* szFileName, SYSTEMTIME& lastWrite, BOOL isgmt)
	{
		FILETIME ftWrite;
		SYSTEMTIME stUTC;

		HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 
			FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		if (!GetFileTime(hFile, NULL, NULL, &ftWrite))
		{
			CloseHandle(hFile);
			return FALSE;
		}

		if (isgmt)
			FileTimeToSystemTime(&ftWrite, &lastWrite);
		else
		{
			FileTimeToSystemTime(&ftWrite, &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &lastWrite);
		}

		CloseHandle(hFile);
		return TRUE;
	}

	void WriteI8deskProtocol()
	{
		#define I8DESK_PROT	"URL:i8desk Protocol"
		#define I8DESK_VER	"0.1"
		#define I8DESK_ICON	""

		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk", "",  REG_SZ, I8DESK_PROT, lstrlen(I8DESK_PROT));
		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk", "@", REG_SZ, I8DESK_PROT, lstrlen(I8DESK_PROT));
		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk", "Version", REG_SZ, I8DESK_VER, lstrlen(I8DESK_VER));
		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk", "URL Protocol", REG_SZ, "", 0);

		char buf[MAX_PATH] = {0};
		SDG_GetAppPath(buf, TRUE);
		lstrcatA(buf, "BarOnline.exe");
		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk\\DefaultIcon", "", REG_SZ, buf, lstrlen(buf));
		lstrcatA(buf, " \"%1\"");
		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk\\Shell\\Open\\Command", "", REG_SZ, buf, lstrlen(buf));
	}
	BOOL EnableDebugPrivilege() 
	{
		BOOL fOk = FALSE; 
		HANDLE hToken; 

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) 
		{
			TOKEN_PRIVILEGES tp; 
			tp.PrivilegeCount = 1; 
			LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid); 
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL); 
			fOk = (GetLastError() == ERROR_SUCCESS); 
			CloseHandle(hToken); 
		} 
		return fOk; 
	}

	void splitString(string strSource, vector<string>& plugs, char ch)
	{
		size_t first = 0;
		size_t second = 0;
		do
		{
			second = strSource.find_first_of(ch, first);
			if (second != string::npos)
			{
				plugs.push_back(strSource.substr(first, second-first));
				first = second+1;
			}
			else
			{
				if (first< strSource.size())
				{
					plugs.push_back(strSource.substr(first));
				}
			}
		}while (second != string::npos);
	}
}