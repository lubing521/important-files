//////////////预处理
#ifndef WINVER                          // 指定要求的最低平台是 Windows Vista。
#define WINVER 0x0600           // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows Vista。
#define _WIN32_WINNT 0x0600     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINDOWS          // 指定要求的最低平台是 Windows 98。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以适用于 Windows Me 或更高版本。
#endif

#ifndef _WIN32_IE                       // 指定要求的最低平台是 Internet Explorer 7.0。
#define _WIN32_IE 0x0700        // 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料


#include <windows.h>
#include <WinInet.h>
#include <ShlObj.h>

#include <Shlwapi.h>
#include <process.h>
#include <time.h>
#include <Userenv.h>
#include <winioctl.h>
#include <tchar.h>
#include <fstream>
#include <wbemidl.h>
#include <Userenv.h>
#include <Psapi.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlstr.h>
#include <dshow.h>
#include <comutil.h>
#pragma comment(lib, "strmiids.lib ")
#pragma comment(lib, "comsuppw.lib")
#pragma warning(disable:4200)
#pragma comment(lib, "version.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Wbemuuid") 
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")

#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
////////////////////
#include <comutil.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include <process.h>
#include <time.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <WinSock2.h>
#include <Tlhelp32.h>
#include <ShellAPI.h>
#include <DbgHelp.h>

#pragma comment(lib, "shell32.lib")
#pragma	comment(lib, "dbghelp.lib")
#include "cltsvrdef.h"
#include "tool.h"
#include "Utility/SmartHandle.hpp"
#include "Utility/Utility.h"


namespace i8desk
{
	void  SDG_GetAppPath(LPTSTR lpszPath, BOOL bAddBackslash)
	{
		GetModuleFileName(NULL, lpszPath, MAX_PATH);
		PathRemoveFileSpec(lpszPath);
		if (bAddBackslash)
			PathAddBackslash(lpszPath);
		else
			PathRemoveBackslash(lpszPath);
	}
	void  SDG_GetAppPathA(LPSTR lpszPath, BOOL bAddBackslash)
	{
		GetModuleFileNameA(NULL, lpszPath, MAX_PATH);
		PathRemoveFileSpecA(lpszPath);
		if (bAddBackslash)
			PathAddBackslashA(lpszPath);
		else
			PathRemoveBackslashA(lpszPath);
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

	void TrimString(stdex::tString& str)
	{
		if (str.size() == 0)
			return ;

		//trime left.
		stdex::tString::iterator it;
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

	BOOL  SDG_DeleteDirectory(LPCTSTR lpszDir,__int64* DelSize, LPCTSTR pExcludedir/* = NULL*/)
	{
		//parse excludedir list.
		vector<stdex::tString> dirlist;
		if (pExcludedir != NULL && lstrlen(pExcludedir) != 0)
		{
			splitString(stdex::tString(pExcludedir), dirlist, '|');
		}
		
		stdex::tString path(lpszDir);
		path += TEXT("*.*");

		WIN32_FIND_DATA wfd = {0};
		HANDLE hFinder = FindFirstFile(path.c_str(), &wfd);
		if (hFinder == INVALID_HANDLE_VALUE)
			return FALSE;
		do 
		{
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				stdex::tString file(lpszDir);
				file+= wfd.cFileName;
				SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
                if (DelSize != NULL)
                {
				    *DelSize = *DelSize +  (((__int64)wfd.nFileSizeHigh)<<32)+wfd.nFileSizeLow;
                }
				DeleteFile(file.c_str());
			}
			else
			{
				if (lstrcmp(wfd.cFileName, TEXT(".")) != 0 && lstrcmp(wfd.cFileName, TEXT("..")) != 0)
				{
					stdex::tString dir(lpszDir);
					dir += wfd.cFileName;
					dir += TEXT("\\");


					bool bDo = true;
					for (vector<stdex::tString>::iterator it=dirlist.begin(); it!=dirlist.end(); it++)
					{
						stdex::tString excludedir = *it;
						TrimString(excludedir);
						if (excludedir.size() && *excludedir.rbegin() != '\\')
							excludedir += TEXT("\\");

						if (lstrcmp(dir.c_str(), excludedir.c_str()) == 0)
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

        SetFileAttributes(lpszDir, FILE_ATTRIBUTE_NORMAL);
		if (!RemoveDirectory(lpszDir))
        {
            return FALSE;
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

	void WriteI8deskProtocol()
	{
		#define I8DESK_PROT	"URL:i8desk Protocol"
		#define I8DESK_VER	"0.1"
		#define I8DESK_ICON	""

		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk", "",  REG_SZ, I8DESK_PROT, lstrlenA(I8DESK_PROT));
		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk", "@", REG_SZ, I8DESK_PROT, lstrlenA(I8DESK_PROT));
		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk", "Version", REG_SZ, I8DESK_VER, lstrlenA(I8DESK_VER));
		SHSetValueA(HKEY_CLASSES_ROOT, "i8desk", "URL Protocol", REG_SZ, "", 0);

		TCHAR buf[MAX_PATH] = {0};
		SDG_GetAppPath(buf, TRUE);
		tstrcat(buf, MAX_PATH, TEXT("BarOnline.exe"));
		SHSetValue(HKEY_CLASSES_ROOT, TEXT("i8desk\\DefaultIcon"), NULL, REG_SZ, buf, lstrlen(buf)*sizeof(TCHAR));
		tstrcat(buf, MAX_PATH, TEXT(" \"%1\""));
		SHSetValue(HKEY_CLASSES_ROOT, TEXT("i8desk\\Shell\\Open\\Command"), NULL, REG_SZ, buf, lstrlen(buf)*sizeof(TCHAR));
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

	void splitString(stdex::tString strSource, vector<stdex::tString>& plugs, TCHAR ch)
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

	DWORD GetBufferCRC(  const void * pInBuffer, DWORD dwSize )
	{
		return utility::CalBufCRC32(pInBuffer, dwSize);
	}

	DWORD GetFileCRC(LPCTSTR pFilePath )
	{
		utility::CAutoFile clFile = CreateFile(pFilePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (!clFile.IsValid())
		{
			return 0;
		}

		DWORD dwSize = GetFileSize(clFile, NULL);
		BYTE* pData = new BYTE[dwSize];
		DWORD dwReadBytes = 0;
		if (!ReadFile(clFile, pData, dwSize, &dwReadBytes, NULL) || dwSize != dwReadBytes)
		{
			delete []pData;
			return 0;
		}

		DWORD dwCRC = utility::CalBufCRC32(pData, dwSize);		
		delete []pData;

		return dwCRC;
	}

	std::string GetRootDirA()
	{
		char szPath[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, szPath, MAX_PATH);
		PathRemoveFileSpecA(szPath);
		PathAddBackslashA(szPath);
		return std::string(szPath);
	}
	std::wstring GetRootDirW()
	{
		WCHAR szPath[MAX_PATH] = {0};
		GetModuleFileNameW(NULL, szPath, MAX_PATH);
		PathRemoveFileSpecW(szPath);
		PathAddBackslashW(szPath);
		return std::wstring(szPath);
	}

	stdex::tString GetSystemDir()
	{
		TCHAR szPath[MAX_PATH] = {0};
		GetSystemDirectory(szPath, MAX_PATH);
		PathRemoveBackslash(szPath);
		PathAddBackslash(szPath);
		return stdex::tString(szPath);
	}

	stdex::tString GetProgramDir()
	{
		TCHAR szPath[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILES, TRUE);
		PathRemoveBackslash(szPath);
		PathAddBackslash(szPath);
		return stdex::tString(szPath);
	}
	
	stdex::tString GetWindowsDir()
	{
		TCHAR szPath[MAX_PATH] = {0};
		GetWindowsDirectory(szPath, MAX_PATH);
		PathRemoveBackslash(szPath);
		PathAddBackslash(szPath);
		return stdex::tString(szPath);		
	}

	stdex::tString GetDocumentDir()
	{
		TCHAR szPath[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_DOCUMENTS, FALSE);
		PathRemoveBackslash(szPath);
		PathAddBackslash(szPath);
		return stdex::tString(szPath);		
	}

	stdex::tString GetRungameExe()
	{
		//return GetRootDir() + TEXT("RunGame.exe");
		TCHAR szPath[MAX_PATH] = {0};
		szPath[0] = '\"';
		GetModuleFileName(NULL, &szPath[1], MAX_PATH);
		PathRemoveFileSpec(szPath);
		PathAddBackslash(szPath);
		tstrcat(szPath, MAX_PATH, TEXT("RunGame.exe\" "));
		return stdex::tString(szPath);
	}

	stdex::tString GetLocalGamePath(DWORD dwGid)
	{
		DWORD dwSize = 1024, dwType = REG_SZ;
		TCHAR szGid[MAX_PATH] = {0}, szGameInfo[MAX_PATH] = {0};
		tsprintf(szGid, MAX_PATH, TEXT("%lu"), dwGid);
		DWORD dwRet = SHGetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID, szGid, &dwType, szGameInfo, &dwSize);
		if (dwRet == ERROR_SUCCESS)
		{
			vector<stdex::tString> parts;
			splitString(szGameInfo, parts, ';');
			stdex::tString name, mode, rpath, lpath;
			for (size_t idx=0; idx<parts.size(); idx++)
			{
				stdex::tString sub = parts[idx];
				if (StrNCmpI(sub.c_str(), TEXT("name="), 5*sizeof(TCHAR)) == 0)
				{
					name = sub.substr(5);
				}
				else if (StrNCmpI(sub.c_str(), TEXT("mode="), 5*sizeof(TCHAR)) == 0)
				{
					mode = sub.substr(5);
				}
				else if (StrNCmpI(sub.c_str(), TEXT("remotepath="), 11*sizeof(TCHAR)) == 0)
				{
					rpath = sub.substr(11);
				}
				else if (StrNCmpI(sub.c_str(), TEXT("localpath="), 10*sizeof(TCHAR)) == 0)
				{
					lpath = sub.substr(10);
				}
			}
			if (mode == TEXT("2"))return lpath;
		}
			return stdex::tString(TEXT(""));
	}

	bool EnsureDllExist(LPCTSTR lpFileName, DWORD dwResId)
	{
		TCHAR szdllPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szdllPath, MAX_PATH);
		PathRemoveFileSpec(szdllPath);
		PathAddBackslash(szdllPath);
		tstrcat(szdllPath, MAX_PATH, lpFileName);

		if (PathFileExists(szdllPath)) return true;

		HMODULE hMod = GetModuleHandle(TEXT("Bootrun.dll"));
		HRSRC hRes = FindResource(hMod, MAKEINTRESOURCE(dwResId), TEXT("DLL"));
		if (hRes == NULL) return FALSE;

		DWORD   dwSize  = SizeofResource(hMod, hRes);
		HGLOBAL hGlobal = LoadResource(hMod, hRes);
		if (hGlobal == NULL) return FALSE;

		LPVOID lpData = GlobalLock(hGlobal);
		if (lpData == NULL)
		{
			FreeResource(hRes);
			return false;
		}

		utility::CAutoFile clFile = CreateFile(szdllPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (!clFile.IsValid())
		{
			GlobalUnlock(hGlobal);
			FreeResource(hRes);
			return false;
		}

		DWORD dwWriteBytes = 0;
		if (!WriteFile(clFile, lpData, dwSize, &dwWriteBytes, NULL))
		{	
			GlobalUnlock(hGlobal);
			FreeResource(hRes);
			return false;
		}

		SetEndOfFile(clFile);
		GlobalUnlock(hGlobal);
		FreeResource(hRes);

		return true;
	}

	BOOL EnsureServiceExist(LPCTSTR lpFileName, LPCTSTR lpServiceName, DWORD dwStartType /* = SERVICE_AUTO_START */)
	{
		SC_HANDLE schMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if(schMgr == NULL) return FALSE;

		stdex::tString strFilePath = GetSystemDir();
		strFilePath += TEXT("drivers\\");
		strFilePath += lpFileName;
		if (!PathFileExists(strFilePath.c_str()))
		{
			stdex::tString strTempPath = GetRootDir();
			strTempPath += lpFileName;
			CopyFile(strTempPath.c_str(), strFilePath.c_str(), TRUE);
		}

		SC_HANDLE schService = OpenService(schMgr, lpServiceName, SERVICE_ALL_ACCESS);
		if (schService == NULL)
		{
			DWORD dwError = GetLastError();
			if(dwError == ERROR_SERVICE_DOES_NOT_EXIST)
			{
				schService = CreateService(schMgr,	lpServiceName, lpServiceName, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_SYSTEM_START,			// start type 
					SERVICE_ERROR_NORMAL, strFilePath.c_str(),	NULL, NULL,	NULL, NULL, NULL);	
			}
			else
			{
				CloseServiceHandle(schMgr);
				return FALSE;
			}
		}

		BOOL bResult = TRUE;
		if(!StartService(schService, 0, NULL)
			&& GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)
		{
			bResult = FALSE;
		}
		CloseServiceHandle(schService);
		CloseServiceHandle(schMgr);
		return bResult;
	}


    stdex::tString GetFileVersion(const stdex::tString& strFileName)
    {
        stdex::tString Version(TEXT("0.0.0.0"));
        DWORD dwSize = GetFileVersionInfoSize(strFileName.c_str(), NULL);
        if(dwSize)
        {
            LPTSTR pblock = new TCHAR[dwSize+1];
            GetFileVersionInfo(strFileName.c_str(), 0, dwSize, pblock);
            UINT nQuerySize;
            DWORD* pTransTable = NULL;
            VerQueryValue(pblock, TEXT("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
            LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
            TCHAR SubBlock[MAX_PATH] = {0};
            _stprintf_s(SubBlock, TEXT("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
            LPTSTR lpData;
            if (VerQueryValue(pblock, SubBlock, (PVOID*)&lpData, &nQuerySize))
                Version = lpData;
            delete[] pblock;

            // replace ',' to '.'
            std::replace_if(Version.begin(), Version.end(), 
                std::bind2nd(std::equal_to<stdex::tString::value_type>(), ','), '.');

            // delete [Space]
            stdex::tString::iterator iter = std::remove_if(Version.begin(), Version.end(),
                std::bind2nd(std::equal_to<stdex::tString::value_type>(), ' '));

            // remove redundant character
            if( iter != Version.end() )
                Version.erase(iter, Version.end());
        }

        return Version;
    }

	BOOL CheckConnect(DWORD ip, WORD port)
	{
		sockaddr_in   server;
		SOCKET sck =socket(AF_INET, SOCK_STREAM, 0);   
		if(sck == INVALID_SOCKET)
			return FALSE;   

		DWORD ul = 1;
		if (SOCKET_ERROR == ioctlsocket(sck, FIONBIO, &ul))
		{
			closesocket(sck);
			return FALSE;
		}

		//连接   
		server.sin_family	= AF_INET;
		server.sin_port		= htons(port);
		server.sin_addr.s_addr = ip;
		if(server.sin_addr.s_addr == INADDR_NONE)
		{
			closesocket(sck);
			return FALSE;
		}
		connect(sck, (PSOCKADDR)&server, sizeof(server));

		timeval to = {4, 0};
		fd_set  fd;
		FD_ZERO(&fd);
		FD_SET(sck, &fd);
		if (select(0, NULL, &fd, NULL, &to) <= 0)
		{
			closesocket(sck);
			return   FALSE;   
		}
		ul = 0;
		if (SOCKET_ERROR == ioctlsocket(sck, FIONBIO, &ul))
		{
			closesocket(sck);
			return FALSE;
		}

		//简单的发送一个数据，以免服务器死连接。
		typedef struct tagDRP
		{
			BYTE			 MajorFunction;//1: Refresh, 2: Read, 3: Write, 4:Query.
			union
			{
				struct 
				{
					LARGE_INTEGER	Offset;
					ULONG			Length;
				}Read;
				struct 
				{
					LARGE_INTEGER   Offset;
					ULONG			Length;
				}Write;
				struct 
				{
					ULONG	DiskSize;
				}Query;
			};
		}DRP,*LPDRP;
		DRP drp = {0x04, 0};
		send(sck, (char*)&drp, sizeof(drp), 0);
		closesocket(sck);

		return TRUE;
	}

	stdex::tString ConvertIPToString( DWORD dwIp )
	{
		in_addr addr;addr.S_un.S_addr = dwIp;
		return stdex::tString(CA2T(inet_ntoa(addr)));
	}

	BOOL IsDriverExist(TCHAR chDriver)
	{
		TCHAR buf[MAX_PATH] = {0};
		tsprintf(buf, MAX_PATH, TEXT("%C:\\"), chDriver);
		return (GetDriveType(buf) != DRIVE_NO_ROOT_DIR);
	}

	HANDLE GetProcessHandle(LPCTSTR lpProcessName)
	{
		if(lpProcessName == NULL || lstrlen(lpProcessName) == 0)
			return INVALID_HANDLE_VALUE;

		HANDLE hProcess = NULL;
		DWORD dwProcesses[1024] = {0}, cbNeeded;
		TCHAR szProcessName[MAX_PATH] = {0};
		if ( !EnumProcesses( dwProcesses, sizeof(dwProcesses), &cbNeeded ) )
			return	hProcess;
		for (DWORD i = 0; i < cbNeeded / sizeof(DWORD); ++i)
		{
			hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, dwProcesses[i]);
			if (hProcess != NULL && GetCurrentProcessId() != dwProcesses[i])
			{
				GetModuleBaseName(hProcess, NULL, szProcessName, MAX_PATH);
				if (lstrcmpi(szProcessName, lpProcessName)  == 0)
				{
					return hProcess;
				}
			}
		}
		return INVALID_HANDLE_VALUE;
	}
    BOOL StartProcess(LPCTSTR lpszFileName, LPCTSTR lpszParam, LPCTSTR lpszCompare, LPCTSTR lpszReserve, BOOL bCheckExist, BOOL bTerminateExits, BOOL bParamPrefix)
	{
		HANDLE hProcess = INVALID_HANDLE_VALUE;
		if(bCheckExist)
		{
			if((hProcess = GetProcessHandle(lpszCompare)) != INVALID_HANDLE_VALUE || (hProcess = GetProcessHandle(lpszReserve)) != INVALID_HANDLE_VALUE)
			{
				if (!bTerminateExits)
				{
					CloseHandle(hProcess);
				    return FALSE;
				}
				else
				{
					TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
				}			
			}
		}

		hProcess = GetProcessHandle(TEXT("explorer.exe"));
		if(hProcess == INVALID_HANDLE_VALUE) return FALSE;
		HANDLE hToken = NULL;
		if(!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
		{
			CloseHandle(hProcess);
			return FALSE;
		}

		STARTUPINFO si = {0}; PROCESS_INFORMATION pi = {0};
		si.cb= sizeof(STARTUPINFO);	
		si.lpDesktop = TEXT("winsta0\\default");
		LPVOID lpEnv = NULL;
		CreateEnvironmentBlock(&lpEnv, hToken, FALSE);
		stdex::tString strTmpParam = GetRungameExe();
        if (lpszParam != NULL && lstrlen(lpszParam) != NULL)
        {
            strTmpParam += (bParamPrefix ? _T("\"|") : _T("\"")) + stdex::tString(lpszFileName) + _T("|") + stdex::tString(lpszParam) + _T("\"");
        }
        else
        {
            strTmpParam += (bParamPrefix ? _T("\"|") : _T("\"")) + stdex::tString(lpszFileName) + _T("\"");
        }

		BOOL bResult = CreateProcessAsUser(hToken, NULL, (LPTSTR)strTmpParam.c_str(), NULL, NULL,
			FALSE,CREATE_UNICODE_ENVIRONMENT,lpEnv, NULL, &si, &pi);
		CloseHandle(hToken);
		CloseHandle(hProcess);
		if(bResult)
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		if (lpEnv)	DestroyEnvironmentBlock(lpEnv);

		return TRUE;
	}

    BOOL StartProcess(LPCTSTR lpszParam, BOOL bUseRunGame)
    {
        HANDLE hProcess = INVALID_HANDLE_VALUE;
        hProcess = GetProcessHandle(TEXT("explorer.exe"));
        if(hProcess == INVALID_HANDLE_VALUE) return FALSE;
        HANDLE hToken = NULL;
        if(!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
        {
            CloseHandle(hProcess);
            return FALSE;
        }

        STARTUPINFO si = {0}; PROCESS_INFORMATION pi = {0};
        si.cb= sizeof(STARTUPINFO);	
        si.lpDesktop = TEXT("winsta0\\default");
        LPVOID lpEnv = NULL;
        CreateEnvironmentBlock(&lpEnv, hToken, FALSE);
        stdex::tString strTmpParam;
        if (bUseRunGame)
        {
            strTmpParam= GetRungameExe() + lpszParam;
        }
        else
        {
            strTmpParam= lpszParam;
        }
        BOOL bResult = CreateProcessAsUser(hToken, NULL, const_cast<LPTSTR>(strTmpParam.c_str()), NULL, NULL,
            FALSE,CREATE_UNICODE_ENVIRONMENT,lpEnv, NULL, &si, &pi);
        CloseHandle(hToken);
        CloseHandle(hProcess);
        if(bResult)
        {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
        if (lpEnv)	DestroyEnvironmentBlock(lpEnv);

        return TRUE;

    }
	stdex::tString GetCookieDateString()
	{
		SYSTEMTIME st = {0};
		time_t t = time(NULL) + 30 * 24 * 60 * 60;
		struct tm* ptm = gmtime(&t);
		if (ptm != NULL)
		{
			st.wYear	= ptm->tm_year + 1900;
			st.wMonth	= ptm->tm_mon + 1;
			st.wDay		= ptm->tm_mday;
			st.wHour	= ptm->tm_hour;
			st.wMinute	= ptm->tm_min;
			st.wSecond	= ptm->tm_sec;
			TCHAR buf[MAX_PATH] = {0};
			InternetTimeFromSystemTime(&st, INTERNET_RFC1123_FORMAT, buf, MAX_PATH * sizeof(TCHAR));
            return stdex::tString(buf);
		}
		return stdex::tString(TEXT(""));
	}

	BOOL ShutdownComputer( BOOL bReboot )
	{
		HANDLE hToken; 
		TOKEN_PRIVILEGES tkp; 

		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
			return false; 

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 

		tkp.PrivilegeCount = 1; 
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 	
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 	

		ExitWindowsEx(bReboot ? EWX_REBOOT|EWX_FORCE : EWX_SHUTDOWN|EWX_FORCE, 0);
		return true;
	}

	DWORD GetDriverFreeSize( TCHAR chDrv )
	{
		stdex::tString strDriver; 
		DWORD dwFreeSize = 0;

		strDriver = chDrv; 
		strDriver += TEXT(":\\");

		_ULARGE_INTEGER uiFreeSpace, uiTotalSpace,uiUserSpace;	
		BOOL bIsSuccess = GetDiskFreeSpaceEx(strDriver.c_str(), &uiUserSpace, &uiTotalSpace, &uiFreeSpace);
		if (bIsSuccess) dwFreeSize = (DWORD)(uiFreeSpace.QuadPart/(1024*1024));

		return dwFreeSize;
    }
    int ClearShortcut(LPCTSTR pszExeFile, LPCTSTR pszParamPrefix)
    {
        assert(pszExeFile != NULL && pszParamPrefix != NULL);
        vector<stdex::tString> vctDelDesktop;

        IShellLink* pIShellLink = NULL; 
        IPersistFile* pIPersistFile = NULL;
        HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &pIShellLink);
        if (FAILED(hr)) 
            return -1;	
        hr = pIShellLink->QueryInterface(IID_IPersistFile, (void**) &pIPersistFile);
        if (FAILED(hr)) 
            return -1;	

        TCHAR szDesktopPath[MAX_PATH] = {0}, szDesktopFile[MAX_PATH] = {0};
        SHGetSpecialFolderPath(NULL, szDesktopPath, CSIDL_COMMON_DESKTOPDIRECTORY, TRUE);
        PathAddBackslash(szDesktopPath);

        tsprintf(szDesktopFile, MAX_PATH, TEXT("%s*.lnk"), szDesktopPath);
        WIN32_FIND_DATA wfd = {0};
        HANDLE hFinder = FindFirstFile(szDesktopFile, &wfd);
        if (hFinder != INVALID_HANDLE_VALUE) 
        {
            do 
            {
                TCHAR szFullPath[MAX_PATH] = {0};
                tsprintf(szFullPath, MAX_PATH, TEXT("%s%s"), szDesktopPath, wfd.cFileName);
                hr = pIPersistFile->Load(szFullPath, 0);
                if (SUCCEEDED(hr))
                {
                    TCHAR szTargetPath[MAX_PATH] = {0}, szTargetParam[MAX_PATH] = {0};
                    pIShellLink->GetPath(szTargetPath, MAX_PATH, NULL, SLGP_UNCPRIORITY);
                    pIShellLink->GetArguments(szTargetParam, MAX_PATH);

                    if (lstrcmpi(szTargetPath, pszExeFile) == 0
                        && StrNCmpI(szTargetParam, pszParamPrefix, lstrlen(pszParamPrefix)) == 0)
                    {
                        vctDelDesktop.push_back(stdex::tString(szFullPath));
                    }
                }

            } while (FindNextFile(hFinder, &wfd));

            for (DWORD dwLoop = 0; dwLoop < vctDelDesktop.size(); ++dwLoop)
            {
                DeleteFile(vctDelDesktop[dwLoop].c_str());
            }
        }
        pIPersistFile->Release();
        pIShellLink->Release();

        return vctDelDesktop.size();
    }
	BOOL CreateShortcut(const vector<STShortcut>& vctShorcut, ILogger* pLog)
	{
        if (vctShorcut.size() == 0)
        {
            return TRUE;
        }
		IShellLink* pIShellLink = NULL; 
		IPersistFile* pIPersistFile = NULL;
		HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &pIShellLink);
		if (FAILED(hr))
        {
            if (pLog)pLog->WriteLog(LM_INFO, _T("创建COM组件ShellLink失败[%d]"), hr);
			return FALSE;
        }
		hr = pIShellLink->QueryInterface(IID_IPersistFile, (void**) &pIPersistFile);
		if (FAILED(hr))
        {
            if (pLog)pLog->WriteLog(LM_INFO, _T("取得COM组件接口IPersistFile失败[%d]"), hr);
			return FALSE;
        }
	
		TCHAR szDesktopPath[MAX_PATH] = {0}, szDesktopFile[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, szDesktopPath, CSIDL_COMMON_DESKTOPDIRECTORY, TRUE);
        PathAddBackslash(szDesktopPath);
        if (pLog)pLog->WriteLog(LM_INFO, _T("取得桌面路径[%s]"), szDesktopPath);
	
		for(size_t i = 0; i < vctShorcut.size(); ++i)
		{
			TCHAR szShortcutPath[MAX_PATH] = {0}, szWorkDir[MAX_PATH] = {0}, szParam[MAX_PATH] = {0};
			tsprintf(szShortcutPath, MAX_PATH, TEXT("%s%s.lnk"), szDesktopPath, vctShorcut[i].szName);
			if (PathFileExists(szShortcutPath))
				continue;
			pIShellLink->SetPath(vctShorcut[i].szTargetPath);
	
			tsprintf(szParam, MAX_PATH, TEXT("%s%s"), vctShorcut[i].szParam, vctShorcut[i].szValue);
			pIShellLink->SetArguments(szParam);
	
			tstrcpy(szWorkDir, MAX_PATH, vctShorcut[i].szTargetPath);
			PathRemoveFileSpec(szWorkDir);
			PathAddBackslash(szWorkDir);
			pIShellLink->SetWorkingDirectory(szWorkDir);	
			pIShellLink->SetIconLocation(vctShorcut[i].szIconPath, 0);
	
            HRESULT dwResult = pIPersistFile->Save(CT2W(szShortcutPath), STGM_READWRITE);
            if (dwResult == S_OK)
            {
                if (pLog)pLog->WriteLog(LM_INFO, _T("在桌面创建快捷方式成功"));
            }
            else
            {
                if (pLog)pLog->WriteLog(LM_INFO, _T("在桌面创建快捷方式失败[%d][%d]"), dwResult, GetLastError());
            }
		}	
	
		pIPersistFile->Release();
		pIShellLink->Release();
		return TRUE;
	}

    void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
    {
        if (excpInfo == NULL) 
        {
            __try 
            {
                RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
            } 
            __except(DumpMiniDump(hFile, GetExceptionInformation()),
                EXCEPTION_CONTINUE_EXECUTION) 
            {
            }
        } 
        else
        {
            MINIDUMP_EXCEPTION_INFORMATION eInfo;
            eInfo.ThreadId = GetCurrentThreadId();
            eInfo.ExceptionPointers = excpInfo;
            eInfo.ClientPointers = FALSE;
            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal,
                excpInfo ? &eInfo : NULL, NULL, NULL);
        }
    }

    LONG WINAPI UnHandledExceptionFilter(PEXCEPTION_POINTERS pExcept)
    {
        TCHAR szDir[MAX_PATH] = {0};
        TCHAR szExe[MAX_PATH] = {0};
        GetModuleFileName(NULL, szDir, _countof(szDir));
        stdex::tString szVer = GetFileVersion(szDir);

        LPTSTR pFile = PathFindFileName(szDir);
        LPTSTR pExt  = PathFindExtension(szDir);
        if (pFile != szDir && pExt != NULL)
        {
            *pExt = 0;
            tstrcpy(szExe, pFile);
        }
        else
        {
            tstrcpy(szExe, TEXT("Crash"));
        }

        PathRemoveFileSpec(szDir);
        PathAddBackslash(szDir);
        tstrcat(szDir, TEXT("Dump\\"));
        CreateDirectory(szDir, NULL);

        SYSTEMTIME st = {0};
        GetLocalTime(&st);
        TCHAR szModuleName[MAX_PATH] = {0};
        _stprintf_s(szModuleName, TEXT("%s%s-%s-%04d%02d%02d%02d%02d%02d.dmp"), szDir, szExe, szVer.c_str(), 
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        utility::CAutoFile clFile = CreateFile(szModuleName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        if (clFile.IsValid())
        {
            DumpMiniDump(clFile, pExcept);
        }
        return 0;
    }

    stdex::tString GetAppPath()
    {
        TCHAR path[MAX_PATH] = {0};
        GetModuleFileName(NULL, path, _countof(path));
        PathRemoveFileSpec(path);
        PathAddBackslash(path);
        return stdex::tString(path);
    }

    DWORD GetLocalIP()
    {
        char acHostName[MAX_PATH];
        if (gethostname(acHostName, MAX_PATH) == SOCKET_ERROR)
        {
            return 0;
        }
        hostent* pstHostInfo = gethostbyname(acHostName);
        if (pstHostInfo == NULL)
        {
            return 0;
        }
        return reinterpret_cast<in_addr*>(pstHostInfo->h_addr_list[0])->S_un.S_addr;
    }

    bool KillProcessEx(LPCTSTR lpszSerive)
    {
        DWORD aProcesses[1024] = {0}, cbNeeded = 0, cProcesses = 0;
        TCHAR path[MAX_PATH] = {0};
        if ( !::EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
            return	false;
        cProcesses = cbNeeded / sizeof(DWORD);
        for (DWORD i = 0; i < cProcesses; i++ )
        {
            HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, aProcesses[i]);
            if (hProcess != NULL )
            {
                if(::GetModuleBaseName(hProcess, NULL, path, MAX_PATH) == 0)
                {
                    CloseHandle( hProcess );
                    continue;
                }
                if (lstrcmpi(path, lpszSerive) == 0)
                {
                    char acCmd[MAX_PATH];
                    sprintf_s(acCmd, "ntsd -c q -p %d", aProcesses[i]);
                    WinExec(acCmd, SW_HIDE);
                    return true;
                }
            }
        }
        return false;
    }
}