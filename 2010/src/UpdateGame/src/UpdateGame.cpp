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

#pragma comment(lib, "Shlwapi.lib")

#ifndef ENGLISH_VERSION
	#define ERR_STR_DIRNOTOK		L"目录名不正确:%s-%s."
	#define ERR_STR_CREATEDIR		L"创建目录失败:%s."
	#define ERR_STR_INNERERR		L"内部错误:%d."
	#define ERR_STR_LOADDEVICE		L"加载虚拟设备出错."
	#define ERR_STR_GETGAMEINFO		L"取游戏信息失败:size:0x%x, ver:0x%x."
	#define ERR_STR_DWNIDX			L"下载索引失败."
	#define ERR_STR_DISKNOROOM		L"磁盘空间不足,可用%dM,需要%dM."
	#define ERR_STR_PARSEIDX		L"解析索引失败:%d."
	#define ERR_STR_CONNFAIL		L"连接服务器失败,请手动启动游戏."
	#define ERR_STR_DWNFAIL			L"下载数据块失败:%s."
	#define ERR_STR_CRCERR			L"数据块crc不同:%s,己提交三层自动修复,请稍后重试."
	#define ERR_STR_FILEOPENWRITE	L"文件操作失败:%s:%d."
	
	#define INFO_STR_GETGAMEINFO	L"取游戏相关信息."
	#define	INFO_STR_DOWNIDXFILE	L"下载索引文件."
	#define	INFO_STR_PRASEIDXFILE	L"解析索引文件."
	#define	INFO_STR_DELETEGAME		L"空间不足,清除游戏:%s."
#else
	#define ERR_STR_DIRNOTOK		L"invalid directory:%s-%s."
	#define ERR_STR_CREATEDIR		L"create directory fail:%s."
	#define ERR_STR_INNERERR		L"inner error:%d."
	#define ERR_STR_LOADDEVICE		L"load virtual device fail."
	#define ERR_STR_GETGAMEINFO		L"query game info fail:size:0x%x, ver:0x%x."
	#define ERR_STR_DWNIDX			L"download index fail."
	#define ERR_STR_DISKNOROOM		L"no disk room,free size:%dM, need size:%dM."
	#define ERR_STR_PARSEIDX		L"parse index fail:%d."
	#define ERR_STR_CONNFAIL		L"connect server fail, please run game."
	#define ERR_STR_DWNFAIL			L"download block fail:%s."
	#define ERR_STR_CRCERR			L"block crc error,%s, pleasess retry."
	#define ERR_STR_FILEOPENWRITE	L"file operator fail:%s:%d"
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
		DWORD	Priority;					//游戏等级，用于删除点击率低的游戏，三层会需要
		DWORD	ShowPriority;				//游戏在菜单上的显示优先级
		char	MatchFile[MAX_PATH];		//游戏自动搜索特征文件
		char	SaveFilter[MAX_PATH];		//游戏存档信息
		DWORD	IdcUptDate;					//游戏在中心服务器的更新时间
		DWORD	SvrUptDate;					//游戏在网吧服务器的更新时间
		DWORD	IdcVer;						//游戏在中心服务器上的版本
		DWORD	SvrVer;						//游戏在网吧服务器上的版本
		DWORD	AutoUpt;					//游戏是否自动从服务器上更新
		DWORD	I8Play;						//游戏需要强制更新
		DWORD	IdcClick;					//游戏全网点率
		DWORD	SvrClick;					//游戏网吧点率
		DWORD	AddDate;					//游戏添加时间
		char	Url[MAX_PATH];				//游戏官网
		DWORD	RunType;					//游戏运行方式
		DWORD	IfUpdate;					//游戏是否更新
		DWORD	IfDisplay;					//游戏是否显示
		DWORD	Status;						//是否是本地游戏
		char	Vid[MAX_PATH];				//游戏所在虚拟盘ID
		char    Comment[MAX_PATH];			//备注
	}tagGameInfo;
public:
	CGameUpdate();
	~CGameUpdate();
	bool StartUpdate(DWORD gid, LPCWSTR src, LPCWSTR dst, DWORD flag, LPCSTR svrip, LPCSTR cliip, DWORD maxspeed);
	void GetUpdateStatus(tagUpdateStatus& UpdateStuas);
	void StopUpdate();
private:
	void  _cdecl SetErrorInfo(LPCWSTR formater, ...);
	inline void SetUpdateStatus(DWORD status)
	{
		CAutoLock<CLock> lock(&m_lockStatus);
		m_UpdateStuats.Status = status;
	}
	inline void SetUpdateInfo(LPCWSTR formater, ...)
	{
		CAutoLock<CLock> lock(&m_lockStatus);
		va_list marker;
		va_start(marker, formater);
		_vswprintf(m_UpdateStuats.Info, formater, marker);
		va_end(marker);
	}
	static bool __stdcall CallBack(void* pcookie, LPCWSTR lpInfo)
	{
		CGameUpdate* pThis = reinterpret_cast<CGameUpdate*>(pcookie);
		if (WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
			return false;

		CAutoLock<CLock> lock(&pThis->m_lockStatus);
		wcscpy(pThis->m_UpdateStuats.Info, lpInfo);
		return true;
	}
	inline bool  CompareIdxfile(__int64& qUpdateBytes, std::map<std::wstring, tagIdxFile*>& mFiles);
	inline void  DeleteIdxfile();
	inline bool  UpdateBefore(__int64 qUpdateBytes);
	inline void  UpdateAfter();
	inline DWORD OpenFile(tagIdxFile* pFile, HANDLE& hFile, BOOL& bUseNoBuf);
	inline DWORD WriteFile(HANDLE hFile, tagIdxBlock* pBlock, BOOL bUseNoBuf, LPOVERLAPPED pov);
	inline DWORD CloseFile(HANDLE& hFile, tagIdxFile* pFile, BOOL bUseNoBuf);
	static UINT __stdcall WorkThread(LPVOID lpVoid);
	static UINT __stdcall RecvThread(LPVOID lpVoid);
	static UINT __stdcall CalCrcThread(LPVOID lpVoid);
	static UINT __stdcall WriteThread(LPVOID lpVoid);
	void LimitSpeed(DWORD nStartTime, DWORD& nLastTime, DWORD& nTotalBytes, DWORD nRecvBytes);
	void GetAllGameList(std::vector<tagGameInfo*>& gameList);
	void DeleteGame(LPCWSTR dir);
private:
	DWORD m_dwGid;
	DWORD m_dwMaxSpeed;
	DWORD m_dwUptFlag;
	std::string  m_strSvrIp;
	std::string  m_strCliIp;
	std::wstring m_strSvrPath;
	std::wstring m_strCliPath;
	std::wstring m_strDevPath;
	CLock m_lockStatus;
	tagUpdateStatus m_UpdateStuats;
	HANDLE		 m_hStopPushEvent;	//停止主动推送。

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
};

CGameUpdate::CGameUpdate() : m_dwGid(-1), m_dwMaxSpeed(0), m_dwUptFlag(0), m_pIdxfile(NULL), 
	m_nIdxfileSize(0), m_dwSvrIdxVer(-1), m_hExited(NULL), m_hThread(NULL), 
	m_pipe(NULL), m_pipe2(NULL)
{
	ZeroMemory(&m_UpdateStuats, sizeof(m_UpdateStuats));
#ifdef UPDATEGAME_EXPORTS
	CEvenyOneSD sd;
	m_hStopPushEvent = CreateEvent(sd.GetSA(), TRUE, FALSE, UPDATE_EVENT_NAME);
#else
	m_hStopPushEvent = NULL;
#endif
}

CGameUpdate::~CGameUpdate()
{
	if (m_hStopPushEvent != NULL)
	{
		CloseHandle(m_hStopPushEvent);
		m_hStopPushEvent = NULL;
	}
}

bool CGameUpdate::StartUpdate(DWORD gid, LPCWSTR src, LPCWSTR dst, DWORD flag, LPCSTR svrip, LPCSTR cliip, DWORD maxspeed)
{
	StopUpdate();
	ZeroMemory(&m_UpdateStuats, sizeof(m_UpdateStuats));

	m_dwGid = gid;
	if (maxspeed)
		m_dwMaxSpeed = (maxspeed < 1000) ? 640 : ((maxspeed - 200) * 1000);

	//check parameter
	if (src == NULL || dst == NULL || wcslen(src) < 2 || wcslen(dst) < 2 ||
		src[1] != L':' || src[2] != L'\\' || 
		dst[1] != L':' || dst[2] != L'\\')
	{
		SetErrorInfo(ERR_STR_DIRNOTOK, src, dst);
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
		SetErrorInfo(ERR_STR_LOADDEVICE);
		return false;
	}
	m_strDevPath = hy_ConvertPath(m_strCliPath);
	if (!hy_CreateDirectory(m_strCliPath) || !hy_CreateDirectory(m_strDevPath))
	{
		SetErrorInfo(ERR_STR_CREATEDIR, m_strCliPath.c_str());
		return false;
	}

	//create update work thread.
	if (m_hStopPushEvent != NULL)
		SetEvent(m_hStopPushEvent);
	m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExited == NULL)
	{
		SetErrorInfo(ERR_STR_INNERERR, GetLastError());
		return false;
	}
	m_pipe  = new PCMomule<tagIdxBlock*>(m_hExited);
	m_pipe2 = new PCMomule<tagIdxBlock*>(m_hExited);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
	if (m_hThread == NULL)
	{
		SetErrorInfo(ERR_STR_INNERERR, GetLastError());
		StopUpdate();
		return false;
	}

	return true;
}

void CGameUpdate::GetUpdateStatus(tagUpdateStatus& UpdateStuas)
{
	CAutoLock<CLock> lock(&m_lockStatus);
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
	if (m_hStopPushEvent != NULL)
		ResetEvent(m_hStopPushEvent);
}

void _cdecl CGameUpdate::SetErrorInfo(LPCWSTR formater, ...)
{
	CAutoLock<CLock> lock(&m_lockStatus);
	m_UpdateStuats.Status = UPDATE_STATUS_ERROR;
	m_UpdateStuats.m_dwLeftTime = 0;

	va_list marker;
	va_start(marker, formater);
	_vswprintf(m_UpdateStuats.Info, formater, marker);
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
	if (wcsicmp(m_strDevPath.c_str(), m_strCliPath.c_str()) != 0)
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
			if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
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
					swprintf(szLog, L"i8desk: free size:%dM, need size:%dM.", (DWORD)(liSize.QuadPart / 1000 / 1000),
						(DWORD)(qUpdateBytes / 1000 / 1000));
					OutputDebugStringW(szLog);
				}
			}
		}
		std::for_each(list.begin(), list.end(), Delete_Pointer<tagGameInfo>);
	}
	//空间不足，返回错误。
	SetErrorInfo(ERR_STR_DISKNOROOM, (DWORD)(liSize.QuadPart / 1000 / 1000), (DWORD)(qUpdateBytes / 1000 / 1000));
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
		if (wcsicmp(m_strDevPath.c_str(), m_strCliPath.c_str()) != 0)
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

bool CGameUpdate::CompareIdxfile(__int64& qUpdateBytes, std::map<std::wstring, tagIdxFile*>& mFiles)
{
	DWORD ret = ERROR_SUCCESS;
	qUpdateBytes = 0;

	//recv game info.(version,idxfile size)
	SetUpdateInfo(INFO_STR_GETGAMEINFO);
	if (!m_Server.GetGameInfo(m_dwGid, m_strSvrPath.c_str(), m_dwSvrIdxVer, m_nIdxfileSize, m_ftIdxfile))
	{
		SetErrorInfo(ERR_STR_GETGAMEINFO, m_nIdxfileSize, m_dwSvrIdxVer);
		return false;
	}

	//compare game block.
	if (m_dwUptFlag & UPDATE_FLAG_FORCE_UPDATE)
	{
		//recv server idxfile.
		SetUpdateInfo(INFO_STR_DOWNIDXFILE);
		if (!m_Server.RecvIdxfile(m_strSvrPath.c_str(), m_pIdxfile, m_nIdxfileSize))
		{
			SetErrorInfo(ERR_STR_DWNIDX);
			return false;
		}
		//parse server idxfile.
		SetUpdateInfo(INFO_STR_PRASEIDXFILE);
		ret = Idxfile_Parse(m_pIdxfile, m_nIdxfileSize, m_vFiles, m_vBlocks, mFiles);
		if (ret != ERROR_SUCCESS)
		{
			SetErrorInfo(ERR_STR_PARSEIDX, ret);
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
			if (m_dwUptFlag | UPDATE_FLAG_DELETE_MOREFILE)
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
				SetErrorInfo(ERR_STR_DWNIDX);
				return false;
			}

			//parse server idxfile.
			SetUpdateInfo(INFO_STR_PRASEIDXFILE);
			ret = Idxfile_Parse(m_pIdxfile, m_nIdxfileSize, m_vFiles, m_vBlocks, mFiles);
			if (ret != ERROR_SUCCESS)
			{
				SetErrorInfo(ERR_STR_PARSEIDX, ret);
				return false;
			}

			//对比两个索引,如果本地索引不存在，或者文件不正确，则扫描磁盘文件。
			std::vector<tagIdxFile*>  m_LocalVFiles;
			std::vector<tagIdxBlock*> m_LocalBlocks;
			std::map<std::wstring, tagIdxFile*> m_LocalMFiles;
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
	std::map<std::wstring, tagIdxFile*> mFiles;
	__int64 qUpdateBytes = 0;		//need updated bytes.

	while (1)
	{
		//connect server.
		if (pThis->m_Server.ConnectServer(pThis->m_strSvrIp.c_str(), pThis->m_strCliIp.c_str()))
		{
			pThis->SetErrorInfo(ERR_STR_CONNFAIL);
			return false;
		}
		
		//step 1:compare idxfile.
		{
			pThis->SetUpdateStatus(UPDATE_STAUTS_COMPARE);
			if (!pThis->CompareIdxfile(qUpdateBytes, mFiles))
				break;
			CAutoLock<CLock> lock(&pThis->m_lockStatus);
			pThis->m_UpdateStuats.m_qNeedsUpdateBytes = qUpdateBytes;
			pThis->m_UpdateStuats.m_qUpdatedBytes	  = 0;
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
				pThis->SetErrorInfo(ERR_STR_INNERERR, GetLastError());
				break;
			}
			WaitForMultipleObjects(_countof(hThread), hThread, TRUE, INFINITE);
			if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
				break;
		}

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
			if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
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
				pThis->SetErrorInfo(ERR_STR_INNERERR, GetLastError());
				goto exit;
			}

			if (!pThis->m_Server.RecvBlock(pFile->name.c_str(), pFile->size, pBlock->offset, 
				pBlock->blksize, bFirstBlock, pBlock->pdata, pThis->m_hExited))
			{
				if (WaitForSingleObject(pThis->m_hExited, 0) != WAIT_OBJECT_0)
					pThis->SetErrorInfo(ERR_STR_DWNFAIL, pFile->name.c_str());
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
			if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
				goto exit;

			if (pThis->m_vBlocks[pFile->crcoff + idx]->update != 1)
				continue;

			if (!pThis->m_pipe->Pop(pBlock))
				goto exit;

			//compare block crc.
			if (pBlock->crc != Idxfile_BlockCrc((unsigned char*)pBlock->pdata, pBlock->blksize))
			{
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

				pThis->SetErrorInfo(ERR_STR_CRCERR, pFile->name.c_str());
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

DWORD CGameUpdate::OpenFile(tagIdxFile* pFile, HANDLE& hFile, BOOL& bUseNoBuf)
{
	wchar_t FileName[MAX_PATH] = {0};
	bUseNoBuf = pFile->size >= 0x10000;
	DWORD flg = (bUseNoBuf ? FILE_FLAG_NO_BUFFERING : 0) | FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN;
	swprintf(FileName, L"%s%s", m_strDevPath.c_str(), pFile->name.c_str());
	SetFileAttributesW(FileName, FILE_ATTRIBUTE_NORMAL);
	hFile = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, 
		NULL, OPEN_ALWAYS, flg, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return GetLastError();

	LARGE_INTEGER liSize;
	liSize.QuadPart = bUseNoBuf ? ALIGN_SIZE(pFile->size, 512) : pFile->size;
	if (!SetFilePointerEx(hFile, liSize, NULL, FILE_BEGIN) || 
		!SetEndOfFile(hFile) ||
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

DWORD  CGameUpdate::CloseFile(HANDLE& hFile, tagIdxFile* pFile, BOOL bUseNoBuf)
{
	DWORD dwError = ERROR_SUCCESS;
	wchar_t FileName[MAX_PATH] = {0};
	swprintf(FileName, L"%s%s", m_strDevPath.c_str(), pFile->name.c_str());
	if (bUseNoBuf)
	{
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		hFile = CreateFileW(FileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			dwError = GetLastError();
		else
		{
			LARGE_INTEGER liSize;
			liSize.QuadPart = pFile->size;
			if (!SetFilePointerEx(hFile, liSize, NULL, FILE_BEGIN) ||
				!SetEndOfFile(hFile))
			{
				dwError = GetLastError();
			}
		}
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		SetFileAttributesW(FileName, pFile->attr);
		FILETIME ft = {0};
		ft.dwLowDateTime  = (DWORD)(pFile->time);
		ft.dwHighDateTime = (DWORD)(pFile->time >> 32);
		SetFileTime(hFile, NULL, NULL, &ft);
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return dwError;
}

UINT __stdcall CGameUpdate::WriteThread(LPVOID lpVoid)
{
	CGameUpdate* pThis = reinterpret_cast<CGameUpdate*>(lpVoid);
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	OVERLAPPED ov = {0, 0, 0, 0, hEvent};

	HANDLE hFile = INVALID_HANDLE_VALUE;
	tagIdxFile* pFile = NULL;
	tagIdxBlock* pBlock = NULL;
	DWORD ret = ERROR_SUCCESS;
	
	for (std::vector<tagIdxFile*>::iterator it = pThis->m_vFiles.begin(); it != pThis->m_vFiles.end(); it++)
	{
		pFile = *it;
		if (pFile->attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			wchar_t FileName[MAX_PATH] = {0};
			swprintf(FileName, L"%s%s", pThis->m_strDevPath.c_str(), pFile->name.c_str());
			CreateDirectoryW(FileName, NULL);
			continue;
		}
		if (pFile->update == 0) continue ;
		
		BOOL bUseNoBuf = FALSE;
		if ((ret = pThis->OpenFile(pFile, hFile, bUseNoBuf)) != ERROR_SUCCESS)
			goto exit;

		pThis->SetUpdateInfo(L"%s", pFile->name.c_str());

		for (DWORD idx=0; idx<pFile->blknum; idx++)
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
				goto exit;

			if (pThis->m_vBlocks[pFile->crcoff + idx]->update != 1)
				continue;

			if (!pThis->m_pipe2->Pop(pBlock))
				goto exit;

			//write file block.
			if ((ret = pThis->WriteFile(hFile, pBlock, bUseNoBuf, &ov)) != ERROR_SUCCESS)
			{
				Delete_Array(pBlock->pdata);
				goto exit;
			}
			Delete_Array(pBlock->pdata);
		}
		if ((ret = pThis->CloseFile(hFile, pFile, bUseNoBuf)) != ERROR_SUCCESS)
			goto exit;
	}
exit:
	if (pFile != NULL && ret != ERROR_SUCCESS)
		pThis->SetErrorInfo(ERR_STR_FILEOPENWRITE, pFile->name.c_str(), ret);

	//中断更新后，直接关闭文件，不修改文件的修改时间，与属性。
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	CloseHandle(hEvent);
	return 0;
}

void CGameUpdate::LimitSpeed(DWORD nStartTime, DWORD& nLastTime, DWORD& nTotalBytes, DWORD nRecvBytes)
{
	CAutoLock<CLock> lock(&m_lockStatus);
	m_UpdateStuats.m_qUpdatedBytes += nRecvBytes;
	nTotalBytes += nRecvBytes;
	DWORD dwNowTime = GetTickCount();
	if (dwNowTime - nLastTime < 1000 && m_dwMaxSpeed != 0 && nTotalBytes >= m_dwMaxSpeed)
	{
		WaitForSingleObject(m_hExited, 1000 - dwNowTime + nLastTime);
		dwNowTime = GetTickCount();
	}
	if (dwNowTime - nLastTime >= 1000)
	{
		DWORD dwSpeed1 = (DWORD)((float)nTotalBytes / (dwNowTime - nLastTime));
		DWORD dwSpeed2 = (DWORD)((float)m_UpdateStuats.m_qUpdatedBytes / (dwNowTime - nStartTime));
		m_UpdateStuats.m_dwSpeed = max(dwSpeed1, dwSpeed2);
		m_UpdateStuats.m_dwLeftTime = (DWORD)(((float)m_UpdateStuats.m_qNeedsUpdateBytes - 
			m_UpdateStuats.m_qUpdatedBytes)/(m_UpdateStuats.m_dwSpeed * 1000)) + 1;
		m_Server.ReportGameStatus(m_dwGid, m_UpdateStuats.m_dwSpeed, 
			DWORD(1000.0f * m_UpdateStuats.m_qUpdatedBytes / m_UpdateStuats.m_qNeedsUpdateBytes),
			m_UpdateStuats.m_qNeedsUpdateBytes,
			m_UpdateStuats.m_qNeedsUpdateBytes - m_UpdateStuats.m_qUpdatedBytes);

		nLastTime	= GetTickCount();
		nTotalBytes = 0;
	}
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

	CPackageHelper inpackage(pData);
	try
	{
		if (inpackage.popDWORD())
		{
			DWORD dwCount = inpackage.popDWORD();
			for(DWORD idx = 0;idx <dwCount;idx++)
			{
				tagGameInfo* pGame = new tagGameInfo;
				pGame->Gid			= inpackage.popDWORD();
				pGame->Pid			= inpackage.popDWORD();
				inpackage.popString(pGame->Name);
				inpackage.popString(pGame->IdcClass);
				inpackage.popString(pGame->GameExe);
				inpackage.popString(pGame->Param);
				pGame->Size			= inpackage.popDWORD();
				pGame->DeskLnk		= inpackage.popDWORD();
				pGame->Toolbar		= inpackage.popDWORD();
				inpackage.popString(pGame->SvrPath);
				PathAddBackslashA(pGame->SvrPath);
				inpackage.popString(pGame->CliPath);
				PathAddBackslashA(pGame->CliPath);
				pGame->Priority		= inpackage.popDWORD();
				pGame->ShowPriority = inpackage.popDWORD();
				inpackage.popString(pGame->MatchFile);
				inpackage.popString(pGame->SaveFilter);
				pGame->IdcUptDate	= inpackage.popDWORD();
				pGame->SvrUptDate	= inpackage.popDWORD();
				pGame->IdcVer		= inpackage.popDWORD();
				pGame->SvrVer		= inpackage.popDWORD();
				pGame->AutoUpt		= inpackage.popDWORD();
				pGame->I8Play		= inpackage.popDWORD();
				pGame->IdcClick		= inpackage.popDWORD();
				pGame->SvrClick		= inpackage.popDWORD();
				pGame->AddDate		= inpackage.popDWORD();
				inpackage.popString(pGame->Url);
				pGame->RunType		= inpackage.popDWORD();
				pGame->IfUpdate		= inpackage.popDWORD();
				pGame->IfDisplay	= inpackage.popDWORD();
				inpackage.popString(pGame->Vid);
				pGame->Status		= inpackage.popDWORD();
				inpackage.popString(pGame->Comment);
				if (pGame->Priority < 3)
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
			if(pGame1->Priority == pGame2->Priority)
				return  pGame1->IdcClick < pGame2->IdcClick;
			return pGame1->Priority < pGame2->Priority;
		}
	};
	Compare f;
	std::stable_sort(gameList.begin(), gameList.end(), f);
}

void CGameUpdate::DeleteGame(LPCWSTR dir)
{
	wchar_t filename[MAX_PATH] = {0};
	wchar_t find[MAX_PATH] = {0};
	swprintf(find, L"%s*.*", dir);

	WIN32_FIND_DATAW wfd = {0};
	HANDLE hFinder = FindFirstFileW(find, &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;
	do 
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
			break;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//filter dot directory.
			if (!((wfd.cFileName[0] == L'.') && 
				((wfd.cFileName[1] == 0) || (wfd.cFileName[1] == L'.' && wfd.cFileName[2] == 0))))
			{
				swprintf(filename, L"%s%s\\", dir, wfd.cFileName);
				DeleteGame(filename);
				RemoveDirectoryW(filename);
			}
		}
		else
		{
			swprintf(filename, L"%s%s", dir, wfd.cFileName);
			SetFileAttributesW(filename, FILE_ATTRIBUTE_NORMAL);
			DeleteFileW(filename);
		}
	}while (FindNextFileW(hFinder, &wfd));
	FindClose(hFinder);
}

IPlug*	__stdcall CreatePlug()
{
	IPlug* pPlug = NULL;

#if		UPDATEGAMESVR_EXPORTS
	pPlug = new CUpdateSvrPlug();								//输出更新服务端插件
#elif	SYNCDISK_EXPORTS
	pPlug = new CSyncPlug();									//输出同步插件
#elif	FORCEUPDATE_EXPORTS
	pPlug = reinterpret_cast<IPlug*>(new CForceUpdatePlug);		//输出主动推送插件
#endif
	return pPlug;
}

IPlug*	__stdcall CreatePlug2(bool bUpdate)
{
	IPlug* pPlug = NULL;
	try
	{
		if (bUpdate)
			pPlug = new CUpdateSvrPlug();
		else
			pPlug = new CSyncPlug();
	}
	catch (...) { }
	return pPlug;
}

HANDLE __stdcall UG_StartUpdate(DWORD gid, LPCWSTR src, LPCWSTR dst, DWORD flag, LPCWSTR sip, LPCWSTR cip, DWORD speed)
{
	CGameUpdate* pUpdate = NULL;
	try
	{
		pUpdate = new CGameUpdate();
		_bstr_t svrip = (sip == NULL) ? _bstr_t("") : _bstr_t(sip);
		_bstr_t cliip = (cip == NULL) ? _bstr_t("") : _bstr_t(cip);
		pUpdate->StartUpdate(gid, src, dst, flag, svrip, cliip, speed);
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

bool  __stdcall UG_GetUpdateStatus(HANDLE handle, tagUpdateStatus* pStatus)
{
	if (pStatus == NULL || pStatus->cbSize != sizeof(tagUpdateStatus))
		return false;
	try
	{
		ZeroMemory(pStatus, sizeof(tagUpdateStatus));
		CGameUpdate* pUpdate = reinterpret_cast<CGameUpdate*>(handle);
		if (pUpdate != NULL)
			pUpdate->GetUpdateStatus(*pStatus);
	}
	catch (...) { return false; }
	return true;
}

//////////////////////////////////////////////////////////////////////////
//兼容以前的10版本输出的函数
DWORD __stdcall FastCopy_Start(LPCSTR psrc, LPCSTR pdest, LPCSTR pMainSrvIP, LPCSTR pSecondSrvIp,
	DWORD GID, FC_COPY_MODE mode, bool FiOpenp2p, DWORD dwProtSize)
{
	DWORD flag = UPDATE_FLAG_DELETE_MOREFILE | UPDATE_FLAG_DELETE_GAME;
	if (mode == FC_COPY_FORE)
		flag |= UPDATE_FLAG_FORCE_UPDATE;
	if (mode == FC_COPY_QUICK)
		flag |= UPDATE_FLAG_QUICK_COMPARE;

	_bstr_t main   = pMainSrvIP   != NULL ? pMainSrvIP	  : _bstr_t("");
	_bstr_t second = pSecondSrvIp != NULL ? pSecondSrvIp : _bstr_t("");
	_bstr_t ip = second + _bstr_t("|") + main;
	HANDLE hUpdate = UG_StartUpdate(GID, _bstr_t(psrc), _bstr_t(pdest), flag, ip);
	return reinterpret_cast<DWORD>(hUpdate);
}

void __stdcall FastCopy_End(DWORD handle)
{
	UG_StopUpdate((HANDLE)handle);
}

void __stdcall FastCopy_Cancel(DWORD handle)
{
	UG_StopUpdate((HANDLE)handle);	
}

void __stdcall FastCopy_GetSpeedEx(DWORD handle, FastCopyStatus* pStatus)
{
	ZeroMemory(pStatus, sizeof(FastCopyStatus));
	tagUpdateStatus Status = {sizeof(tagUpdateStatus)};
	
	if (!UG_GetUpdateStatus((HANDLE)handle, &Status))
		return ;

	if (Status.m_qNeedsUpdateBytes)
		pStatus->TotalProgress = (DWORD)(Status.m_qUpdatedBytes * 1000 / Status.m_qNeedsUpdateBytes);
	pStatus->AverageSpeed	= Status.m_dwSpeed;
	pStatus->LeftSecond		= Status.m_dwLeftTime;
	pStatus->TotalFileSize	= Status.m_qNeedsUpdateBytes;
	pStatus->UpdateFizeSize = Status.m_qUpdatedBytes;
	lstrcpyA(pStatus->szCurFile, _bstr_t(Status.Info));
	
	switch (Status.Status)
	{
	case UPDATE_STATUS_FINISH:
		pStatus->Progress = FCS_FINISH;
		break;
	case UPDATE_STAUTS_COMPARE:
		pStatus->Progress = FCS_PROGRESS_SCAN;
		break;
	case UPDATE_STATUS_UPDATE:
		pStatus->Progress = FCS_PROGRESS_UPDATE;
		break;
	case UPDATE_STATUS_DELETE_MOREFILE:
		pStatus->Progress = FCS_PROGRESS_DELMOREFILE;
		break;
	case UPDATE_STATUS_DELETE_GAME:
		pStatus->Progress = FCS_DELGAME;
		break;
	case UPDATE_STATUS_ERROR:
		pStatus->Progress = FCS_ERROR;
		break;
	}
}

void __stdcall FastCopy_GetSpeed(DWORD handle, FastCopyStatus* pStatus)
{
	FastCopy_GetSpeedEx(handle, pStatus);
}

DWORD __stdcall FastCopy_GetVersion()
{
	return 0x00000001;
}
//////////////////////////////////////////////////////////////////////////