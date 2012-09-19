#ifndef _i8desk_tool_h_
#define _i8desk_tool_h_

#include "../include/NetLayer.h"
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

#pragma comment(lib, "shell32.lib")


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

	template <typename t> 
	void SDG_Delete(t* & p)
	{
		delete p;
		p = NULL;
	}

	template <typename t> 
	void SDG_Delete_Array(t* & p)
	{
		delete[] p;
		p = NULL;
	}
	DWORD SDG_GetServerIp(BOOL bFind = FALSE);
	void  SDG_GetAppPath(LPSTR lpszPath, BOOL bAddBackslash);
	void  SDG_WriteServerIp(char* ip);
	BOOL  SDG_GetLastWriteTime(char* szFileName, SYSTEMTIME& lastWrite, BOOL isgmt);
	bool  SDG_CompareSysTime(SYSTEMTIME& t1, SYSTEMTIME& t2);
	BOOL  SDG_CreateDirectory(LPCSTR lpszDir, int offset = 0);
	BOOL  SDG_DeleteDirectory(LPCSTR lpszDir,__int64* DelSize, LPCSTR pExcludedir = NULL);
	BOOL  SDG_KillProcess(file_info& info);
	BOOL  SDG_StopService(file_info& info);
	BOOL  SDG_StartService(file_info& info);
	BOOL  SDG_RunExe(string strfile,string strPara);
	void  SDG_RebootComputer();
	void  SDG_SleepEx(char chDriver, DWORD dwSecond = 0);

	void  SDG_DeleteBootDirOrFile(LPCSTR lpszFileName, BOOL isFile);
	void  splitString(string strSource, vector<string>& plugs, char ch='|');
	void WriteI8deskProtocol();//for BarOnline.exe writeing I8desk protocel info to registry.
	void ProcessFile();////·À»úÆ÷¹·´¦Àí¡£ROBOT_DOGVirus 
	BOOL EnableDebugPrivilege();


// 	#define NET_STATUS_FAIL					1
// 
// 	#define NETCMD_VDISK_MARK		0x0100
// 	#define NETCMD_VDISK_GETALLVDISK		0x0101
// 
// 	#define NETCMD_SYSOPTION_MARK	0x0200
// 	#define NETCMD_SYSOPTION_GETALLOPTION	0x0201
// 	#define NETCMD_SYSOPTION_GETDELLIST		0x0202
// 	#define NETCMD_SYSOPTION_GETBOOTRUN		0x0203
// 	#define NETCMD_SYSOPTION_GETFAVORITEINFO 0x0205
// 	#define NETCMD_SYSOPTION_DOWNSKINFILE	0x0206
// 	#define NETCMD_SYSOPTION_BOOTCLEANDIR	0x0207
// 	#define NETCMD_SYSOPTION_GETTASKINFO	0x0208
// 
// 	#define NETCMD_GAMEMGR_MARK		0x0300
// 	#define NETCMD_GAMEMGR_GETALLGAMEINFO	0x0301
// 	#define NETCMD_GAMEMGR_UPLOADCLICKNUMS	0x0302
// 	#define NETCMD_GMAEMGR_ADDGAME			0x0303
// 	#define NETCMD_GAMEMGR_GETLNKGAME		0x0305
// 	#define NETCMD_GAMEMGR_DELGID			0x0306
// 	#define NETCMD_GAMEMGR_GETGIDINFO		0x0307
// 
// 	#define NETCMD_USERMGR_MARK		0x0400
// 	#define NETCMD_USERMGR_ADDUSER			0x0401
// 	#define NETCMD_USERMGR_CHECKUSER		0x0402
// 	#define NETCMD_USERMGR_DOWNFILE			0x0403
// 	#define NETCMD_USERMGR_UPFILE			0x0404	
// 	#define NETCMD_USERMGR_DELFILE			0x0405
// 	#define NETCMD_USERMGR_MODIFYPASSWORD	0x0406
// 	#define NETCMD_USERMGR_QUIT				0x0407
// 
// 	#define NETCMD_GETSERVER_IP				0x0501
// 
// 	#define NETCMD_UPDATE_MARK		0x0600
// 	#define NETCMD_UPDATE_GETLIST			0x0601
// 	#define NETCMD_UPDATE_DOWNLOAD			0x0602
}


#endif