#include "stdafx.h"
#include "public.h"
#include "idxfile.h"
#include "socket.h"
#include "hy.h"
#include "UpdateGame.h"
#include "UpdateGameSvr.h"
#include "forceupdate.h"
#include <algorithm>
#include <process.h>
#include <Shlwapi.h>
#include <Psapi.h>
#include <atlbase.h>
#include <minmax.h>

#include "../../../include/Utility/utility.h"

#ifndef max
#define max std::max
#endif

#pragma comment(lib, "Shlwapi.lib")

struct ErrorInfo_st
{
    UpdateErrorCode enErrCode;
    LPCTSTR         lpszErrInfo;
};
#ifndef ENGLISH_VERSION

ErrorInfo_st C_ERR_INFO[] =
{
    {UPT_ERR_DIRNOTOK,		_T("目录名不正确:服务端路径 %s 客户端路径 %s.")},
    {UPT_ERR_CREATEDIR,		_T("创建目录失败:%s.")},
    {UPT_ERR_INNERERR,		_T("内部错误:%d.")},
    {UPT_ERR_LOADDEVICE,	_T("加载虚拟设备出错.")},
    {UPT_ERR_GETGAMEINFO,	_T("游戏索引文件丢失.")},
    {UPT_ERR_DWNIDX,		_T("下载索引失败.")},
    {UPT_ERR_DISKNOROOM,	_T("磁盘空间不足,可用%dM,需要%dM.")},
    {UPT_ERR_PARSEIDX,		_T("解析索引失败:%d.")},
    {UPT_ERR_CONNFAIL,		_T("连接服务器失败,请手动启动游戏.")},
    {UPT_ERR_DWNFAIL,		_T("下载数据块失败:%s.")},
    {UPT_ERR_CRCERR,		_T("数据块crc不同:%s,己提交三层自动修复,请稍后重试.")},
    {UPT_ERR_FILEOPEN,    	_T("打开文件失败:%s:%d.")},
    {UPT_ERR_FILEWRITE,	    _T("写文件失败:%s:%d.")},
    {UPT_ERR_FILEPOPDATA, 	_T("接受数据失败:%s.")},
    {UPT_ERR_FILECLOSE,   	_T("关闭文件失败:%s:%d.")},
};
	#define INFO_STR_GETGAMEINFO	L"取游戏相关信息."
	#define	INFO_STR_DOWNIDXFILE	L"下载索引文件."
	#define	INFO_STR_PRASEIDXFILE	L"解析索引文件."
	#define	INFO_STR_DELETEGAME		L"空间不足,清除游戏:%s."
#else

ErrorInfo_st C_ERR_INFO[] =
{
    {UPT_ERR_DIRNOTOK   , _T("invalid directory:%s-%s.")},
    {UPT_ERR_CREATEDIR  , _T("create directory fail:%s.")},
    {UPT_ERR_INNERERR   , _T("inner error:%d.")},
    {UPT_ERR_LOADDEVICE , _T("load virtual device fail.")},
    {UPT_ERR_GETGAMEINFO, _T("game index file lost.")},
    {UPT_ERR_DWNIDX     , _T("download index fail.")},
    {UPT_ERR_DISKNOROOM , _T("no disk room,free size:%dM, need size:%dM.")},
    {UPT_ERR_PARSEIDX   , _T("parse index fail:%d.")},
    {UPT_ERR_CONNFAIL   , _T("connect server fail, please run game.")},
    {UPT_ERR_DWNFAIL    , _T("download block fail:%s.")},
    {UPT_ERR_CRCERR     , _T("block crc error,%s, pleasess retry.")},
    {UPT_ERR_FILEOPEN   , _T("fail to open file:%s:%d.")},
    {UPT_ERR_FILEWRITE  , _T("fail to write file:%s:%d.")},
    {UPT_ERR_FILEPOPDATA, _T("fail to pop data:%s.")},
    {UPT_ERR_FILECLOSE  , _T("fail to close file:%s:%d.")},
};
	#define INFO_STR_GETGAMEINFO	L"get game information"
	#define	INFO_STR_DOWNIDXFILE	L"down index file."
	#define	INFO_STR_PRASEIDXFILE	L"prase index file."
	#define	INFO_STR_DELETEGAME		L"delete game:%s."
#endif

#define SAVE_DISK_ROOMSIZE		(1LL * 1024 * 1024 * 1024)

template<>
struct _func_<tagIdxBlock*>
{
	void operator()(tagIdxBlock*& pBlock) const
	{
		Delete_Array(pBlock->pdata);
	}
};

class CGameUpdate
{
private:
	typedef struct tagGameInfo
	{
		DWORD	Gid;						//游戏GID
		DWORD	Pid;						//游戏关联的GID
		char	Name[MAX_PATH];				//游戏名
		char	IdcClass[MAX_PATH];			//游戏类别
		char	GameExe[MAX_PATH];			//游戏官方EXE
		char	Param[MAX_PATH];			//游戏运行参数
		DWORD	Size;						//游戏大小,以K为单位
		DWORD	DeskLnk;					//游戏是否显示到桌面快捷方式
		DWORD	Toolbar;					//游戏是否显示到菜单的工具栏上
		char	SvrPath[MAX_PATH];			//游戏在服务器上的路径
		char	CliPath[MAX_PATH];			//游戏在客户机上的路径
		DWORD	dwDeleteFlg;				//游戏空间不足时允许删除
		char	MatchFile[MAX_PATH];		//游戏自动搜索特征文件
		char	SaveFilter[MAX_PATH];		//游戏存档信息
		DWORD	IdcUptDate;					//游戏在中心服务器的更新时间
		DWORD	IdcVer;						//游戏在中心服务器上的版本
		DWORD	SvrVer;						//游戏在网吧服务器上的版本
		DWORD	AutoUpt;					//游戏是否自动从服务器上更新
		DWORD	I8Play;						//游戏需要强制更新
		DWORD	IdcClick;					//游戏全网点率
		DWORD	SvrClick;					//游戏网吧点率
        DWORD	RunType;					//游戏运行方式
        char	Vid[MAX_PATH];				//游戏所在虚拟盘ID
		DWORD	Status;						//是否是本地游戏
        char    Comment[MAX_PATH];			//备注
        DWORD   dwRepair;                   //更新游戏时用CRC比较
	}tagGameInfo;
public:
	CGameUpdate();
	~CGameUpdate();
	/*
	2011-05-23
	根据11版需求，增加tid参数。tid由客户端从开机任务及心跳信息中获取。
	2011-06-03
	增加type参数，0表示推送，1表示同步，2表示内网更新.
	*/
	bool StartUpdate(DWORD gid, LPCWSTR tid, LPCWSTR src, LPCWSTR dst, 
					 DWORD flag, LPCSTR svrip, LPCSTR cliip, DWORD maxspeed, DWORD type);

	// 暂停更新
	void PauseUpdate()
	{
		m_eventRun.ResetEvent();
	}
	// 恢复更新
	void ResumeUpdate()
	{
		m_eventRun.SetEvent();
	}

	void GetUpdateStatus(tagUpdateStatus& UpdateStuas);
	void StopUpdate();
    void SetConnectCallBack(OnConnect_pfn pfnCallBack, void* pCallBackParam);
private:
	void  _cdecl SetErrorInfo(UpdateErrorCode enErrCode, ...);
	inline void SetUpdateStatus(DWORD status)
	{
		AutoLock lock(m_lockStatus);
		m_UpdateStuats.dwStatus = status;
	}
	inline void SetUpdateInfo(LPCWSTR formater, ...)
	{
		AutoLock lock(m_lockStatus);
		va_list marker;
		va_start(marker, formater);
		vswprintf_s(m_UpdateStuats.awInfo, formater, marker);
		va_end(marker);
	}
	static bool __stdcall CallBack(void* pcookie, LPCWSTR lpInfo)
	{
		CGameUpdate* pThis = reinterpret_cast<CGameUpdate*>(pcookie);
		if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
			return false;

		AutoLock lock(pThis->m_lockStatus);
		wcscpy_s(pThis->m_UpdateStuats.awInfo, lpInfo);
		return true;
	}
	inline bool  CompareIdxfile(__int64& qUpdateBytes, IdxMap& mFiles);
	inline void  DeleteIdxfile();
	inline bool  UpdateBefore(__int64 qUpdateBytes);
	inline void  UpdateAfter();
    inline DWORD OpenFile(tagIdxFile* pFile, utility::CAutoFile& clFile, BOOL& bUseNoBuf);
	inline DWORD WriteFile(HANDLE hFile, tagIdxBlock* pBlock, BOOL bUseNoBuf, LPOVERLAPPED pov);
	inline DWORD CloseFile(utility::CAutoFile& clFile, tagIdxFile* pFile, BOOL bUseNoBuf);
	static UINT __stdcall WorkThread(LPVOID lpVoid);
	static UINT __stdcall RecvThread(LPVOID lpVoid);
	static UINT __stdcall CalCrcThread(LPVOID lpVoid);
	static UINT __stdcall WriteThread(LPVOID lpVoid);
	void LimitSpeed(DWORD nStartTime, DWORD& nLastTime, DWORD& nTotalBytes, DWORD nRecvBytes);
	void GetAllGameList(std::vector<tagGameInfo*>& gameList);
	void DeleteGame(LPCWSTR dir);
    BOOL CheckState();
private:
	DWORD m_dwGid;
	DWORD m_dwMaxSpeed;
	DWORD m_dwUptFlag;
	DWORD m_dwUptType;
	typedef async::thread::AutoCriticalSection	MetuxLock;
	typedef async::thread::AutoLock<MetuxLock>	AutoLock;
	typedef async::thread::AutoEvent			Event;

	TCHAR m_szTid[MAX_GUID_LEN];
	std::string  m_strSvrIp;
	std::string  m_strCliIp;
	std::wstring m_strSvrPath;
	std::wstring m_strCliPath;
	std::wstring m_strDevPath;
	MetuxLock m_lockStatus;
	tagUpdateStatus m_UpdateStuats;

	Event		m_eventRun;				// 运行事件

	//记录新索引的数据信息
	char*  m_pIdxfile;
	DWORD  m_nIdxfileSize;
	DWORD  m_dwSvrIdxVer;
	FILETIME m_ftIdxfile;
	std::vector<tagIdxFile*>  m_vFiles;
	std::vector<tagIdxBlock*> m_vBlocks;

	//线程以及共享的数据。
	HANDLE m_hExited;
	HANDLE m_hThread;
	PCMomule<tagIdxBlock*>* m_pipe;
	PCMomule<tagIdxBlock*>* m_pipe2;

	CSocket m_Server;
    OnConnect_pfn m_pfnConnectCallBack;
    void* m_pConnectCallBackParam;
};

CGameUpdate::CGameUpdate() : m_dwGid(-1), m_dwMaxSpeed(0), m_dwUptFlag(0), m_pIdxfile(NULL), 
	m_nIdxfileSize(0), m_dwSvrIdxVer(-1), m_hExited(NULL), m_hThread(NULL), 
	m_pipe(NULL), m_pipe2(NULL),m_pfnConnectCallBack(NULL),m_pConnectCallBackParam(NULL)
{
	ZeroMemory(m_szTid, sizeof(m_szTid));
	ZeroMemory(&m_UpdateStuats, sizeof(m_UpdateStuats));
    m_UpdateStuats.cbSize = sizeof(m_UpdateStuats);
}

CGameUpdate::~CGameUpdate()
{
}

bool CGameUpdate::StartUpdate(DWORD gid, LPCWSTR tid, LPCWSTR src, LPCWSTR dst, DWORD flag, LPCSTR svrip, LPCSTR cliip, DWORD maxspeed, DWORD type)
{
	StopUpdate();
    ZeroMemory(&m_UpdateStuats, sizeof(m_UpdateStuats));
    m_UpdateStuats.cbSize = sizeof(m_UpdateStuats);

	m_dwGid = gid;
	m_dwUptType = type;
	if(tid!= NULL && wcslen(tid) < MAX_GUID_LEN)
	{
		_tcscpy_s(m_szTid, CW2T(tid));
	}
	
	if (maxspeed)
		//m_dwMaxSpeed = (maxspeed < 1000) ? 640 : ((maxspeed - 200) * 1000);
    {
        if (maxspeed * .2 < 200)
        {
            m_dwMaxSpeed = max(DWORD(maxspeed * .8)  * 1024, 640);
        }
        else
        {
            m_dwMaxSpeed = (maxspeed - 200)  * 1024;
        }
    }

	//check parameter
	if (src == NULL || dst == NULL || wcslen(src) < 2 || wcslen(dst) < 2 ||
		src[1] != L':' || src[2] != L'\\' || 
		dst[1] != L':' || dst[2] != L'\\')
	{
		SetErrorInfo(UPT_ERR_DIRNOTOK, src, dst);
		return false;
	}

	//save parameter.
	m_strSvrPath = src;	m_strCliPath = dst;	
	if (svrip != NULL)	m_strSvrIp = svrip; 
	if (cliip != NULL)	m_strCliIp = cliip; 
	m_dwUptFlag  = flag;
	if (*m_strSvrPath.rbegin() != L'\\')
		m_strSvrPath += L"\\";
	if (*m_strCliPath.rbegin() != L'\\')
		m_strCliPath += L"\\";

	//convert and create directory.
	if (!hy_LoadVirtualDevice(m_strCliPath[0]))
	{
		SetErrorInfo(UPT_ERR_LOADDEVICE);
		return false;
	}
	m_strDevPath = hy_ConvertPath(m_strCliPath);
	if (!hy_CreateDirectory(m_strCliPath) || !hy_CreateDirectory(m_strDevPath))
	{
		SetErrorInfo(UPT_ERR_CREATEDIR, m_strCliPath.c_str());
		return false;
	}

	//create update work thread.
	m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExited == NULL)
	{
		SetErrorInfo(UPT_ERR_INNERERR, GetLastError());
		return false;
	}

	// 暂停事件
	if( !m_eventRun.Create(0, TRUE, TRUE) )
	{
		SetErrorInfo(UPT_ERR_INNERERR, GetLastError());
		return false;
	}

	m_pipe  = new PCMomule<tagIdxBlock*>(m_hExited);
	m_pipe2 = new PCMomule<tagIdxBlock*>(m_hExited);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
	if (m_hThread == NULL)
	{
		SetErrorInfo(UPT_ERR_INNERERR, GetLastError());
		StopUpdate();
		return false;
	}

	return true;
}

void CGameUpdate::GetUpdateStatus(tagUpdateStatus& UpdateStuas)
{
	AutoLock lock(m_lockStatus);
	UpdateStuas = m_UpdateStuats;
}

void CGameUpdate::StopUpdate()
{
	if (m_hThread != NULL && m_hExited != NULL)
	{
		SetEvent(m_hExited);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		CloseHandle(m_hExited);
	}
	m_hThread = NULL;
	m_hExited = NULL;
    if (m_eventRun.IsOpen())
    {
        m_eventRun.Close();
    }
}

void _cdecl CGameUpdate::SetErrorInfo(UpdateErrorCode enErrCode, ...)
{
	AutoLock lock(m_lockStatus);
    if (m_UpdateStuats.dwStatus == UPDATE_STATUS_ERROR)
    {
        return;
    }
	m_UpdateStuats.dwStatus = UPDATE_STATUS_ERROR;
	m_UpdateStuats.dwLeftTime = 0;
    m_UpdateStuats.enErrCode = enErrCode;

    TCHAR atErrInfo[MAX_PATH] = _T("Unknown Error.");
    for (int i = 0; i < _countof(C_ERR_INFO); ++i)
    {
        if (C_ERR_INFO[i].enErrCode == enErrCode)
        {
            _tcscpy_s(atErrInfo, C_ERR_INFO[i].lpszErrInfo);
            break;
        }
    }

	va_list marker;
	va_start(marker, enErrCode);
	vswprintf_s(m_UpdateStuats.awInfo, atErrInfo, marker);
	va_end(marker);

	//notify thread exit.
	if (m_hExited != NULL)
		SetEvent(m_hExited);
}

void  CGameUpdate::DeleteIdxfile()
{
	//delete local idxfile.
	std::wstring idxfile = m_strCliPath + INDEX_FILE_NAMEW;
	SetFileAttributesW(idxfile.c_str(), FILE_ATTRIBUTE_NORMAL);
	DeleteFileW(idxfile.c_str());

	//delete virtual device idxfile.
	if (_wcsicmp(m_strDevPath.c_str(), m_strCliPath.c_str()) != 0)
	{
		std::wstring devidxfile = m_strDevPath + INDEX_FILE_NAMEW;
		SetFileAttributesW(devidxfile.c_str(), FILE_ATTRIBUTE_NORMAL);
		DeleteFileW(devidxfile.c_str());
	}
}

bool  CGameUpdate::UpdateBefore(__int64 qUpdateBytes)
{
	//check disk free room.
	wchar_t szLog[1024] = {0};
	qUpdateBytes += SAVE_DISK_ROOMSIZE;
	LARGE_INTEGER liSize = {0};
	hy_GetDiskRoomSize(m_strCliPath[0], &liSize);
	if (liSize.QuadPart >= qUpdateBytes)
		return true;
	
	//删除游戏，保证空间足够。
	if (m_dwUptFlag & UPDATE_FLAG_DELETE_GAME)
	{
		SetUpdateStatus(UPDATE_STATUS_DELETE_GAME);
		std::vector<tagGameInfo*> list;
		GetAllGameList(list);
		for (size_t idx=0; idx<list.size(); idx++)
		{
			if (!CheckState())
			{
				std::for_each(list.begin(), list.end(), Delete_Pointer<tagGameInfo>);
				return false;
			}

			tagGameInfo* pGame = list[idx];
			if (PathFileExistsA(pGame->CliPath) && pGame->CliPath[0] == m_strCliPath[0] &&
				lstrcmpiW(_bstr_t(pGame->CliPath), m_strCliPath.c_str()) != 0)
			{
				std::wstring str(_bstr_t(pGame->CliPath));
				std::wstring str2 = hy_ConvertPath(str);
				
				SetUpdateInfo(INFO_STR_DELETEGAME, str.c_str());

				DeleteGame(str2.c_str());
				hy_DyncaRefreDriver(str[0]);

				hy_GetDiskRoomSize(m_strCliPath[0], &liSize);
				if (liSize.QuadPart >= qUpdateBytes)
				{
					std::for_each(list.begin(), list.end(), Delete_Pointer<tagGameInfo>);
					return true;
				}
				else
				{
					swprintf_s(szLog, L"i8desk: free size:%dM, need size:%dM.", (DWORD)(liSize.QuadPart / 1000 / 1000),
						(DWORD)(qUpdateBytes / 1000 / 1000));
					OutputDebugStringW(szLog);
				}
			}
		}
		std::for_each(list.begin(), list.end(), Delete_Pointer<tagGameInfo>);
	}
	//空间不足，返回错误。
	SetErrorInfo(UPT_ERR_DISKNOROOM, (DWORD)(liSize.QuadPart / 1000 / 1000), (DWORD)(qUpdateBytes / 1000 / 1000));
	return false;
}

void  CGameUpdate::UpdateAfter()
{
	//当真正更新了文件(索引在更新前己经被删除)，
	//以及版本号不相同（有可能不需要更新(版本号不相同索引也被删除)）时，
	//都需要写索引文件，以及动态还原。
	std::wstring idxfile = m_strCliPath + INDEX_FILE_NAMEW;
	if (!PathFileExistsW(idxfile.c_str()))
	{
		//write local idxfile.
		HANDLE hFile = CreateFileW(idxfile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwWriteBytes = 0;
			::WriteFile(hFile, m_pIdxfile, m_nIdxfileSize, &dwWriteBytes, NULL);
			SetFileTime(hFile, NULL, NULL, &m_ftIdxfile);
			CloseHandle(hFile);
		}

		//write virtual device idxfile.
		if (_wcsicmp(m_strDevPath.c_str(), m_strCliPath.c_str()) != 0)
		{
			std::wstring devidxfile = m_strDevPath + INDEX_FILE_NAMEW;
			HANDLE hFile = CreateFileW(devidxfile.c_str(), GENERIC_WRITE, 0, NULL, 
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwWriteBytes = 0;
				::WriteFile(hFile, m_pIdxfile, m_nIdxfileSize, &dwWriteBytes, NULL);
				SetFileTime(hFile, NULL, NULL, &m_ftIdxfile);
				CloseHandle(hFile);
			}
		}

		//notify server game update finished.
		m_Server.ReportUpdateFinished(m_dwGid, m_dwSvrIdxVer, m_strCliPath.c_str());

		//hy dynamic execute update directory.
		hy_ExueteUpdateDir(m_strCliPath.c_str());
	}
}

bool CGameUpdate::CompareIdxfile(__int64& qUpdateBytes, IdxMap& mFiles)
{
	DWORD ret = ERROR_SUCCESS;
	qUpdateBytes = 0;

	//recv game info.(version,idxfile size)
	SetUpdateInfo(INFO_STR_GETGAMEINFO);
	if (!m_Server.GetGameInfo(m_dwGid, m_strSvrPath.c_str(), m_dwSvrIdxVer, m_nIdxfileSize, m_ftIdxfile))
	{
		SetErrorInfo(UPT_ERR_GETGAMEINFO);
		m_Server.ReportGameIdxError(m_dwGid);

		return false;
	}

	//compare game block.
	if (m_dwUptFlag & UPDATE_FLAG_FORCE_UPDATE)
	{
		//recv server idxfile.
		SetUpdateInfo(INFO_STR_DOWNIDXFILE);
		if (!m_Server.RecvIdxfile(m_strSvrPath.c_str(), m_pIdxfile, m_nIdxfileSize))
		{
			SetErrorInfo(UPT_ERR_DWNIDX);
			return false;
		}

		//parse server idxfile.
		SetUpdateInfo(INFO_STR_PRASEIDXFILE);
		ret = Idxfile_Parse(m_pIdxfile, m_nIdxfileSize, m_vFiles, m_vBlocks, mFiles);
		if (ret != ERROR_SUCCESS)
		{
			SetErrorInfo(UPT_ERR_PARSEIDX, ret);
			return false;
		}

		//set block to updated.
		for (std::vector<tagIdxFile*>::iterator it = m_vFiles.begin(); it != m_vFiles.end(); it++)
		{
			(*it)->update = 1;
		}
		for (std::vector<tagIdxBlock*>::iterator it = m_vBlocks.begin(); it != m_vBlocks.end(); it++)
		{
			(*it)->update = 1;
			qUpdateBytes += (*it)->blksize;
		}
	}
	else
	{
		//先对比版本号
		if (!(m_dwUptFlag & UPDATE_FLAG_QUICK_COMPARE) && m_dwSvrIdxVer != -1 && 
			m_dwSvrIdxVer == Idxfile_GetVersion((m_strCliPath + INDEX_FILE_NAMEW).c_str()))
		{
			//version equal,don't update and delete morefile.
			if (m_dwUptFlag & UPDATE_FLAG_DELETE_MOREFILE)
				m_dwUptFlag &= ~UPDATE_FLAG_DELETE_MOREFILE;
			return true;
		}
		else
		{
			//快速对比是，是根据磁盘上的文件对比。因此把索引删除。
			if (m_dwUptFlag & UPDATE_FLAG_QUICK_COMPARE)
			{
				std::wstring file = m_strCliPath + INDEX_FILE_NAMEW;
				SetFileAttributesW(file.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFileW(file.c_str());
			}
			//recv server idxfile.
			SetUpdateInfo(INFO_STR_DOWNIDXFILE);
			if (!m_Server.RecvIdxfile(m_strSvrPath.c_str(), m_pIdxfile, m_nIdxfileSize))
			{
				SetErrorInfo(UPT_ERR_DWNIDX);
				return false;
			}

			//parse server idxfile.
			SetUpdateInfo(INFO_STR_PRASEIDXFILE);
			ret = Idxfile_Parse(m_pIdxfile, m_nIdxfileSize, m_vFiles, m_vBlocks, mFiles);
			if (ret != ERROR_SUCCESS)
			{
				SetErrorInfo(UPT_ERR_PARSEIDX, ret);
				return false;
			}

			//对比两个索引,如果本地索引不存在，或者文件不正确，则扫描磁盘文件。
			std::vector<tagIdxFile*>  m_LocalVFiles;
			std::vector<tagIdxBlock*> m_LocalBlocks;
			IdxMap m_LocalMFiles;
			std::wstring idxfile = m_strCliPath + INDEX_FILE_NAMEW;
			if (ERROR_SUCCESS == Idxfile_Parse(idxfile.c_str(), m_LocalVFiles, m_LocalBlocks, m_LocalMFiles))
				Idxfile_Compare(mFiles, m_vBlocks, m_LocalMFiles, m_LocalBlocks, qUpdateBytes, CallBack, this);
			else
				Idxfile_Compare(m_strCliPath.c_str(), m_vFiles, m_vBlocks, qUpdateBytes, CallBack, this);
			std::for_each(m_LocalVFiles.begin(), m_LocalVFiles.end(), Delete_Pointer<tagIdxFile>);
			std::for_each(m_LocalBlocks.begin(), m_LocalBlocks.end(), Delete_Pointer<tagIdxBlock>);
		}
	}
	//delete idxfile.
	DeleteIdxfile();
	return true;
}

UINT __stdcall CGameUpdate::WorkThread(LPVOID lpVoid)
{
	CGameUpdate* pThis = reinterpret_cast<CGameUpdate*>(lpVoid);
	HANDLE hThread[3] = {0};
	IdxMap mFiles;
	__int64 qUpdateBytes = 0;		//need updated bytes.

	while (1)
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 1000) )
			break;

		//connect server.
		if (pThis->m_Server.ConnectServer(pThis->m_strSvrIp.c_str(), pThis->m_strCliIp.c_str()))
		{
			pThis->SetErrorInfo(UPT_ERR_CONNFAIL);
			return false;
		}
        if (pThis->m_pfnConnectCallBack != NULL)
        {
            (*pThis->m_pfnConnectCallBack)(pThis->m_pConnectCallBackParam, win32::system::GetDesIP(pThis->m_Server));
        }
		
		//step 1:compare idxfile.
		{
			if (!pThis->CheckState())
				break;

			pThis->SetUpdateStatus(UPDATE_STAUTS_COMPARE);
			if (!pThis->CompareIdxfile(qUpdateBytes, mFiles))
				break;
			AutoLock lock(pThis->m_lockStatus);
			pThis->m_UpdateStuats.qNeedsUpdateBytes = qUpdateBytes;
			pThis->m_UpdateStuats.qUpdatedBytes = 0;
		}

		//step 2:down block.
		//if (qUpdateBytes)
		{
			if (!pThis->UpdateBefore(qUpdateBytes))
				break;
			pThis->SetUpdateStatus(UPDATE_STATUS_UPDATE);
			hThread[0] = (HANDLE)_beginthreadex(NULL, 0, RecvThread,	pThis, 0, NULL);
			hThread[1] = (HANDLE)_beginthreadex(NULL, 0, CalCrcThread,	pThis, 0, NULL);
			hThread[2] = (HANDLE)_beginthreadex(NULL, 0, WriteThread,	pThis, 0, NULL);
			if (hThread[0] == NULL || hThread[1] == NULL || hThread[2] == NULL)
			{
				pThis->SetErrorInfo(UPT_ERR_INNERERR, GetLastError());
				break;
			}
			WaitForMultipleObjects(_countof(hThread), hThread, TRUE, INFINITE);
		}

        if (!pThis->CheckState())
            break;

		//step 3:delete more file and directory.
		if (pThis->m_dwUptFlag & UPDATE_FLAG_DELETE_MOREFILE)
		{
			pThis->SetUpdateStatus(UPDATE_STATUS_DELETE_MOREFILE);
			if (mFiles.size())
				Idxfile_DeleteMorefile(pThis->m_strDevPath.c_str(), mFiles, CallBack, pThis);
		}

		pThis->UpdateAfter();
		pThis->SetUpdateStatus(UPDATE_STATUS_FINISH);
		break;
	}

	//free resource.
	pThis->m_Server.CloseServer();
	if (hThread[0] != NULL) CloseHandle(hThread[0]);
	if (hThread[1] != NULL) CloseHandle(hThread[1]);
	if (hThread[2] != NULL) CloseHandle(hThread[2]);
	
	Delete_Pointer(pThis->m_pipe);
	Delete_Pointer(pThis->m_pipe2);
	Delete_Pointer(pThis->m_pIdxfile);

	std::for_each(pThis->m_vFiles.begin(),  pThis->m_vFiles.end(),  Delete_Pointer<tagIdxFile>);
	std::for_each(pThis->m_vBlocks.begin(), pThis->m_vBlocks.end(), Delete_Pointer<tagIdxBlock>);
	
	return 0;
}

UINT __stdcall CGameUpdate::RecvThread(LPVOID lpVoid)
{
	CGameUpdate* pThis = reinterpret_cast<CGameUpdate*>(lpVoid);
	
	DWORD dwStarTime  = GetTickCount();
	DWORD dwLastTime  = GetTickCount();
	DWORD dwTotalBytes = 0;
	for (std::vector<tagIdxFile*>::iterator it = pThis->m_vFiles.begin(); it != pThis->m_vFiles.end(); it++)
	{
		tagIdxFile* pFile = (*it);
		if ((pFile->attr & FILE_ATTRIBUTE_DIRECTORY) || (pFile->update == 0))
			continue;

		HANDLE hFile = INVALID_HANDLE_VALUE;
		bool bFirstBlock = true;
		for (DWORD idx=0; idx<pFile->blknum; idx++)
        {
            if (!pThis->CheckState())
                goto exit;

			tagIdxBlock* pBlock = pThis->m_vBlocks[pFile->crcoff + idx];
			if (pBlock->update != 1)
				continue;
			try 
			{
				pBlock->pdata = new char[ALIGN_SIZE(pBlock->blksize, 0x10000)]; 
			}
			catch (...)
			{
				pThis->SetErrorInfo(UPT_ERR_INNERERR, GetLastError());
				goto exit;
			}

			if (!pThis->m_Server.RecvBlock(pFile->name.c_str(), pFile->size, pBlock->offset, 
				pBlock->blksize, bFirstBlock, pBlock->pdata, pThis->m_hExited))
			{
				if (WaitForSingleObject(pThis->m_hExited, 0) != WAIT_OBJECT_0)
					pThis->SetErrorInfo(UPT_ERR_DWNFAIL, pFile->name.c_str());
				Delete_Array(pBlock->pdata);
				goto exit;
			}
			
			if (!pThis->m_pipe->Push(pBlock))
			{
				Delete_Array(pBlock->pdata);
				goto exit;
			}

			bFirstBlock = false;

			pThis->LimitSpeed(dwStarTime, dwLastTime, dwTotalBytes, pBlock->blksize);
		}
	}
exit:
	return 0;
}

UINT __stdcall CGameUpdate::CalCrcThread(LPVOID lpVoid)
{
	CGameUpdate* pThis = reinterpret_cast<CGameUpdate*>(lpVoid);
	tagIdxBlock* pBlock = NULL;

	for (std::vector<tagIdxFile*>::iterator it = pThis->m_vFiles.begin(); it != pThis->m_vFiles.end(); it++)
	{
		tagIdxFile* pFile = (*it);
		if ((pFile->attr & FILE_ATTRIBUTE_DIRECTORY) || (pFile->update == 0))
			continue;

		for (DWORD idx=0; idx<pFile->blknum; idx++)
		{
			if (!pThis->CheckState())
				goto exit;

			if (pThis->m_vBlocks[pFile->crcoff + idx]->update != 1)
				continue;

			if (!pThis->m_pipe->Pop(pBlock))
				goto exit;

			//compare block crc.
			if (pBlock->crc != Idxfile_BlockCrc((unsigned char*)pBlock->pdata, pBlock->blksize))
			{
//////////////////////////////////////////////////////////////////////////
//for debug.
// 				{
// 					HANDLE hFile = CreateFileW(L"C:\\1.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
// 					if (hFile != INVALID_HANDLE_VALUE)
// 					{
// 						DWORD dwWriteBytes = 0;
// 						::WriteFile(hFile, pBlock->pdata, pBlock->blksize, &dwWriteBytes, NULL);
// 						CloseHandle(hFile);
// 					}
// 					wchar_t msg[1024] = {0};
// 					swprintf(msg, L"name:%s:size:%I64d:offset:%I64d", pFile->name.c_str(), pFile->size, pBlock->offset);
// 					::MessageBoxW(NULL, msg, L"Tip", MB_OK);					
// 				}
//////////////////////////////////////////////////////////////////////////
				pThis->SetErrorInfo(UPT_ERR_CRCERR, pFile->name.c_str());
				pThis->m_Server.ReportGameIdxError(pThis->m_dwGid);
				Delete_Array(pBlock->pdata);
				goto exit;
			}
			else
			{
				if (!pThis->m_pipe2->Push(pBlock))
					Delete_Array(pBlock->pdata);
			}
		}
	}
exit:
	return 0;
}

DWORD CGameUpdate::OpenFile(tagIdxFile* pFile, utility::CAutoFile& clFile, BOOL& bUseNoBuf)
{
	wchar_t FileName[MAX_PATH] = {0};
	bUseNoBuf = pFile->size >= 0x10000;
	DWORD flg = (bUseNoBuf ? FILE_FLAG_NO_BUFFERING : 0) | FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN;
	swprintf_s(FileName, L"%s%s", m_strDevPath.c_str(), pFile->name.c_str());
	SetFileAttributesW(FileName, FILE_ATTRIBUTE_NORMAL);
	clFile = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, 
		NULL, OPEN_ALWAYS, flg, NULL);

	if (clFile.Get() == INVALID_HANDLE_VALUE)
		return GetLastError();

	LARGE_INTEGER liSize;
	liSize.QuadPart = bUseNoBuf ? ALIGN_SIZE(pFile->size, 512) : pFile->size;
	if (!SetFilePointerEx(clFile, liSize, NULL, FILE_BEGIN) || 
		!SetEndOfFile(clFile) ||
		!SetFileAttributesW(FileName, FILE_ATTRIBUTE_NORMAL))
	{
		return GetLastError();
	}
	return ERROR_SUCCESS;
}

DWORD CGameUpdate::WriteFile(HANDLE hFile, tagIdxBlock* pBlock, BOOL bUseNoBuf, LPOVERLAPPED pov)
{
	pov->Offset		= (DWORD)(pBlock->offset);
	pov->OffsetHigh = (DWORD)(pBlock->offset >> 32);
	ResetEvent(pov->hEvent);
	DWORD dwWriteBytes = 0;
	BOOL ret = ::WriteFile(hFile, pBlock->pdata, bUseNoBuf ? 
		ALIGN_SIZE(pBlock->blksize, 512) : pBlock->blksize, &dwWriteBytes, pov);
	if ((!ret && ERROR_IO_PENDING != GetLastError()) || 
		!GetOverlappedResult(hFile, pov, &dwWriteBytes, TRUE))
	{
		return GetLastError();
	}
	return ERROR_SUCCESS;
}

DWORD  CGameUpdate::CloseFile(utility::CAutoFile& clFile, tagIdxFile* pFile, BOOL bUseNoBuf)
{
	DWORD dwError = ERROR_SUCCESS;
	wchar_t FileName[MAX_PATH] = {0};
	swprintf_s(FileName, L"%s%s", m_strDevPath.c_str(), pFile->name.c_str());
	if (bUseNoBuf)
	{
		if (clFile.Get() != INVALID_HANDLE_VALUE)
			clFile.CleanUp();
		clFile = CreateFileW(FileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (clFile.Get() == INVALID_HANDLE_VALUE)
			dwError = GetLastError();
		else
		{
			LARGE_INTEGER liSize;
			liSize.QuadPart = pFile->size;
			if (!SetFilePointerEx(clFile, liSize, NULL, FILE_BEGIN) ||
				!SetEndOfFile(clFile))
			{
				dwError = GetLastError();
			}
		}
	}
	if (clFile.Get() != INVALID_HANDLE_VALUE)
	{
		SetFileAttributesW(FileName, pFile->attr);
		FILETIME ft = {0};
		ft.dwLowDateTime  = (DWORD)(pFile->time);
		ft.dwHighDateTime = (DWORD)(pFile->time >> 32);
		SetFileTime(clFile, NULL, NULL, &ft);
		clFile.CleanUp();
		clFile = INVALID_HANDLE_VALUE;
	}
	return dwError;
}

UINT __stdcall CGameUpdate::WriteThread(LPVOID lpVoid)
{
	CGameUpdate* pThis = reinterpret_cast<CGameUpdate*>(lpVoid);
    //
	async::thread::AutoEvent clEvent;
    clEvent.Create(NULL, TRUE, FALSE, NULL);
	OVERLAPPED ov = {0, 0, 0, 0, clEvent};

    utility::CAutoFile clFile;
	tagIdxFile* pFile = NULL;
	tagIdxBlock* pBlock = NULL;
	DWORD ret = ERROR_SUCCESS;
	
	for (std::vector<tagIdxFile*>::iterator it = pThis->m_vFiles.begin(); it != pThis->m_vFiles.end(); it++)
	{
		pFile = *it;
		if (pFile->attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			wchar_t FileName[MAX_PATH] = {0};
			swprintf_s(FileName, L"%s%s", pThis->m_strDevPath.c_str(), pFile->name.c_str());
			CreateDirectoryW(FileName, NULL);
			continue;
		}
		if (pFile->update == 0) continue ;
		
		BOOL bUseNoBuf = FALSE;
		if ((ret = pThis->OpenFile(pFile, clFile, bUseNoBuf)) != ERROR_SUCCESS || clFile.Get() == INVALID_HANDLE_VALUE)
        {
            pThis->SetErrorInfo(UPT_ERR_FILEOPEN, pFile->name.c_str(), ret);
            return 0;
        }

		pThis->SetUpdateInfo(L"%s", pFile->name.c_str());

		for (DWORD idx=0; idx<pFile->blknum; idx++)
		{
			if (!pThis->CheckState())
            {
                return 0;
            }

			if (pThis->m_vBlocks[pFile->crcoff + idx]->update != 1)
				continue;

			if (!pThis->m_pipe2->Pop(pBlock))
            {
                pThis->SetErrorInfo(UPT_ERR_FILEPOPDATA, pFile->name.c_str(), 0);
                return 0;
            }

			//write file block.
			if ((ret = pThis->WriteFile(clFile, pBlock, bUseNoBuf, &ov)) != ERROR_SUCCESS)
			{
                Delete_Array(pBlock->pdata);
                pThis->SetErrorInfo(UPT_ERR_FILEWRITE, pFile->name.c_str(), ret);
                return 0;
			}
			Delete_Array(pBlock->pdata);
		}
		if ((ret = pThis->CloseFile(clFile, pFile, bUseNoBuf)) != ERROR_SUCCESS)
        {
            pThis->SetErrorInfo(UPT_ERR_FILECLOSE, pFile->name.c_str(), ret);
            return 0;
        }
	}

	return 0;
}

void CGameUpdate::LimitSpeed(DWORD nStartTime, DWORD& nLastTime, DWORD& nTotalBytes, DWORD nRecvBytes)
{
	AutoLock lock(m_lockStatus);
	m_UpdateStuats.qUpdatedBytes += nRecvBytes;
	nTotalBytes += nRecvBytes;
	DWORD dwNowTime = GetTickCount();
	if (m_dwMaxSpeed != 0)
    {
        DWORD dwScheduleUseTime = DWORD(m_UpdateStuats.qUpdatedBytes * 1000 / m_dwMaxSpeed);
        DWORD dwUseTime = dwNowTime - nStartTime;
        if (dwScheduleUseTime > dwUseTime && dwScheduleUseTime - dwUseTime > 1000) //如果实际用时比计划用时小了一秒以上就开始限速
        {
            WaitForSingleObject(m_hExited, dwScheduleUseTime - dwUseTime);
            dwNowTime = GetTickCount();
        }
    }
	if (dwNowTime - nLastTime >= 1000)
	{
		DWORD dwSpeed1 = (DWORD)((float)nTotalBytes / (dwNowTime - nLastTime));
		DWORD dwSpeed2 = (DWORD)((float)m_UpdateStuats.qUpdatedBytes / (dwNowTime - nStartTime));
		m_UpdateStuats.dwSpeed = max(dwSpeed1, dwSpeed2);
		m_UpdateStuats.dwLeftTime = (DWORD)(((float)m_UpdateStuats.qNeedsUpdateBytes - 
			m_UpdateStuats.qUpdatedBytes)/(m_UpdateStuats.dwSpeed * 1000)) + 1;

		if( m_dwUptType != Sync )
		{
			m_Server.ReportGameStatus(m_dwGid, m_UpdateStuats.dwSpeed, 
				DWORD(1000.0f * m_UpdateStuats.qUpdatedBytes / m_UpdateStuats.qNeedsUpdateBytes),
				m_UpdateStuats.qNeedsUpdateBytes,
				m_UpdateStuats.qNeedsUpdateBytes - m_UpdateStuats.qUpdatedBytes, m_szTid, m_dwUptType, _T(""), _T(""));
		}

		nLastTime	= GetTickCount();
		nTotalBytes = 0;
	}
}

template<int iCount>
void ReadStringW2A(CPkgHelper& pack, char(&acData)[iCount])
{
    WCHAR awData[iCount];
    pack.PopString(awData);
    strcpy_s(acData, CW2A(awData));
}

void CGameUpdate::GetAllGameList(std::vector<tagGameInfo*>& gameList)
{
	gameList.clear();
	
	wchar_t Path[MAX_PATH] = {0};
	GetModuleFileNameW(NULL, Path, MAX_PATH);
	PathRemoveFileSpecW(Path);
	PathAddBackslashW(Path);
	lstrcatW(Path, L"Data\\GamesInfo.dat");
	HANDLE hFile = CreateFileW(Path, GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return ;

	DWORD dwReadByte = 0;
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	char* pData = new char[dwFileSize +1];
	if(!ReadFile(hFile, pData, dwFileSize, &dwReadByte, NULL))
	{
		CloseHandle(hFile);
		delete[]pData;
		return;
	}
	pData[dwReadByte] = 0;
	CloseHandle(hFile);

	CPkgHelper inpackage(pData);
	try
	{
		DWORD nStatus = 0;
		if (inpackage.Pop(nStatus))
		{
			DWORD dwCount = 0;
			inpackage.Pop(dwCount);
			for(DWORD idx = 0;idx <dwCount;idx++)
			{
				tagGameInfo* pGame = new tagGameInfo;
				inpackage.Pop(pGame->Gid);
				inpackage.Pop(pGame->Pid);
                ReadStringW2A(inpackage, pGame->Name);
				ReadStringW2A(inpackage, pGame->IdcClass);
				ReadStringW2A(inpackage, pGame->GameExe);
				ReadStringW2A(inpackage, pGame->Param);
				inpackage.Pop(pGame->Size);
				inpackage.Pop(pGame->DeskLnk);
				inpackage.Pop(pGame->Toolbar);
				ReadStringW2A(inpackage, pGame->SvrPath);
				PathAddBackslashA(pGame->SvrPath);
				ReadStringW2A(inpackage, pGame->CliPath);
				PathAddBackslashA(pGame->CliPath);
				inpackage.Pop(pGame->dwDeleteFlg);
				ReadStringW2A(inpackage, pGame->MatchFile);
				ReadStringW2A(inpackage, pGame->SaveFilter);
				inpackage.Pop(pGame->IdcUptDate);
				inpackage.Pop(pGame->IdcVer);
				inpackage.Pop(pGame->SvrVer);
				inpackage.Pop(pGame->AutoUpt);
				inpackage.Pop(pGame->I8Play);
				inpackage.Pop(pGame->IdcClick);
				inpackage.Pop(pGame->SvrClick);
				inpackage.Pop(pGame->RunType);
				ReadStringW2A(inpackage, pGame->Vid);
				inpackage.Pop(pGame->Status);
                ReadStringW2A(inpackage, pGame->Comment);
                inpackage.Pop(pGame->dwRepair);
				if (pGame->dwDeleteFlg != 0 || TRUE) //暂时不检查可删除设置
					gameList.push_back(pGame);
				else
					delete pGame;
			}
		}
	}
	catch (...)  { }
	delete []pData;

	struct Compare
	{	
		bool operator()(tagGameInfo* pGame1, tagGameInfo* pGame2)
		{
			if(pGame1->SvrClick < pGame2->SvrClick)
				return true;
			else
				return pGame1->IdcClick < pGame2->IdcClick;
		}
	};
	Compare f;
	std::stable_sort(gameList.begin(), gameList.end(), f);
}

void CGameUpdate::DeleteGame(LPCWSTR dir)
{
	wchar_t filename[MAX_PATH] = {0};
	wchar_t find[MAX_PATH] = {0};
	swprintf_s(find, L"%s*.*", dir);

	WIN32_FIND_DATAW wfd = {0};
	HANDLE hFinder = FindFirstFileW(find, &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;
	do 
	{
		if (!CheckState())
			break;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//filter dot directory.
			if (!((wfd.cFileName[0] == L'.') && 
				((wfd.cFileName[1] == 0) || (wfd.cFileName[1] == L'.' && wfd.cFileName[2] == 0))))
			{
				swprintf_s(filename, L"%s%s\\", dir, wfd.cFileName);
				DeleteGame(filename);
				RemoveDirectoryW(filename);
			}
		}
		else
		{
			swprintf_s(filename, L"%s%s", dir, wfd.cFileName);
			SetFileAttributesW(filename, FILE_ATTRIBUTE_NORMAL);
			DeleteFileW(filename);
		}
	}while (FindNextFileW(hFinder, &wfd));
	FindClose(hFinder);
}

void CGameUpdate::SetConnectCallBack(OnConnect_pfn pfnCallBack, void* pCallBackParam)
{
    m_pfnConnectCallBack = pfnCallBack;
    m_pConnectCallBackParam = pCallBackParam;
}

ISvrPlug* __stdcall CreateSvrPlug(DWORD Reserved)
{
	ISvrPlug* pPlug = NULL;
	LPCTSTR pName = (LPCTSTR)Reserved;
	if (lstrcmpi(pName, PLUG_UPTSVR_NAME) == 0)
		return new CUpdateSvrPlug;
	else if (lstrcmpi(pName, PLUG_SYNCDISK_NAME) == 0)
		return new CSyncPlug;
	return NULL;
}

ICliPlug* __stdcall CreateClientPlug(DWORD Reserved)
{
	ICliPlug* pPlug = NULL;
	LPCTSTR pName = (LPCTSTR)Reserved;
	if (lstrcmpi(pName, PLUG_FORCEUPT_NAME) == 0)
		return new CForceUpdatePlug;
	return NULL;
}

HANDLE __stdcall UG_StartUpdate(DWORD gid, LPCWSTR tid, LPCWSTR src, LPCWSTR dst, DWORD flag, LPCWSTR sip, LPCWSTR cip, DWORD speed, DWORD type)
{
	CGameUpdate* pUpdate = NULL;
	try
	{
		pUpdate = new CGameUpdate();
		_bstr_t svrip = (sip == NULL) ? _bstr_t("") : _bstr_t(sip);
		_bstr_t cliip = (cip == NULL) ? _bstr_t("") : _bstr_t(cip);
		pUpdate->StartUpdate(gid, tid, src, dst, flag, svrip, cliip, speed, type);
	}
	catch (...) { pUpdate = NULL; }
	return reinterpret_cast<HANDLE>(pUpdate);
}

void  __stdcall UG_StopUpdate(HANDLE handle)
{
	try
	{
		CGameUpdate* pUpdate = reinterpret_cast<CGameUpdate*>(handle);
		if (pUpdate != NULL)
		{
			pUpdate->StopUpdate();
			delete pUpdate;
		}
	}
	catch (...) { }
}


void  __stdcall UG_PauseUpdate(HANDLE handle)
{
	try
	{
		CGameUpdate* pUpdate = reinterpret_cast<CGameUpdate*>(handle);
		if (pUpdate != NULL)
		{
			pUpdate->PauseUpdate();
		}
	}
	catch (...) { }
}

void  __stdcall UG_ResumeUpdate(HANDLE handle)
{
	try
	{
		CGameUpdate* pUpdate = reinterpret_cast<CGameUpdate*>(handle);
		if (pUpdate != NULL)
		{
			pUpdate->ResumeUpdate();
		}
	}
	catch (...) { }
}



bool  __stdcall UG_GetUpdateStatus(HANDLE handle, tagUpdateStatus* pStatus)
{
	if (pStatus == NULL || pStatus->cbSize != sizeof(tagUpdateStatus))
		return false;
	try
	{
		CGameUpdate* pUpdate = reinterpret_cast<CGameUpdate*>(handle);
		if (pUpdate != NULL)
			pUpdate->GetUpdateStatus(*pStatus);
	}
	catch (...) { return false; }
	return true;
}

bool __stdcall UG_SetConnectCallBack(HANDLE handle, OnConnect_pfn pfnCallBack, void* pCallBackParam)
{
    try
    {
        CGameUpdate* pUpdate = reinterpret_cast<CGameUpdate*>(handle);
        if (pUpdate != NULL)
            pUpdate->SetConnectCallBack(pfnCallBack, pCallBackParam);
    }
    catch (...) { return false; }
    return true;
}

BOOL CGameUpdate::CheckState()
{
    HANDLE ahEvent[] = {m_hExited, m_eventRun};
    DWORD dwResult = WaitForMultipleObjects(_countof(ahEvent), ahEvent, FALSE, INFINITE);
    return dwResult != WAIT_OBJECT_0;
}
