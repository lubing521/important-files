#include "stdafx.h"
#include <shlwapi.h>
#include <process.h>
#include <WinSvc.h>
#include <winsock2.h>
#include <Iphlpapi.h>
#include <wininet.h>
#include <intshcut.h>

#include "Markup.h"
#include "resource.h"
#include "MyChiToLetter.h"
#include "CustomFileDLL.h"
#pragma comment(lib, "CustomFileDLL.lib")
#include "UI/IconHelper.h"
#include "MultiThread/Lock.hpp"
#include "VirtualDiskDll.h"

#include "dbmgr.h"

//#import "winhttp.dll"
//#import "C:\WINDOWS\WinSxS\x86_Microsoft.Windows.WinHTTP_6595b64144ccf1df_5.1.3790.4427_x-ww_FDB042FC\winhttp.dll"
#import "C:\WINDOWS\System32\winhttp.dll"
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Wininet.lib")

#define I8DESK_CLISVR_NAME		TEXT("i8deskclisvr")
#define I8DESK_CLISVR_EXE		TEXT("i8deskclisvr.exe")

#define I8DESK_VDISKSVR_NAME	TEXT("wxNda")
#define I8DESK_VDISKSVR_EXE		TEXT("wxNda.sys")

using namespace i8desk;

// inline bool In(wchar_t start, wchar_t end, wchar_t code)
// {
// 	if (code >= start && code <= end) 
// 	{
// 		return true;
// 	}
// 	return false;
// }
// 
// inline char convert(wchar_t n)
// {
// 	if (In(0xB0A1,0xB0C4,n)) return 'a';
// 	if (In(0XB0C5,0XB2C0,n)) return 'b';
// 	if (In(0xB2C1,0xB4ED,n)) return 'c';
// 	if (In(0xB4EE,0xB6E9,n)) return 'd';
// 	if (In(0xB6EA,0xB7A1,n)) return 'e';
// 	if (In(0xB7A2,0xB8c0,n)) return 'f';
// 	if (In(0xB8C1,0xB9FD,n)) return 'g';
// 	if (In(0xB9FE,0xBBF6,n)) return 'h';
// 	if (In(0xBBF7,0xBFA5,n)) return 'j';
// 	if (In(0xBFA6,0xC0AB,n)) return 'k';
// 	if (In(0xC0AC,0xC2E7,n)) return 'l';
// 	if (In(0xC2E8,0xC4C2,n)) return 'm';
// 	if (In(0xC4C3,0xC5B5,n)) return 'n';
// 	if (In(0xC5B6,0xC5BD,n)) return 'o';
// 	if (In(0xC5BE,0xC6D9,n)) return 'p';
// 	if (In(0xC6DA,0xC8BA,n)) return 'q';
// 	if (In(0xC8BB,0xC8F5,n)) return 'r';
// 	if (In(0xC8F6,0xCBF0,n)) return 's';
// 	if (In(0xCBFA,0xCDD9,n)) return 't';
// 	if (In(0xCDDA,0xCEF3,n)) return 'w';
// 	if (In(0xCEF4,0xD188,n)) return 'x';
// 	if (In(0xD1B9,0xD4D0,n)) return 'y';
// 	if (In(0xD4D1,0xD7F9,n)) return 'z';
// 	return '\0';
// }
// 
// CString GetPY(CString& sChinese)
// {
// 	std::string result;
// 	std::string src = _bstr_t(sChinese);
// 	for (size_t idx=0; idx<src.size(); idx++)
// 	{
// 		if (IsDBCSLeadByte(src[idx]))
// 		{
// 			wchar_t wchr = 0;
// 			wchr = (src[idx] & 0xff) << 8;
// 			wchr |= (src[++idx] & 0xff);			
// 			result += convert(wchr);
// 		}
// 		else
// 		{
// 			result += tolower(src[idx]);
// 		}
// 	}
// 	return CString(result.c_str());
// }

//多音字字库, 第一个为多音字，加L以示UNICODE编码。后面两个字母为多音字两个拼音首字母，必须为小写。
STMultiChar   g_arMultiChar[] = 
{
	{L'降', TEXT("jx")},
	{L'阿', TEXT("ae")},	{L'拗', TEXT("an")},	{L'扒', TEXT("bp")},	{L'堡', TEXT("bp")},
	{L'暴', TEXT("bp")},	{L'辟', TEXT("bp")},	{L'扁', TEXT("bp")},	{L'藏', TEXT("cz")},
	{L'禅', TEXT("cs")},	{L'颤', TEXT("cz")},	{L'朝', TEXT("cz")},	{L'车', TEXT("cj")},
	{L'澄', TEXT("cd")},	{L'匙', TEXT("cs")},	{L'臭', TEXT("cx")},	{L'畜', TEXT("cx")},
	{L'枞', TEXT("cz")},	{L'攒', TEXT("cz")},	{L'撮', TEXT("cz")},	{L'单', TEXT("ds")},
	{L'提', TEXT("dt")},	{L'囤', TEXT("dt")},	{L'革', TEXT("gj")},	{L'合', TEXT("gh")},
	{L'给', TEXT("gj")},	{L'谷', TEXT("gy")},	{L'桧', TEXT("gh")},	{L'巷', TEXT("hx")},
	{L'会', TEXT("hk")},	{L'颈', TEXT("jg")},	{L'枸', TEXT("jg")},	{L'奇', TEXT("jq")},
	{L'茄', TEXT("jq")},	{L'侥', TEXT("jy")},	{L'剿', TEXT("jc")},	{L'校', TEXT("jx")},
	{L'解', TEXT("jx")},	{L'芥', TEXT("jg")},	{L'矜', TEXT("jq")},	{L'卡', TEXT("kq")},
	{L'壳', TEXT("kq")},	{L'蔓', TEXT("mw")},	{L'秘', TEXT("mb")},	{L'泌', TEXT("mb")},
	{L'刨', TEXT("pb")},	{L'瀑', TEXT("pb")},	{L'栖', TEXT("qx")},	{L'蹊', TEXT("qx")},
	{L'稽', TEXT("qj")},	{L'趄', TEXT("qj")},	{L'厦', TEXT("sx")},	{L'识', TEXT("sz")},
	{L'调', TEXT("td")},	{L'尾', TEXT("wx")},	{L'尉', TEXT("wy")},	{L'吓', TEXT("xh")},
	{L'纤', TEXT("xq")},	{L'行', TEXT("xh")},	{L'省', TEXT("xs")},	{L'宿', TEXT("xs")},
	{L'吁', TEXT("yx")},	{L'轧', TEXT("zy")},	{L'粘', TEXT("zn")},	{L'属', TEXT("zs")},
	{L'幢', TEXT("zc")},	{L'传', TEXT("cz")}
};

CString GetPY(CString& sChinese)
{
	std::string src = _bstr_t(sChinese);
	std::string dst = GetPyLetter(src);
	
	return CString(dst.c_str());
}

static DWORD Crc32Table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

DWORD CalBufCRC32(BYTE* buffer, DWORD dwSize)
{
	ULONG  crc(0xffffffff);
	int len;
	len = dwSize;
	while(len--)
		crc = (crc >> 8) ^ Crc32Table[(crc & 0xFF) ^ *buffer++];
	return crc^0xffffffff;
}

class CLogger
{
public:
	CLogger()
	{
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, MAX_PATH);
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, TEXT("Log\\"));
		CreateDirectory(szFile, NULL);
		lstrcat(szFile, TEXT("BarOnline.log"));
		hFile = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			WriteLog(TEXT("start baronline..."));
		}
	}
	~CLogger()
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			WriteLog(TEXT("exit baronline.\r\n\r\n"));
			CloseHandle(hFile);
		}
	}
	void WriteLog(LPCTSTR lpszLog, bool bAddDate = true)
	{
		lock.Lock();
		if (hFile != INVALID_HANDLE_VALUE)
		{
			char buf[4096] = {0};
			int nLen = 0;
			if (bAddDate)
			{
				SYSTEMTIME st = {0};
				GetLocalTime(&st);
				nLen = sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d ", 
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			}
			nLen += sprintf(&buf[nLen], "%s\r\n", (LPCSTR)_bstr_t(lpszLog));
			DWORD dwWriteBytes = 0;
			WriteFile(hFile, buf, nLen, &dwWriteBytes, NULL);
		}
		lock.Unlock();
	}
private:
	HANDLE hFile;
    async::thread::AutoCriticalSection  lock;
};
static CLogger g_sLogger;

void AfxWriteLog(LPCTSTR lpszLog, bool bAddDate)
{
	g_sLogger.WriteLog(lpszLog, bAddDate);
}

void AfxWriteLog2(LPCTSTR lpszLog, ...)
{
	TCHAR szLog[4096] = {0};
	va_list marker;
	va_start(marker, lpszLog);
	_vstprintf(szLog, lpszLog, marker);
	g_sLogger.WriteLog(szLog);
}

#define DEFAULT_FILED_LENGTH 4096

CDbMgr::CDbMgr()
{
	m_pIconMoudle	= NULL;
	m_strNavUrlCap	= CAP_URL_NAV;
	m_strNavUrl		= URL_URLNAVAGITOER;
	m_strDotCardCap = CAP_DOT_CARD;
	m_strDotCardUrl = URL_DOTCARD;
	m_strPersonUrl  = URL_PERSON;
	m_strPersonTabText = CAP_PERSON;
	m_nDefaultPage	= 0;
	m_bShowGongGao	= false;
	m_nRBPopAdv		= 0;

	m_BrowseTab.push_back(CAP_URL_NAV);
	m_GameRoomTab.push_back(CLASS_RM_NAME);
	m_ChatRoomTab.push_back(CLASS_LT_NAME);
	//m_SoftwareTab.push_back(CLASS_CY_NAME);
	m_DotCardTab.push_back(CAP_DOT_CARD);

	LoadMultiChar();
	LoadSysOpt();
	ParseSvrList();
	ParseInValue();
	LoadFavorite();
	LoadVDisk();
	LoadIconInfo();
	LoadGameInfo();
	
	m_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExit == NULL)
		m_hThread = NULL;
	else
	{
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, NULL);
	}
}

CDbMgr::~CDbMgr()
{
	WaitExit();
	FreeSysOpt();
	FreeSvrList();
	FreeFavorite();
	FreeVDisk();
	FreeGameInfo();
	FreeIconInfo();
}

CDbMgr* CDbMgr::getInstance()
{
	static CDbMgr self;
	return &self;
}

void  CDbMgr::CheckCliService()
{
	AfxWriteLog(TEXT("check I8DeskCliSvr service."));
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE | SERVICE_CHANGE_CONFIG);
	if (schSCManager == NULL)
	{
		AfxWriteLog2(TEXT("open service manager fail:%d"), GetLastError());
		return ;
	}

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathAddBackslash(szPath);
	lstrcat(szPath, TEXT("I8DeskCliSvr.exe"));
	if (!PathFileExists(szPath))
	{
		AfxWriteLog(TEXT("I8DeskCliSvr.exe not exist."));
		CloseServiceHandle(schSCManager);
		return ;
	}
	CString strPath(szPath);
	strPath.Insert(0, TEXT('\"'));
	strPath += TEXT('\"');

	//如果没有安装服务，则安装服务
	SC_HANDLE schService = OpenService(schSCManager, I8DESK_CLISVR_NAME, SERVICE_ALL_ACCESS);	
	if (schService == NULL && GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
	{
		schService = CreateService(schSCManager, I8DESK_CLISVR_NAME, I8DESK_CLISVR_NAME, SERVICE_ALL_ACCESS, 
				SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				szPath, NULL, NULL, NULL, NULL, NULL);
	}
	if (schService == NULL)
	{
		AfxWriteLog2(TEXT("install I8DeskCliSvr fail:%d"), GetLastError());
		CloseServiceHandle(schSCManager);
		return ;
	}

	//修改服务的配置。
	ChangeServiceConfig(schService, SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, 
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szPath, NULL, NULL, NULL, NULL, NULL, I8DESK_CLISVR_NAME);
	
	//保证服务运行起来。
	StartService(schService, 0, NULL);

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	AfxWriteLog(TEXT("install i8deskclisvr success."));
}

void CDbMgr::CheckVDiskService()
{
	AfxWriteLog(TEXT("check VDisk service."));
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE | SERVICE_CHANGE_CONFIG);
	if (schSCManager == NULL)
	{
		AfxWriteLog2(TEXT("open service manager fail:%d"), GetLastError());
		return ;
	}
	TCHAR szDriver[MAX_PATH] = {0};
	GetSystemDirectory(szDriver, MAX_PATH);
	PathAddBackslash(szDriver);
	lstrcat(szDriver, TEXT("drivers\\")I8DESK_VDISKSVR_EXE);
	if (!PathFileExists(szDriver))
	{
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, MAX_PATH);
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, I8DESK_VDISKSVR_EXE);
		if (PathFileExists(szFile))
		{
			CopyFile(szFile, szDriver, FALSE);
			AfxWriteLog(TEXT("Copy driver to driver"));
		}
	}

	//如果没有安装服务，则安装服务
	SC_HANDLE schService = OpenService(schSCManager, I8DESK_VDISKSVR_NAME, SERVICE_ALL_ACCESS);	
	if (schService == NULL && GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
	{		
		schService = CreateService(schSCManager, I8DESK_VDISKSVR_NAME, I8DESK_VDISKSVR_NAME, SERVICE_ALL_ACCESS, 
			SERVICE_KERNEL_DRIVER, SERVICE_SYSTEM_START, SERVICE_ERROR_NORMAL,
			szDriver, NULL, NULL, NULL, NULL, NULL);
	}
	if (schService == NULL)
	{
		AfxWriteLog2(TEXT("install VDisk Service fail:%d"), GetLastError());
		CloseServiceHandle(schSCManager);
		return ;
	}

	//修改服务的配置。
	ChangeServiceConfig(schService, SERVICE_KERNEL_DRIVER, SERVICE_SYSTEM_START, SERVICE_ERROR_NORMAL,
		szDriver, NULL, NULL, NULL, NULL, NULL, I8DESK_VDISKSVR_NAME);

	//保证服务运行起来。
	StartService(schService, 0, NULL);

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	AfxWriteLog(TEXT("install VDisk Service success."));
}

void CDbMgr::WriteI8deskProtocol()
{
	AfxWriteLog(TEXT("write i8desk protocol."));
	#define I8DESK_ROOT	TEXT("i8desk")
	#define I8DESK_PROT	TEXT("URL:i8desk Protocol")
	#define I8DESK_VER	TEXT("0.1")
	#define I8DESK_ICON	TEXT("")

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathAddBackslash(szPath);
	lstrcat(szPath, TEXT("I8DeskCliSvr.exe"));
	if (!PathFileExists(szPath))
		return ;
	PathRemoveFileSpec(szPath);
	PathAddBackslash(szPath);
	lstrcat(szPath, TEXT("BarOnline.exe"));

	SHSetValue(HKEY_CLASSES_ROOT, I8DESK_ROOT, TEXT(""), REG_SZ, 
		I8DESK_PROT, sizeof(TCHAR) * lstrlen(I8DESK_PROT));
	SHSetValue(HKEY_CLASSES_ROOT, I8DESK_ROOT, TEXT("@"), 
		REG_SZ, I8DESK_PROT, sizeof(TCHAR) * lstrlen(I8DESK_PROT));
	SHSetValue(HKEY_CLASSES_ROOT, I8DESK_ROOT, TEXT("Version"), 
		REG_SZ, I8DESK_VER, sizeof(TCHAR) * lstrlen(I8DESK_VER));
	SHSetValue(HKEY_CLASSES_ROOT, I8DESK_ROOT, TEXT("URL Protocol"),	REG_SZ, "", 0);
	SHSetValue(HKEY_CLASSES_ROOT, TEXT("i8desk\\DefaultIcon"), TEXT(""), 
		REG_SZ, szPath, sizeof(TCHAR) * lstrlen(szPath));
	lstrcat(szPath, TEXT(" \"%1\""));
	SHSetValue(HKEY_CLASSES_ROOT, TEXT("i8desk\\Shell\\Open\\Command"), TEXT(""),
		REG_SZ, szPath, sizeof(TCHAR) * lstrlen(szPath));
	AfxWriteLog(TEXT("end of writing i8desk protocol."));
}

void CDbMgr::WaitExit()
{
	if (m_hExit != NULL)
	{
		if ( m_hThread != NULL)
		{
			SetEvent(m_hExit);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		CloseHandle(m_hExit);
		m_hExit = NULL;
	}
}

void CDbMgr::RefreshOfflineData()
{
	FreeGameInfo();
	FreeIconInfo();
	m_GameRoomTab.clear();
	m_ChatRoomTab.clear();
	m_SoftwareTab.clear();

	m_GameRoomTab.push_back(CLASS_RM_NAME);
	m_ChatRoomTab.push_back(CLASS_LT_NAME);
//	m_SoftwareTab.push_back(CLASS_CY_NAME);

	LoadIconInfo();
	LoadGameInfo();
}

bool CDbMgr::GetBoolOpt(LPCTSTR lpOptName, bool nOptDef /* = false*/)
{
	std::map<CString, CString>::iterator it = m_mapSysOpt.find(lpOptName);

	if (it == m_mapSysOpt.end() || it->second.IsEmpty())
		return nOptDef;

	return (_ttoi(it->second) != 0);
}

long CDbMgr::GetIntOpt(LPCTSTR lpOptName, long nOptDef /* = 0 */)
{
	std::map<CString, CString>::iterator it = m_mapSysOpt.find(lpOptName);

	if (it == m_mapSysOpt.end() || it->second.IsEmpty())
		return nOptDef;
	
	return _ttoi(it->second);
}

__int64 CDbMgr::GetInt64Opt(LPCTSTR lpOptName, __int64 nOptDef /* = 0 */)
{
	std::map<CString, CString>::iterator it = m_mapSysOpt.find(lpOptName);

	if (it == m_mapSysOpt.end() || it->second.IsEmpty())
		return nOptDef;

	return _ttoi64(it->second);
}

CString CDbMgr::GetStrOpt(LPCTSTR lpOptName, LPCTSTR lpOptDef /* = TEXT("") */, bool bEmptyIsDef /*= false*/)
{
	CString szValue(lpOptName);
	szValue.MakeLower();
	std::map<CString, CString>::iterator it = m_mapSysOpt.find(szValue);

	if (it == m_mapSysOpt.end() || (bEmptyIsDef && it->second.IsEmpty()))
		return lpOptDef;

	return it->second;
}

#define INI_FILE_NAME	TEXT("GameClient.ini")

CString CDbMgr::GetIniFile()
{
	TCHAR szIniFile[MAX_PATH] = {0};
	GetModuleFileName(NULL, szIniFile, MAX_PATH);
	PathRemoveFileSpec(szIniFile);
	PathAddBackslash(szIniFile);
	lstrcat(szIniFile, INI_FILE_NAME);
	return CString(szIniFile);
}

CString CDbMgr::LoadIpAddr()
{
	TCHAR szIp[MAX_PATH] = {0};
	GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), szIp, MAX_PATH, GetIniFile());
	return CString(szIp);
}

void CDbMgr::SaveIpAddr(CString str)
{
	WritePrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), str, GetIniFile());
}

CString	CDbMgr::GetUptSvrList()
{
	CString ip;
	std::vector<tagSvrCfg*>::iterator it = m_SvrCfg.begin();
	for (; it != m_SvrCfg.end(); it++)
	{
		tagSvrCfg* pSvrCfg = *it;
		if (pSvrCfg->UptIP.GetLength())
		{
			ip += pSvrCfg->UptIP;
			ip += TEXT("|");
		}
	}

	if (ip.Right(1) == TEXT("|"))
		ip.Delete(ip.GetLength()-1);

	AfxWriteLog2(TEXT("upt svr list:%s"), (LPCTSTR)ip);
	return ip;
}

tagGameInfo*	CDbMgr::FindGameInfo(DWORD gid)
{
	std::map<DWORD, tagGameInfo*>::iterator it = m_mapGameInfo.find(gid);
	if (it == m_mapGameInfo.end())
		return NULL;
	return it->second;
}

tagFavInfo*		CDbMgr::FindFavInfo(CString name)
{
	for (size_t idx=0; idx<m_FavList.size(); idx++)
	{
		if (m_FavList[idx]->strName == name)
		{
			return m_FavList[idx];
		}
	}
	return NULL;
}

tagVDiskInfo*	CDbMgr::FindVDiskInfo(CString vid)
{
	std::map<CString, tagVDiskInfo*>::iterator it = m_mapVDisk.find(vid);
	if (it == m_mapVDisk.end())
		return NULL;
	return it->second;
}

HICON CDbMgr::FindIconOfGame(DWORD gid)
{
	std::map<DWORD, tagGameInfo*>::iterator it = m_mapGameInfo.find(gid);
	if (it != m_mapGameInfo.end())
	{
		return it->second->hIcon;
	}
	return NULL;
}

HICON CDbMgr::FindIconOfGame_(DWORD gid)
{
	HICON hIcon = NULL;
	std::map<DWORD, IconInfo_st>::iterator it = m_mapIconInfo.find(gid);
	if (it != m_mapIconInfo.end())
	{
		IconInfo_st& stIconInfo = it->second;

        hIcon = ui::LoadIconFromBuffer(static_cast<const unsigned char *>(stIconInfo.pIconData), stIconInfo.dwSize);
	}
	if (hIcon == NULL)
		hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	return hIcon;
}

CString CDbMgr::GetDataDir()
{
	TCHAR szDir[MAX_PATH] = {0};
	GetModuleFileName(NULL, szDir, MAX_PATH);
	PathRemoveFileSpec(szDir);
	lstrcat(szDir, TEXT("\\Data\\"));
	PathAddBackslash(szDir);
	return CString(szDir);
}

void CDbMgr::LoadSysOpt()
{
	AfxWriteLog(TEXT("prase system option..."));
	CString File = GetDataDir() + TEXT("sysopt.dat");
	HANDLE hFile = CreateFile(File, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxWriteLog2(TEXT("open system option file fail:%d."), GetLastError());
		return ;
	}

	DWORD dwSize = GetFileSize(hFile, NULL);
	char* pData = new char[dwSize+1];
	DWORD dwReadBytes = 0;
	if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
	{
		AfxWriteLog2(TEXT("read open system option file fail:%d."), GetLastError());
		CloseHandle(hFile);
		delete[] pData;
		return ;
	}
	pData[dwReadBytes] = 0;
	try
	{
        i8desk::CPkgHelper package(pData);
		if (CMD_RET_SUCCESS == package.PopDWORD())
		{
			DWORD dwCount;
            package>>dwCount;
			for (DWORD idx=0; idx<dwCount; idx++)
			{	
				TCHAR szName[DEFAULT_FILED_LENGTH] = {0}, szValue[DEFAULT_FILED_LENGTH] = {0};
				CString Val;
                package>>szName;
				Val = szName;
                Val.MakeLower();
                BYTE ucInvalid;
				package>>ucInvalid;
				package>>szValue;
				m_mapSysOpt.insert(std::make_pair(Val, CString(szValue)));
			}
		}
	}
	catch (...) { AfxWriteLog(TEXT("data package exeception.")); }

	CloseHandle(hFile);
	delete []pData;
	AfxWriteLog(TEXT("prase system option success."));
}

void CDbMgr::LoadVDisk()
{
	AfxWriteLog(TEXT("load vdisk file..."));
	bool bHaveSID = true;
	CString szFile2 = GetDataDir() + TEXT("VDisk2.dat");
	CString szFile  = GetDataDir() + TEXT("VDisk.dat");
	HANDLE hFile = CreateFile(szFile2, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			AfxWriteLog2(TEXT("open vdisk file error:%d"), GetLastError());
			return ;
		}
		bHaveSID = false;
	}
	else
	{
		DeleteFile(szFile);
	}

	DWORD dwSize = GetFileSize(hFile, NULL);
	char* pData = new char[dwSize+1];
	DWORD dwReadBytes = 0;
	if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
	{
		AfxWriteLog2(TEXT("read vdisk file error:%d"), GetLastError());
		CloseHandle(hFile);
		delete[] pData;
		return ;
	}
	pData[dwReadBytes] = 0;
	try
	{
		i8desk::CPkgHelper package(pData);
        if (CMD_RET_SUCCESS == package.PopDWORD())
		{
			DWORD dwCount;
            package.Pop(dwCount);
			for (DWORD idx=0; idx<dwCount; idx++)
			{
				tagVDiskInfo* pVDisk = new tagVDiskInfo;
				TCHAR szValue[DEFAULT_FILED_LENGTH] = {0};
				package.PopString(szValue);
                pVDisk->VID = szValue;
				pVDisk->VID.ReleaseBuffer();
                pVDisk->IP = package.PopDWORD();
				pVDisk->Port = package.PopDWORD();
				pVDisk->SvrDrv = (TCHAR)package.PopDWORD();
				pVDisk->CliDrv = (TCHAR)package.PopDWORD();
				pVDisk->Serial = package.PopDWORD();
				pVDisk->Type = package.PopDWORD();
				pVDisk->LoadType = package.PopDWORD();
				if (bHaveSID)
				{
					package.PopString(szValue);
                    pVDisk->SID = szValue;
				}
				m_mapVDisk.insert(std::make_pair(pVDisk->VID, pVDisk));

				in_addr addr;
				addr.s_addr = pVDisk->IP;
				CString szIP(inet_ntoa(addr));
				AfxWriteLog2(TEXT("vdisk:ip=%s, port=%d, SDrv=%c, CDrv=%c, Type=%d"), 
					(LPCTSTR)szIP, pVDisk->Port, pVDisk->SvrDrv, pVDisk->CliDrv, pVDisk->Type);
			}
		}
	}
	catch (...) { AfxWriteLog(TEXT("data package exeception.")); }

	CloseHandle(hFile);
	delete []pData;
	AfxWriteLog2(TEXT("load vdisk file success."));
}

void CDbMgr::LoadFavorite()
{
	AfxWriteLog(TEXT("Load Favorite..."));
	CString File = GetDataDir() + TEXT("WebSiteInfo.dat");
	HANDLE hFile = CreateFile(File, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxWriteLog2(TEXT("Open Favorite File fail:%d"), GetLastError());
		return ;
	}

	DWORD dwSize = GetFileSize(hFile, NULL);
	char* pData = new char[dwSize+1];
	DWORD dwReadBytes = 0;
	if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
	{
		AfxWriteLog2(TEXT("Read Favorite File fail:%d"), GetLastError());
		CloseHandle(hFile);
		delete[] pData;		
		return ;
	}
	pData[dwReadBytes] = 0;
	try
	{
		i8desk::CPkgHelper package(pData);
		if (CMD_RET_SUCCESS == package.PopDWORD())
		{
			DWORD dwCount = package.PopDWORD();
			TCHAR szValue[DEFAULT_FILED_LENGTH] = {0};
			for (DWORD idx=0; idx<dwCount; idx++)
			{
				tagFavInfo* pFav = new tagFavInfo;
				package.PopString(szValue); pFav->strType = szValue;
				package.PopString(szValue);	pFav->strName = szValue;
				package.PopString(szValue);	pFav->strUrl  = szValue;
				if (pFav->strType.IsEmpty() || pFav->strName.IsEmpty() || pFav->strUrl.IsEmpty())
					delete pFav;
				else
				{
					m_FavList.push_back(pFav);
					if (pFav->strType == FAV_TYPE_WEB)
						m_BrowseTab.push_back(pFav->strName);
					else
						m_VideoTab.push_back(pFav->strName);
				}
			}
		}
	}
	catch (...) { AfxWriteLog(TEXT("data package exeception.")); }

	CloseHandle(hFile);
	delete []pData;
	AfxWriteLog(TEXT("Load Favorite file success."));
}

void CDbMgr::LoadGameInfo()
{
	std::map<CString, CString> gameClass;
	AfxWriteLog(TEXT("Load Game Info ..."));	
	CString File = GetDataDir() + TEXT("GamesInfo.dat");
	HANDLE hFile = CreateFile(File, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxWriteLog2(TEXT("Open Game Info file Fail:%d"), GetLastError());
		return ;
	}

	DWORD dwSize = GetFileSize(hFile, NULL);
	char* pData = new char[dwSize+1];
	DWORD dwReadBytes = 0;
	if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
	{
		AfxWriteLog2(TEXT("Read Game Info file Fail:%d"), GetLastError());
		CloseHandle(hFile);
		delete[] pData;
		return ;
	}
	pData[dwReadBytes] = 0;
	try
	{
		i8desk::CPkgHelper package(pData);
		if (CMD_RET_SUCCESS == package.PopDWORD())
		{
			DWORD dwCount = package.PopDWORD();
			TCHAR szValue[DEFAULT_FILED_LENGTH] = {0};
			for (DWORD idx=0; idx<dwCount; idx++)
			{
				tagGameInfo* pGame = new tagGameInfo;
				pGame->gid	= package.PopDWORD();
				pGame->pid	= package.PopDWORD();
				package.PopString(szValue);
                pGame->Name	= szValue;
				GetGameNamePy(pGame);
				package.PopString(szValue);
                pGame->IdcClass = szValue;
				
				gameClass.insert(std::make_pair(pGame->IdcClass, pGame->IdcClass));

				package.PopString(szValue);
                pGame->Exe = szValue;
				package.PopString(szValue);
                pGame->Param = szValue;
				pGame->Size = package.PopDWORD();
				pGame->DeskLnk = (package.PopDWORD() != 0);
				pGame->Toolbar = (package.PopDWORD() != 0);
				package.PopString(szValue);
                pGame->SvrPath = szValue;
				package.PopString(szValue);
                pGame->CliPath = szValue;
				pGame->dwDeleteFlg = package.PopDWORD();
				package.PopString(szValue);
                pGame->MatchFile = szValue;
				package.PopString(szValue);
                pGame->SaveFilter = szValue;
                package.PopDWORD();			//游戏在中心服务器的更新时间
                package.PopDWORD();			//游戏在中心服务器上的版本
                pGame->dwNbVer = package.PopDWORD();			//游戏在网吧服务器上的版本
				package.PopDWORD();			//游戏是否自动从服务器上更新
				pGame->I8Play = (package.PopDWORD() != 0);
				pGame->IdcClick = package.PopDWORD();
				pGame->SvrClick = package.PopDWORD();
				pGame->RunType = package.PopDWORD();
				package.PopString(szValue);
                pGame->VID = szValue;
				package.PopDWORD();			//是否是本地游戏
				package.PopString(szValue);	//备注
				pGame->hIcon = FindIconOfGame_(pGame->gid);

				std::pair<std::map<DWORD, tagGameInfo*>::iterator, bool> ret;
				ret = m_mapGameInfo.insert(std::make_pair(pGame->gid, pGame));
				if (!ret.second)
				{
					AfxWriteLog2(TEXT("gid:%lu游戏信息异常."), pGame->gid);
				}

				if (pGame->Toolbar && m_toolGameInfo.size() < 5)
				{
					m_toolGameInfo.push_back(pGame);
				}
			}
		}
	}
	catch (...) { AfxWriteLog(TEXT("data package exeception.")); }
	CloseHandle(hFile);
	delete []pData;

	//添加常用工具的TAB
	{
		CString szClass[] = {
			CLASS_CY_NAME,
			CLASS_YY_NAME,
			CLASS_GP_NAME
		};
		for (int idx=0; idx<_countof(szClass); idx++)
		{
			std::map<CString, CString>::iterator it = gameClass.find(szClass[idx]);
			if (it != gameClass.end())
			{
				gameClass.erase(it);
				m_SoftwareTab.push_back(szClass[idx]);
			}
		}
		if (m_SoftwareTab.size() == 0)
			m_SoftwareTab.push_back(CLASS_CY_NAME);
	}

	//添加游戏大厅的TAB
	{
		//添加官方类别(顺序是固定的)，（如果官方类别下面有游戏)
		CString szClass[] = {
			CLASS_RM_NAME,
			CLASS_WL_NAME,
			CLASS_DJ_NAME,
			CLASS_XX_NAME,
			CLASS_WY_NAME,
			CLASS_DZ_NAME,
			CLASS_QP_NAME,
			CLASS_PL_NAME,
			CLASS_LT_NAME
		};
		for (int idx=0; idx<_countof(szClass); idx++)
		{
			std::map<CString, CString>::iterator it = gameClass.find(szClass[idx]);
			if (it != gameClass.end())
			{
				gameClass.erase(it);
				m_GameRoomTab.push_back(szClass[idx]);
			}
		}

		//添加用户自定义的类别
		for (std::map<CString, CString>::iterator it = gameClass.begin(); it != gameClass.end(); it++)
		{
			m_GameRoomTab.push_back(it->first);
		}

		//添加中心控制的后置网址列表
		std::vector<std::pair<CString, CString>>::iterator it = m_GamePreUrlList.begin();
		for (; it != m_GamePreUrlList.end(); it++)
		{
			m_GameRoomTab.push_back(it->first);
		}
	}
	AfxWriteLog2(TEXT("Load Game Info file success:%d"), m_mapGameInfo.size());
}

void CDbMgr::LoadIconInfo()
{
	AfxWriteLog2(TEXT("Load Icon Info ..."));
	CString strIcoFile = GetDataDir() + TEXT("gameicon.dat");

    try
    {
        if (m_pIconMoudle != NULL)
        {
            DestroyCustomFile(m_pIconMoudle);
        }
	    m_pIconMoudle = CreateCustomFile(strIcoFile);
	    size_t ulIconCount = 0;
	    CustomFileInfoPtr pIconSet;

	    GetAllFileData(m_pIconMoudle, ulIconCount, pIconSet);
        assert(ulIconCount == pIconSet->size() && "CustomFileDLL的接口返回信息与预想不一致。");

	    for (size_t i = 0; i < pIconSet->size(); ++i)
	    {
            m_mapIconInfo.insert(std::make_pair(pIconSet->at(i).gid, IconInfo_st(pIconSet->at(i).data, pIconSet->at(i).size)));
        }
    }
    catch (...)
    {
        AfxWriteLog(TEXT("data package exeception."));
    }
	AfxWriteLog2(TEXT("Load Icon Info file Sucess:%d"), m_mapIconInfo.size());
}

void CDbMgr::ParseInValue()
{
	CString szSub;

	//解析点卡商城
	CString szFull = GetStrOpt(OPT_M_DOTCARD);
	if (!szFull.IsEmpty())
	{
		AfxExtractSubString(m_strDotCardCap, szFull, 0, TEXT('|'));
		AfxExtractSubString(m_strDotCardUrl, szFull, 1, TEXT('|'));
	}


	//解析网址列表以及网址导航
	szFull = GetStrOpt(OPT_M_URLNAV);
	if (!szFull.IsEmpty())
	{
		std::vector<CString> strList;
		for (int idx=0;;idx++)
		{
			AfxExtractSubString(szSub, szFull, idx, TEXT('^'));
			if (szSub.IsEmpty())
				break;
			strList.push_back(szSub);
		}

		//中心设置了网址列表，第一个是网址导航
		if (strList.size())
		{
			//设置网址导航
			szFull = strList[0];
			AfxExtractSubString(m_strNavUrlCap, szFull, 0, TEXT('|'));
			AfxExtractSubString(m_strNavUrl, szFull, 1, TEXT('|'));

			if (strList.size() > 1)
			{
				std::vector<tagFavInfo*> fList;
				for (size_t idx=1; idx<strList.size(); idx++)
				{
					tagFavInfo* pFav = new tagFavInfo;
					pFav->strType = FAV_TYPE_WEB;
					szFull = strList[idx];
				
					AfxExtractSubString(pFav->strName, szFull, 0, TEXT('|'));
					AfxExtractSubString(pFav->strUrl,  szFull, 1, TEXT('|'));
					fList.push_back(pFav);
					m_BrowseTab.push_back(pFav->strName);
				}
				m_FavList.insert(m_FavList.begin(), fList.begin(), fList.end());
			}
		}
	}

	//解析影视/音乐网址
	szFull = GetStrOpt(OPT_M_VDMUS);
	if (!szFull.IsEmpty())
	{
		std::vector<CString> strList;
		for (int idx=0;;idx++)
		{
			AfxExtractSubString(szSub, szFull, idx, TEXT('^'));
			if (szSub.IsEmpty())
				break;
			strList.push_back(szSub);
		}

		std::vector<tagFavInfo*> fList;
		for (size_t idx=0; idx<strList.size(); idx++)
		{
			tagFavInfo* pFav = new tagFavInfo;
			pFav->strType = FAV_TYPE_VIDEO;
			szFull = strList[idx];
			AfxExtractSubString(pFav->strName, szFull, 0, TEXT('|'));
			AfxExtractSubString(pFav->strUrl,  szFull, 1, TEXT('|'));
			fList.push_back(pFav);
			m_VideoTab.push_back(pFav->strName);
		}
		m_FavList.insert(m_FavList.begin(), fList.begin(), fList.end());
	}

	//解析简默认首页
	szFull = GetStrOpt(OPT_M_MENUPAGE);//"p1$1,p2$1001"
	if (!szFull.IsEmpty())
	{
		CString str;
		AfxExtractSubString(szSub, szFull, 0, TEXT(','));
		AfxExtractSubString(str, szSub, 1, TEXT('$'));
		if (_ttoi(str) == 1)
		{
			AfxExtractSubString(szSub, szFull, 1, TEXT(','));
			AfxExtractSubString(str, szSub, 1, TEXT('$'));
			m_nDefaultPage = _ttoi(str);
		}
	}

	//解析是否需要显示公告
	szFull = GetStrOpt(OPT_M_GGONOFF);
	{
		//中心开关(p1$n,p2$n),目前只有p1有效,p2保留
		CString str;
		AfxExtractSubString(szSub, szFull, 0, TEXT(','));
		AfxExtractSubString(str, szSub, 1, TEXT('$'));
		int nFlag = _ttoi(str);
		bool bNeedShow = false;
		if (nFlag == 1)			//中心要求强制显示
			m_bShowGongGao = true;
		else if (nFlag == 0)	//中心要示强制不显示
			m_bShowGongGao = false;
		else					//中心要求按网吧设置来显示.
		{
			//网吧服务端开关
			m_bShowGongGao = (GetIntOpt(OPT_M_SHOWGONGGAO) == 1);
		}
	}

	//得到菜单右下角弹出广告的机率
	szFull = GetStrOpt(OPT_M_RPOPONOFF);
	if (!szFull.IsEmpty())
	{
		CString str;
		AfxExtractSubString(szSub, szFull, 0, TEXT(','));
		AfxExtractSubString(str, szSub, 1, TEXT('$'));
		if (_ttoi(str) == 1)
		{
			AfxExtractSubString(szSub, szFull, 1, TEXT(','));
			AfxExtractSubString(str, szSub, 1, TEXT('$'));
			m_nRBPopAdv = _ttoi(str);
		}
	}

	//解析个人用户网址
	szFull = GetStrOpt(OPT_M_PERSONURL);
	if (!szFull.IsEmpty())
	{
		m_strPersonUrl = szFull;
	}

	//游戏后置网址列表。
	m_GamePreUrlList.clear();
	szFull = GetStrOpt(OPT_M_GAMEROOMURL);
	if (!szFull.IsEmpty())
	{
		for (int idx=0; ;idx++)
		{
			AfxExtractSubString(szSub, szFull, idx, TEXT('^'));
			if (szSub.IsEmpty())
				break;
			else
			{
				CString name, url;
				AfxExtractSubString(name, szSub, 0, TEXT('|'));
				AfxExtractSubString(url, szSub, 1, TEXT('|'));
				name.Trim(); url.Trim();
				if (!name.IsEmpty() && !url.IsEmpty())
				{
					m_GamePreUrlList.push_back(std::make_pair(name, url));
				}
			}
		}
	}
}

void CDbMgr::ParseSvrList()
{
	AfxWriteLog(TEXT("Load Server List Config..."));
	bool bAdd = true;
	CString szSvrIp = LoadIpAddr();

	CString File = GetDataDir() + TEXT("ServerCfg.dat");
	HANDLE hFile = CreateFile(File, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxWriteLog2(TEXT("Open Server List Config fail:%d"), GetLastError());
	}
	else
	{
		DWORD dwSize = GetFileSize(hFile, NULL);
		char* pData = new char[dwSize+1];
		DWORD dwReadBytes = 0;
		if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
		{
			AfxWriteLog2(TEXT("Read Server List Config fail:%d"), GetLastError());
			CloseHandle(hFile);
			delete[] pData;		
			return ;
		}
		pData[dwReadBytes] = 0;
		try
		{
			i8desk::CPkgHelper package(pData);
			if (CMD_RET_SUCCESS == package.PopDWORD())
			{
				DWORD dwCount = package.PopDWORD();
				for (DWORD dwLoop = 0; dwLoop < dwCount; ++dwLoop)
				{
					TCHAR buf[DEFAULT_FILED_LENGTH] = {0};
					tagSvrCfg* pSvrCfg = new tagSvrCfg;
					package.PopString(buf); pSvrCfg->Sid	= buf;
					package.PopString(buf);	pSvrCfg->SyncIP	= buf;
					package.PopString(buf);	pSvrCfg->VDiskIP= buf;;
					package.PopString(buf); pSvrCfg->UptIP  = buf;
					pSvrCfg->dwSyncType = package.PopDWORD();
					pSvrCfg->dwDestDrv  = package.PopDWORD();
					pSvrCfg->dwBalanceType = package.PopDWORD();
					m_SvrCfg .push_back(pSvrCfg);
					AfxWriteLog2(TEXT("Svr:VD=%s/Upt=%s/Sync=%s"), (LPCTSTR)pSvrCfg->VDiskIP, 
						(LPCTSTR)pSvrCfg->UptIP, (LPCTSTR)pSvrCfg->SyncIP);

					if (pSvrCfg->SyncIP.CompareNoCase(szSvrIp) == 0 ||
						pSvrCfg->UptIP.CompareNoCase(szSvrIp) == 0 ||
						pSvrCfg->VDiskIP.CompareNoCase(szSvrIp) == 0)
					{
						bAdd = false;
					}
				}
				AfxWriteLog(TEXT("Load Server List Config file success."));
			}
		}
		catch (...) { AfxWriteLog(TEXT("data package exeception.")); }
		CloseHandle(hFile);
		delete []pData;
	}
	//添加主服务器进入服务器列表。
	if (bAdd)
	{
		tagSvrCfg* pSvrCfg = new tagSvrCfg;
		pSvrCfg->Sid = TEXT("{CE625906-FC97-4890-9C1F-7C72C81103A2}");
		pSvrCfg->SyncIP = pSvrCfg->VDiskIP = pSvrCfg->UptIP = szSvrIp;
		pSvrCfg->dwSyncType = pSvrCfg->dwDestDrv = pSvrCfg->dwBalanceType = 0;
		m_SvrCfg .push_back(pSvrCfg);
	}
}

void CDbMgr::FreeSysOpt()
{
	m_mapSysOpt.clear();
}

void CDbMgr::FreeVDisk()
{
	std::map<CString, tagVDiskInfo*>::iterator it = m_mapVDisk.begin();
	for (; it!=m_mapVDisk.end(); it++)
	{
		delete it->second;
	}
	m_mapVDisk.clear();
}

void CDbMgr::FreeFavorite()
{
	int nSize = m_FavList.size();
	std::vector<tagFavInfo*>::iterator it = m_FavList.begin();
	for (; it != m_FavList.end(); it++)
	{
		delete (*it);
	}
	m_FavList.clear();
}

void CDbMgr::FreeGameInfo()
{
	std::map<DWORD, tagGameInfo*>::iterator it = m_mapGameInfo.begin();
	for (; it != m_mapGameInfo.end(); it ++)
	{
		DestroyIcon(it->second->hIcon);
		delete it->second;
	}
	m_mapGameInfo.clear();
	m_toolGameInfo.clear();
}

void CDbMgr::FreeIconInfo()
{
	if (m_pIconMoudle != NULL)
    {
        DestroyCustomFile(m_pIconMoudle);
		m_pIconMoudle = NULL;
	}
	m_mapIconInfo.clear();
}

void CDbMgr::FreeSvrList()
{
	std::vector<tagSvrCfg*>::iterator it = m_SvrCfg.begin();
	for (; it != m_SvrCfg.end(); it++)
	{
		delete (*it);
	}
	m_SvrCfg.clear();

}

bool CDbMgr::CheckConnect(DWORD ip, WORD port)
{
	sockaddr_in   server;
	SOCKET sck =socket(AF_INET, SOCK_STREAM, 0);   
	if(sck == INVALID_SOCKET)
		return false;   

	DWORD ul = 1;
	if (SOCKET_ERROR == ioctlsocket(sck, FIONBIO, &ul))
	{
		closesocket(sck);
		return false;
	}

	//连接   
	server.sin_family	= AF_INET;
	server.sin_port		= port;
	server.sin_addr.s_addr = ip;
	if(server.sin_addr.s_addr == INADDR_NONE)
	{
		closesocket(sck);
		return false;
	}
	connect(sck, (PSOCKADDR)&server, sizeof(server));

	timeval to = {4, 0};
	fd_set  fd;
	FD_ZERO(&fd);
	FD_SET(sck, &fd);
	if (select(0, NULL, &fd, NULL, &to) <= 0)
	{
		closesocket(sck);
		return   false;   
	}
	ul = 0;
	if (SOCKET_ERROR == ioctlsocket(sck, FIONBIO, &ul))
	{
		closesocket(sck);
		return false;
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

	return true;
}

UINT CDbMgr::ThreadProc(LPVOID lpVoid)
{
	CheckVDiskService();
	CheckCliService();
	WriteI8deskProtocol();

	CDbMgr* pDbMgr = reinterpret_cast<CDbMgr*>(lpVoid);
	if (pDbMgr == NULL)
		return 0;

	AfxWriteLog(TEXT("start refresh vdisk..."));
	TCHAR szIniFile[MAX_PATH] = {0};
	GetModuleFileName(NULL, szIniFile, MAX_PATH);
	PathRemoveFileSpec(szIniFile);
	PathAddBackslash(szIniFile);
    lstrcat(szIniFile, TEXT("Refresh.dll"));
    CVirtualDiskDll clVirtualDisk;
    if (!clVirtualDisk.Load(szIniFile))
    {
        AfxWriteLog(TEXT("Load Refresh.dll fail."));
        AfxWriteLog(TEXT("end refresh vdisk..."));
        return false;
    }

	std::map<CString, tagVDiskInfo*>::iterator it = pDbMgr->m_mapVDisk.begin();	
	CString szVDiskTempDir = pDbMgr->GetStrOpt(OPT_M_VDDIR);
	if (szVDiskTempDir.IsEmpty())
		szVDiskTempDir = TEXT("C:\\Temp\\");
	if (szVDiskTempDir.Right(1) != TEXT("\\") || szVDiskTempDir.Right(1) != TEXT("/"))
		szVDiskTempDir += TEXT("\\");
	SHCreateDirectory(NULL, _bstr_t(szVDiskTempDir));
	if (!PathFileExists(szVDiskTempDir))
	{
		AfxWriteLog2(TEXT("VDisk Temp dir is error:%s"), (LPCTSTR)szVDiskTempDir);
		AfxWriteLog(TEXT("end refresh vdisk..."));
		return 0;
	}

	for (; it != pDbMgr->m_mapVDisk.end(); it++)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(pDbMgr->m_hExit, 0))
			break;

		tagVDiskInfo *pVDisk = it->second;
		if (pVDisk->Type == 0 && (pVDisk->LoadType == vdStartMenu || pVDisk->LoadType == vdBoot))	//表示是i8自己的虚拟盘
		{
			DWORD	ip	 = pVDisk->IP;
			WORD	port = htons((WORD)pVDisk->Port);
			bool    bNeedCheck = true;

			if (pVDisk->SID.GetLength())
			{
				for (std::vector<tagSvrCfg*>::iterator it = pDbMgr->m_SvrCfg.begin();
					it != pDbMgr->m_SvrCfg.end(); it++)
				{
					tagSvrCfg* pSvrCfg = *it;
					DWORD dwIp = inet_addr(_bstr_t(pSvrCfg->VDiskIP));
					if (pDbMgr->CheckConnect(dwIp, port))
					{
						ip = dwIp;
						bNeedCheck = false;
						break;
					}
					else
					{
						in_addr addr;
						addr.s_addr = ip;
						char* szIP = inet_ntoa(addr);
						AfxWriteLog2(TEXT("check connect to vdisk server fail:%s."), CString(szIP));
					}
				}
			}

			in_addr addr;
			addr.s_addr = ip;
			CString szIP(inet_ntoa(addr));
			if (!bNeedCheck || pDbMgr->CheckConnect(ip, port))
			{
				int ret = clVirtualDisk.DiskMount(0, ip, port, (char)pVDisk->CliDrv, _bstr_t(szVDiskTempDir));
				_com_error Error(ret);
				AfxWriteLog2(TEXT("refresh VDisk:%s:%c:%s"), (LPCTSTR)szIP, pVDisk->CliDrv, (LPCTSTR)Error.ErrorMessage());
			}
			else
			{
				AfxWriteLog2(TEXT("check connect to vdisk server fail:%s."), (LPCTSTR)szIP);
			}
		}
	}
	AfxWriteLog(TEXT("end refresh vdisk..."));
	return 0;
}

bool CDbMgr::IsRefVDiskFinish()
{
	if (m_hThread == NULL || WAIT_OBJECT_0 == WaitForSingleObject(m_hThread, 0))
		return true;

	return false;
}

bool CDbMgr::RefreshVDisk( TCHAR chCltDrv )
{
	TCHAR szBuffer[MAX_PATH] = {0};
	_stprintf(szBuffer, TEXT("%C:\\"), chCltDrv);
	if(GetDriveType(szBuffer) != DRIVE_NO_ROOT_DIR)
	{
		return true;
	}

	AfxWriteLog(TEXT("start refresh vdisk ..."));
	TCHAR szIniFile[MAX_PATH] = {0};
	GetModuleFileName(NULL, szIniFile, MAX_PATH);
	PathRemoveFileSpec(szIniFile);
	PathAddBackslash(szIniFile);
	lstrcat(szIniFile, TEXT("Refresh.dll"));
    CVirtualDiskDll clVirtualDisk;
    if (!clVirtualDisk.Load(szIniFile))
    {
        AfxWriteLog(TEXT("Load Refresh.dll fail."));
        AfxWriteLog(TEXT("end refresh vdisk..."));
        return false;
    }

	std::map<CString, tagVDiskInfo*>::iterator it = m_mapVDisk.begin();	
	CString szVDiskTempDir = GetStrOpt(OPT_M_VDDIR);
	if (szVDiskTempDir.IsEmpty())
		szVDiskTempDir = TEXT("C:\\Temp\\");
	if (szVDiskTempDir.Right(1) != TEXT("\\") || szVDiskTempDir.Right(1) != TEXT("/"))
		szVDiskTempDir += TEXT("\\");
	SHCreateDirectory(NULL, _bstr_t(szVDiskTempDir));
	if (!PathFileExists(szVDiskTempDir))
	{
		AfxWriteLog2(TEXT("VDisk Temp dir is error:%s"), (LPCTSTR)szVDiskTempDir);
		AfxWriteLog(TEXT("end refresh vdisk..."));
		return false;
	}

	bool bRes = false;
	for (; it != m_mapVDisk.end(); it++)
	{
		tagVDiskInfo *pVDisk = it->second;
		if (pVDisk->CliDrv != chCltDrv)
		{
			continue;
		}

		if (pVDisk->Type == 0)	//表示是i8自己的虚拟盘
		{
			DWORD	ip	 = pVDisk->IP;
			WORD	port = htons((WORD)pVDisk->Port);
			bool    bNeedCheck = true;

			if (pVDisk->SID.GetLength())
			{
				for (std::vector<tagSvrCfg*>::iterator it = m_SvrCfg.begin();
					it != m_SvrCfg.end(); it++)
				{
					tagSvrCfg* pSvrCfg = *it;
					DWORD dwIp = inet_addr(_bstr_t(pSvrCfg->VDiskIP));
					if (CheckConnect(dwIp, port))
					{
						ip = dwIp;
						bNeedCheck = false;
						break;
					}
					else
					{
						in_addr addr;
						addr.s_addr = ip;
						char* szIP = inet_ntoa(addr);
						AfxWriteLog2(TEXT("check connect to vdisk server fail:%s."), CString(szIP));
					}
				}
			}

			in_addr addr;
			addr.s_addr = ip;
			CString szIP(inet_ntoa(addr));
			if (!bNeedCheck || CheckConnect(ip, port))
			{
				int ret = clVirtualDisk.DiskMount(0, ip, port, (char)pVDisk->CliDrv, _bstr_t(szVDiskTempDir));
				bRes = true;
				_com_error Error(ret);
				AfxWriteLog2(TEXT("refresh VDisk:%s:%c:%s"), (LPCTSTR)szIP, pVDisk->CliDrv, (LPCTSTR)Error.ErrorMessage());
			}
			else
			{
				AfxWriteLog2(TEXT("check connect to vdisk server fail:%s."), (LPCTSTR)szIP);
			}			
		}
		break;
	}
	AfxWriteLog(TEXT("end refresh vdisk..."));
	return bRes;   
}

void CDbMgr::GetMacAddr( LPSTR buf, int nSize )
{
	LPTSTR pData = new TCHAR[nSize];
	pData[0] = 0;
	DWORD dwType = REG_SZ;
	DWORD dwSize = nSize * sizeof(TCHAR);
	DWORD dwError = SHGetValue(HKEY_LOCAL_MACHINE, TEXT("software\\Goyoo\\i8desk"), TEXT("cmac"), &dwType, pData, &dwSize);
	strcpy(buf, _bstr_t(pData));
	delete []pData;
}

void CDbMgr::GetMacAddr( LPTSTR buf, int nSize )
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = nSize * sizeof(TCHAR);
	DWORD dwError = SHGetValue(HKEY_LOCAL_MACHINE, TEXT("software\\Goyoo\\i8desk"), TEXT("cmac"), &dwType, buf, &dwSize);
}

CString CDbMgr::TransChToGBK( LPCTSTR lpszSouce )
{
	std::string source = CT2A(lpszSouce);
	char element[4];
	unsigned char chr;
	std::string res;
	for ( unsigned int i=0; i<source.length(); i++ )
	{
		chr = source[i];
		if (chr > 127)		//汉字
		{
			sprintf(element, "%%%02X", chr);				
			res += element;
			chr = source[++i];
			sprintf(element, "%%%02X", chr);
			res += element;
		}
		else if ( (chr >= '0' && chr <= '9') || 
			(chr >= 'a' && chr <= 'z') ||
			(chr >= 'A' && chr <= 'Z') )
		{
			res += chr;
		}
		//else if ( strchr(reserved,chr) || strchr(unsafe,chr) || strchr(other,chr) )
		else
		{
			sprintf( element, "%%%02X", chr); 
			res += element;
		}
	}	

	return CString(res.c_str());
}

void CDbMgr::LoadMultiChar()
{
    int nMultiCharCount = _countof(g_arMultiChar);
	for (int i = 0; i < nMultiCharCount; ++i)
	{
		m_mapMultiChar.insert(std::make_pair(g_arMultiChar[i].chChar, g_arMultiChar[i].szPy));
	}
}

void CDbMgr::GetGameNamePy( tagGameInfo* pGameInfo )
{
    pGameInfo->Py = GetPY(pGameInfo->Name);
	
	int nNameLen = pGameInfo->Name.GetLength(), nIndex = 0;
	std::map<TCHAR, WCHAR*>::iterator it = m_mapMultiChar.end();
	for (nIndex = 0; nIndex < nNameLen; ++nIndex)
	{
		it = m_mapMultiChar.find(pGameInfo->Name[nIndex]);
		if (it != m_mapMultiChar.end()) 
			break;
	}
	if (it != m_mapMultiChar.end())
	{
        pGameInfo->MultiPy = pGameInfo->Py;
		pGameInfo->Py.SetAt(nIndex, it->second[0]);
		pGameInfo->MultiPy.SetAt(nIndex, it->second[1]);
	}
}

DWORD CDbMgr::Idxfile_GetVersion( LPCWSTR file )
{
	HANDLE hFile = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return -1;

	DWORD dwSize = GetFileSize(hFile, NULL);
	tagIdxHeahder header = {0};
	DWORD dwReadBytes = 0;
	if (dwSize < sizeof(tagIdxHeahder) || !ReadFile(hFile, &header, sizeof(tagIdxHeahder), &dwReadBytes, NULL))
	{
		CloseHandle(hFile);
		return -1;
	}

	DWORD offset = sizeof(tagIdxHeahder) + header.blknum * 32 + header.xmlsize;
	if (dwSize < offset + 32)
	{
		CloseHandle(hFile);
		return -1;
	}

	DWORD dwVersion = 0;
	if (!SetFilePointer(hFile, offset, NULL, FILE_BEGIN) || 
		!ReadFile(hFile, &dwVersion, sizeof(DWORD), &dwReadBytes, NULL))
	{
		CloseHandle(hFile);
		return -1;
	}

	CloseHandle(hFile);
	return dwVersion;
}

CRptData::CRptData()
{
}

CRptData::~CRptData()
{
	CloseServer();
}

void CRptData::ReprotDataToServer(CDockNavBar* pNav, std::map<DWORD, DWORD>& GameClick, bool bAck /* = true */)
{
	ASSERT(pNav != NULL);
	if (ConnectToServer())
	{
		char buf[0x10000] = {0};
		i8desk::CPkgHelper package(buf, CMD_GAME_REPCLICK, OBJECT_BARONLINE);
		
		//游戏点击数
		package.PushDWORD(GameClick.size());
		{
			std::map<DWORD, DWORD>::iterator it = GameClick.begin();
			for (; it!=GameClick.end(); it++)
			{
				package.PushDWORD(it->first);
				package.PushDWORD(it->second);
			}
		}

		//导航栏的点击数（包括菜单点击数）
		TCHAR szMacAddr[30] = TEXT("FF-FF-FF-FF-FF-FF");
		AfxGetDbMgr()->GetMacAddr(szMacAddr, sizeof(szMacAddr));
		package.PushString(szMacAddr, 17);
		package.PushDWORD(6+1);
		{
			//首先压入菜单的点击数。菜单ID固定为-1;
			package.PushDWORD(-1);
			{
				//第一次上报就报了菜单运行了，以后就不需要报了，即上报为0.
				static bool bFirst = true;
				package.PushDWORD(bFirst ? 1 : 0); 
				bFirst = false;
			}
			package.PushDWORD(0);
		
			//压入导航栏运行次数。
			tagNavClick NavClick[6] = {0};
			pNav->GetAllNavClick(NavClick);
			for (int idx=0; idx<_countof(NavClick); idx++)
			{
				package.PushDWORD(NavClick[idx].dwIdx);
				package.PushDWORD(NavClick[idx].dwClick);
				package.PushDWORD(NavClick[idx].dwRunTime);
			}
		}

		//清空己报过的游戏点击数。
		if (SendDataToServer(buf, package.GetLength(), bAck))
		{
			GameClick.clear();
		}
		CloseServer();
	}
}

bool CRptData::ConnectToServer()
{
	sockaddr_in   server;
	m_sck =socket(AF_INET, SOCK_STREAM, 0);   
	if(m_sck == INVALID_SOCKET)
		return false;   

	DWORD ul = 1;
	if (SOCKET_ERROR == ioctlsocket(m_sck, FIONBIO, &ul))
	{
		closesocket(m_sck);
		return false;
	}

	//连接   
	server.sin_family	= AF_INET;   
	server.sin_port		= htons(17918);
	server.sin_addr.s_addr = inet_addr(_bstr_t(AfxGetDbMgr()->LoadIpAddr()));
	if(server.sin_addr.s_addr == INADDR_NONE)
	{
		closesocket(m_sck);
		return false;
	}
	connect(m_sck, (PSOCKADDR)&server, sizeof(server));
	
	timeval to = {3, 0};
	fd_set  fd;
	FD_ZERO(&fd);
	FD_SET(m_sck, &fd);
	if (select(0, NULL, &fd, NULL, &to) <= 0)
	{
		closesocket(m_sck);
		return   false;   
	}
	ul = 0;
	if (SOCKET_ERROR == ioctlsocket(m_sck, FIONBIO, &ul))
	{
		closesocket(m_sck);
		return false;
	}
	return true;
}

bool CRptData::SendDataToServer(char* pData, int len, bool bAck /* = false */)
{
	//send data to server.
	int nSend = 0;
	while(nSend < len)
	{
		int nlen = send(m_sck, pData+nSend, len-nSend, 0);
		if(nlen <= 0)
		{
			CloseServer();;
			return false;
		}
		nSend += nlen;
	}
	if (!bAck)
		return true;
	
	char buf[0x10000] = {0};
	i8desk::pkgheader* pheader = (i8desk::pkgheader*)buf;
	int   nRecvLen = 0;
	while (1)
	{
		FD_SET fdset;
		timeval tv = {3, 0};
		FD_ZERO(&fdset);
		FD_SET(m_sck, &fdset);
		int ret = select(0, &fdset, NULL, NULL, &tv);
		if (ret <= 0)
		{
			CloseServer();
			return false;
		}

		if (FD_ISSET(m_sck, &fdset))
		{
			int len = recv(m_sck, buf + nRecvLen, sizeof(buf)-nRecvLen, 0);
			if (len <= 0)
			{
				CloseServer();
				return false;
			}
			nRecvLen += len;
			
			if (nRecvLen >= sizeof(i8desk::pkgheader) && nRecvLen >= (int)pheader->Length)
				return true;
		}
	}
	return true;
}

void CRptData::CloseServer()
{
	if (m_sck != INVALID_SOCKET)
	{
		closesocket(m_sck);
		m_sck = INVALID_SOCKET;
	}
}

LPCTSTR CPersonMgr::GetCookie()
{
	DWORD dwSize = 0x1000;
	BOOL  bFlag = FALSE;
	LPTSTR pszCookie = NULL;
	while (!bFlag)
	{
		pszCookie = new TCHAR[dwSize];
		ZeroMemory((LPVOID)pszCookie, dwSize*sizeof(TCHAR));
		bFlag = InternetGetCookie(TEXT("http://i8.com.cn"), NULL, pszCookie, &dwSize);
		if (!bFlag)
		{
			delete []pszCookie;
			pszCookie = NULL;

			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				dwSize *= 2;
			else
				break;
		}
	}
	return pszCookie;
}

void CPersonMgr::NoOperator()
{
	OutputDebugString(TEXT("..."));
}

void CPersonMgr::GetPersonState()
{
	LPCTSTR pszCookie = GetCookie();
	if (pszCookie == NULL)
		return ;

	try
	{
		using namespace WinHttp;
		IWinHttpRequestPtr http(__uuidof(WinHttpRequest));

		http->Open(TEXT("POST"), _bstr_t(TEXT("http://app.i8.com.cn/InterFace/UserState.koc")), VARIANT_FALSE);
		http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("Keep-Alive"));
		http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));
		http->SetRequestHeader(_bstr_t("Cookie"), _bstr_t(pszCookie));
		http->Send();
		if (200 != http->Status)
			_com_raise_error(0);
		else
		{
			/*
			<UserInfo>
				<User_ID>1000</User_ID><!-- 用户ID(-1为末登录) -->
				<User_Name>i8cn</User_Name><!-- 用户名-->
				<Login_IP>192.168.1.244</Login_IP><!-- 登录IP -->
				<Login_Date>2010-01-04 10:58:00</Login_Date><!-- 登录时间-->
			</UserInfo>
			*/
			CString strXml = http->ResponseText;
			CMarkup xml;
			if (xml.SetDoc(strXml))
			{
				xml.ResetPos();
				if (xml.FindElem(TEXT("UserInfo")) && xml.IntoElem())
				{
					if (xml.FindElem(TEXT("User_ID")))
					{
						m_dwUserId = _ttoi(xml.GetData());
						if (xml.FindElem(TEXT("User_Name")))
							m_strUserName = xml.GetData();
						if (xml.FindElem(TEXT("Login_IP")))
							m_strLgnIp = xml.GetData();
						if (xml.FindElem(TEXT("Login_Date")))
							m_strLgnDate = xml.GetData();
					}
				}
			}
		}
	}
	catch (...) { }
	delete []pszCookie;
}

CPersonMgr::RES_AddGame CPersonMgr::AddGameToFavList(DWORD gid)
{
	LPCTSTR pszCookie = GetCookie();
	if (pszCookie == NULL)
		return RES_AddGame_Failed;

	if (m_dwUserId == -1)
	{
		return RES_AddGame_NotLogin;
	}

	RES_AddGame res = RES_AddGame_Failed;
	try
	{
		using namespace WinHttp;
		IWinHttpRequestPtr http(__uuidof(WinHttpRequest));

		http->Open(TEXT("POST"), _bstr_t(TEXT("http://app.i8.com.cn/InterFace/FavoritesProductAdd.koc")), VARIANT_FALSE);
		http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("Keep-Alive"));
		http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));
		http->SetRequestHeader(_bstr_t("Cookie"), _bstr_t(pszCookie));
		TCHAR szBuf[100] = {0};
		_stprintf(szBuf, TEXT("ProductID=%d&ListOrder=0"), gid);
		http->Send(szBuf);
		if (200 != http->Status)
			_com_raise_error(0);
		else
		{
			CString strXml = http->ResponseText;
            CString strFlag;
		    AfxExtractSubString(strFlag, strXml, 0, TEXT('&'));
			AfxExtractSubString(strFlag, strFlag, 1, TEXT('='));	
			if(_ttoi(strFlag) == 1)
			{
				res = RES_AddGame_OK;
			}
		}
	}
	catch (...) { }
	delete []pszCookie;

	return res;
}

HRESULT CPersonMgr::CreateShortcutToURL(LPCTSTR pszURL, LPCTSTR pszLinkFile)
{
	HRESULT hRes;
	IUniformResourceLocator *pURL = NULL;
	
	hRes = CoCreateInstance (CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (LPVOID*)&pURL);
	if (SUCCEEDED(hRes))
	{
		IPersistFile *pPF = NULL;
		hRes = pURL->SetURL(pszURL, 0);
		if (SUCCEEDED(hRes))
		{
			hRes = pURL->QueryInterface (IID_IPersistFile, (void **)&pPF);
			if (SUCCEEDED(hRes))
			{
				hRes = pPF->Save(_bstr_t(pszLinkFile), TRUE);
				pPF->Release ();
			}
		}
		pURL->Release ();
	}
	return hRes;
}

void CPersonMgr::GetPersonFavorite(DWORD dwNum /* = -1 */, DWORD dwDefault /* = 1 */)
{
	//删除先添加的收藏夹
	DeleteFavOfAdded();

	LPCTSTR pszCookie = GetCookie();
	if (pszCookie == NULL)
		return ;
	try
	{
		using namespace WinHttp;
		IWinHttpRequestPtr http(__uuidof(WinHttpRequest));

		http->Open(TEXT("POST"), _bstr_t(TEXT("http://user.cgi.i8cn.com/Personal/FavoritesWebsiteGet.koc")), VARIANT_FALSE);
		http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("Keep-Alive"));
		http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));
		http->SetRequestHeader(_bstr_t("Cookie"), _bstr_t(pszCookie));
		TCHAR szBuf[100] = {0};
		_stprintf(szBuf, TEXT("UserID=%d&Num=%d&Default=%d"), m_dwUserId, dwNum, dwDefault);
		http->Send(szBuf);
		if (200 != http->Status)
			_com_raise_error(0);
		else
		{
			/*
			<FavoritesList>
				<Item>
					<ID>1000</ID><!-- 收藏ID(0为默认收藏) -->
					<Name>新浪</Name><!-- 收藏名称 -->
					<URL>http://www.sina.com.cn</URL><!-- 网址 -->
					<Create_Date>2009-12-29 16:52:48</Create_Date><!-- 收藏时间 -->
					<Create_IP>124.205.27.50</Create_IP><!-- 收藏IP -->
				</Item>
				<Item/>
			</FavoritesList>
			*/
			TCHAR szLink[MAX_PATH] = {0};
			DWORD dwType = REG_SZ;
			DWORD dwSize = MAX_PATH;
			HRESULT hRes = E_FAIL;  
			if (ERROR_SUCCESS == SHGetValue(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), TEXT("Favorites"), &dwType, szLink, &dwSize))
			{
				PathAddBackslash(szLink);
			}
			CString strLink = szLink;

			CString strXml = http->ResponseText;
			CMarkup xml;
			if (xml.SetDoc(strXml))
			{
				xml.ResetPos();

				if (xml.FindElem(TEXT("FavoritesList")) && xml.IntoElem())
				{
					while (xml.FindElem(TEXT("Item")) && xml.IntoElem())
					{
						CString Name, Url;
						if (xml.FindElem(TEXT("Name")))
							Name = xml.GetData();
						if (xml.FindElem(TEXT("URL")))
							Url = xml.GetData();
						if (!Name.IsEmpty() && !Url.IsEmpty())
						{
							CString szUrlFileName = strLink + Name + TEXT(".url");
							if (S_OK == CreateShortcutToURL(Url, szUrlFileName))
							{
								m_favlst.push_back(szUrlFileName);
							}
						}
						xml.OutOfElem();
					}
				}
			}
		}

	}
	catch (...) { }
	delete []pszCookie;
}

void CPersonMgr::GetPersonalizationSet()
{
	LPCTSTR pszCookie = GetCookie();
	if (pszCookie == NULL)
		return ;
	try
	{
		using namespace WinHttp;
		IWinHttpRequestPtr http(__uuidof(WinHttpRequest));

		http->Open(TEXT("POST"), _bstr_t(TEXT("http://user.cgi.i8cn.com/Personal/PersonalizationGet.koc")), VARIANT_FALSE);
		http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("Keep-Alive"));
		http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));
		http->SetRequestHeader(_bstr_t("Cookie"), _bstr_t(pszCookie));
		TCHAR szBuf[100] = {0};
		_stprintf(szBuf, TEXT("UserID=%d"), m_dwUserId);
		http->Send(szBuf);
		if (200 != http->Status)
			_com_raise_error(0);
		else
		{
			/*
			<Personalization>
				<Item>
					<Name>屏幕分辨率</Name><!-- 设置项名称-->
					<Remark>宽*高</Remark><!-- 备注说明 -->
					<Code>Resolution</Code><!-- 设置项编码(读取时以此为主关键) -->
					<Value>800*600</Value><!-- 设置值 -1:为默认值 -->
				</Item>
				<Item/>
			</Personalization>
			*/
			CString strXml = http->ResponseText;
			CMarkup xml;
			if (xml.SetDoc(strXml))
			{
				xml.ResetPos();
				if (xml.FindElem(TEXT("Personalization")) && xml.IntoElem())
				{
					while (xml.FindElem(TEXT("Item")) && xml.IntoElem())
					{
						if (xml.FindElem(TEXT("Code")))
						{
							CString szName = xml.GetData();
							if (xml.FindElem(TEXT("Value")))
							{
								CString szValue = xml.GetData();
								if (szValue.CompareNoCase(TEXT("-1")) != 0)
								{
									if (szName.CompareNoCase(TEXT("Resolution")) == 0)
									{
										CString szWidth, szHeight;
										AfxExtractSubString(szWidth, szValue, 0, TEXT('*'));
										AfxExtractSubString(szHeight, szValue, 1, TEXT('*'));
										int nWidth = _ttoi(szWidth);
										int nHeight = _ttoi(szHeight);
										ChangeDisplaySetting(nWidth, nHeight);
									}
									else if (szName.CompareNoCase(TEXT("MouseSpeed")) == 0)
									{
										int nNewSpeed = 2 * _ttoi(szValue);
										ChangeMouseSpeed(nNewSpeed);
									}
									else if (szName.CompareNoCase(TEXT("InputMethod")) == 0)
									{
										xml.ResetMainPos();
										if (xml.FindElem(TEXT("Remark")))
										{
											CString str = xml.GetData();
											InstallInput(str);
										}
									}
								}
							}
						}
						xml.OutOfElem();
					}
				}
			}
		}

	}
	catch (...) { }
	delete []pszCookie;
}

void CPersonMgr::ChangeDisplaySetting(int nWidth, int nHeight)
{
	DEVMODE DevMode = {0};
	int     nIndex = 0;
	BOOL    bCanChange = FALSE;
	while(EnumDisplaySettings(NULL, nIndex++, &DevMode))
	{
        if (DevMode.dmPelsWidth == nWidth
			&& DevMode.dmPelsHeight == nHeight)
        {
			bCanChange = TRUE;
			break;
        }
		ZeroMemory(&DevMode, sizeof(DEVMODE));
	}
	if (!bCanChange) return;

    ZeroMemory(&DevMode, sizeof(DEVMODE));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);
	if (DevMode.dmPelsWidth != nWidth || DevMode.dmPelsHeight != nHeight)
	{
		DevMode.dmPelsWidth  = nWidth;
		DevMode.dmPelsHeight = nHeight;
		DevMode.dmSize = sizeof(DEVMODE);
		DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		if ( DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&DevMode, 0))
		{
			try
			{
				CWnd* pWnd = AfxGetMainWnd();
				if (pWnd != NULL && pWnd->m_hWnd != NULL)
				{
					::PostMessage(AfxGetMainWnd()->m_hWnd, WM_THEMECHANGED, 0, 0);
				}
			}
			catch (...) {}
		}
	}
}

void CPersonMgr::ChangeMouseSpeed(int nSpeed)
{
	int nCurSpeed = 0;
	BOOL bSuccess = SystemParametersInfo(SPI_GETMOUSESPEED, NULL, &nCurSpeed, NULL);
	if (nSpeed != nCurSpeed)
	{
		SystemParametersInfo(SPI_SETMOUSESPEED, NULL, (PVOID)nSpeed, NULL);
	}
}

#include "UptPrgDlg.h"

void CPersonMgr::InstallInput(CString str)
{
	//gid=40724表示输入法安装工具
	tagGameInfo* pGameInfo = AfxGetDbMgr()->FindGameInfo(40724);
	if (pGameInfo != NULL)
	{
		tagGameInfo* pGame = new tagGameInfo;
		*pGame = *pGameInfo;
		pGameInfo->Param = str;
		CUptPrgDlg::RunGame(pGame);
		delete pGame;
	}
}

void CPersonMgr::DeleteFavOfAdded()
{
	std::vector<CString>::iterator it = m_favlst.begin();
	for (; it != m_favlst.end(); it++)
	{
		DeleteFile((*it));
	}
	m_favlst.clear();
}

UINT __stdcall CPersonMgr::ThreadProc(LPVOID lpVoid)
{
	CPersonMgr* pThis = reinterpret_cast<CPersonMgr*>(lpVoid);
	HANDLE hTimer = CreateEvent(NULL, FALSE, TRUE, NULL);
	HANDLE hEvt[2] = {pThis->m_hExited, hTimer};

	CoInitialize(NULL);
	while (1)
	{
		DWORD dwRet = WaitForMultipleObjects(_countof(hEvt), hEvt, FALSE, 10 * 60 * 1000);
		if (dwRet == WAIT_TIMEOUT || dwRet == WAIT_OBJECT_0 + 1)
		{
			DWORD dwOldId = pThis->m_dwUserId;
			pThis->GetPersonState();
			if (pThis->m_dwUserId != dwOldId)
			{
				pThis->GetPersonFavorite();

				if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
					break;

				pThis->GetPersonalizationSet();
			}
			break; //开机获取一次后就退出，不再每10分钟获取
		}
		else
		{
			break;
		}

	}
	CloseHandle(hTimer);
	CoUninitialize();

	return 0;
}