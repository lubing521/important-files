#ifndef _i8desk_tool_h_
#define _i8desk_tool_h_


#ifdef UNICODE
	#define tsprintf swprintf_s
	#define tstrcpy  wcscpy_s
	#define tstrcat  wcscat_s
    #define GetRootDir GetRootDirW
#else 
	#define tsprintf sprintf_s
	#define tstrcpy  strcpy_s
	#define tstrcat  strcat_s
    #define GetRootDir GetRootDirA
#endif

#include <assert.h>
#include "cltsvrdef.h"
#include "frame.h"

using namespace std;
namespace i8desk
{
	#define SDG_VERSION 0x0109

	typedef struct _file_info
	{
		char	path[MAX_PATH];
		char	file[MAX_PATH];
		DWORD	size;
		DWORD	flag;
		BOOL	needreboot;
		SYSTEMTIME time;
		_file_info()
		{
			path[0] = 0;
			file[0] = 0;
			size = 0;
			flag = 0;
			needreboot = FALSE;
			memset(&time, 0, sizeof(time));
		}
	}file_info;

	enum _file_type
	{
		_file_exe = 1,
		_file_service = 2,	
		_file_sys = 3,
		_file_normal = 4,	
		_file_self = 5
	};

	void  SDG_GetAppPath(LPTSTR lpszPath, BOOL bAddBackslash);
	bool  SDG_CompareSysTime(SYSTEMTIME& t1, SYSTEMTIME& t2);
	BOOL  SDG_DeleteDirectory(LPCTSTR lpszDir,__int64* DelSize, LPCTSTR pExcludedir = NULL);
	void  SDG_RebootComputer();

	void  splitString(stdex::tString strSource, vector<stdex::tString>& plugs, TCHAR ch='|');
	void  WriteI8deskProtocol();//for BarOnline.exe writeing I8desk protocel info to registry.
	BOOL  EnableDebugPrivilege();

	DWORD GetBufferCRC( const void * pInBuffer, DWORD dwSize);
	DWORD GetFileCRC(LPCTSTR pFilePath);

	std::string GetRootDirA();
	std::wstring GetRootDirW();

	stdex::tString GetSystemDir();
	stdex::tString GetProgramDir();
	stdex::tString GetWindowsDir();	
	stdex::tString GetDocumentDir();
	stdex::tString GetRungameExe();
	stdex::tString GetLocalGamePath(DWORD dwGid);
	DWORD  GetDriverFreeSize(TCHAR chDrv);
	BOOL   StartProcess(LPCTSTR lpszFileName, LPCTSTR lpszParam, 
		LPCTSTR lpszCompare, LPCTSTR lpszReserve, 
		BOOL bCheckExist = TRUE,
        BOOL bTerminateExits = FALSE, BOOL bParamPrefix = TRUE);
    BOOL   StartProcess(LPCTSTR lpszParam, BOOL bUseRunGame = TRUE);
	BOOL   ShutdownComputer(BOOL bReboot = TRUE);
	bool   EnsureDllExist(LPCTSTR lpFileName, DWORD dwResId);
	//¼ì²â·þÎñ
	BOOL   EnsureServiceExist(LPCTSTR lpFileName, LPCTSTR lpServiceName, DWORD dwStartType = SERVICE_AUTO_START);
	stdex::tString GetFileVersion(const stdex::tString& strFileName);

	BOOL   CheckConnect(DWORD ip, WORD port);
	stdex::tString ConvertIPToString( DWORD dwIp );
    BOOL   IsDriverExist(TCHAR chDriver);
	HANDLE GetProcessHandle(LPCTSTR lpProcessName);
	stdex::tString GetCookieDateString();

    BOOL CreateShortcut(const vector<STShortcut>& vctShorcut, ILogger* pLog);
    int ClearShortcut(LPCTSTR pszExeFile, LPCTSTR pszParamPrefix);

    //write mini dump file.
    LONG WINAPI UnHandledExceptionFilter(PEXCEPTION_POINTERS pExcept);

    //get application path.
    stdex::tString GetAppPath();
    DWORD GetLocalIP();
    bool KillProcessEx(LPCTSTR lpszSerive);
}

#endif