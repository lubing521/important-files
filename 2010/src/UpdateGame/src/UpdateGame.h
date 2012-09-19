#ifndef __update_game_inc__
#define __update_game_inc__

#include "public.h"

#define UPDATE_FLAG_FORCE_UPDATE		0x01		//强制更新
#define UPDATE_FLAG_QUICK_COMPARE		0x02		//快速更新	//与强制更新只能二选一。
#define UPDATE_FLAG_DELETE_MOREFILE		0x04		//执行删除多余文件
#define UPDATE_FLAG_DELETE_GAME			0x08		//空间不足时，执行删除点击率低的游戏。

#define UPDATE_STATUS_FINISH			0x01		//更新成功完成
#define UPDATE_STAUTS_COMPARE			0x02		//对比文件
#define UPDATE_STATUS_UPDATE			0x04		//更新文件块
#define UPDATE_STATUS_DELETE_MOREFILE	0x08		//删除多余文件
#define UPDATE_STATUS_DELETE_GAME		0x010		//删除游戏
#define UPDATE_STATUS_ERROR				0xffffffff	//更新出错

typedef struct tagUpdateStatus
{
	DWORD   cbSize;						//结构体大小，用于版本控制
	DWORD	Status;						//更新状态
	__int64	m_qNeedsUpdateBytes;		//需要更新的字节数
	__int64	m_qUpdatedBytes;			//己经更新的字节数
	DWORD	m_dwLeftTime;				//估计剩余时间
	DWORD	m_dwSpeed;					//更新速度k/s
	wchar_t Info[1024];					//相关信息，文件名或者错误信息
}tagUpdateStatus;

IPlug*	__stdcall CreatePlug();
IPlug*	__stdcall CreatePlug2(bool bUpdate);
HANDLE	__stdcall UG_StartUpdate(DWORD gid, LPCWSTR src, LPCWSTR dst, DWORD flag, 
	LPCWSTR sip, LPCWSTR cip = NULL, DWORD speed = 0);
void	__stdcall UG_StopUpdate(HANDLE handle);
bool	__stdcall UG_GetUpdateStatus(HANDLE handle, tagUpdateStatus* pStatus);

//////////////////////////////////////////////////////////////////////////
//兼容以前的10版本输出的函数
struct FastCopyStatus
{
	DWORD   Progress;			//update status.
	DWORD   StartType;			//reserved.
	DWORD	TotalUpdateFile;	//reserved.
	DWORD	UpdateFile;			//reserved.
	DWORD   CurProgress;		//reserved.
	DWORD   TotalProgress;		//progress of updating.(500表示50%.)
	DWORD   AverageSpeed;		//average speed. k/s
	DWORD   UsedSecond;			//reserved.
	DWORD   LeftSecond;			//need seconds.
	__int64 TotalFileSize;		//need update total bytes.
	__int64 UpdateFizeSize;		//updated bytes.
	char 	szCurFile[260];		//info tip.
};

enum FC_STATUS
{
	FCS_FINISH					=	0x00000001,
	FCS_ERROR					=	0x00010000,
	FCS_PROGRESS				=	0x00020000,
	FCS_PROGRESS_SCAN			=	0x00020001,
	FCS_PROGRESS_UPDATE			=	0x00020002,
	FCS_PROGRESS_DELMOREFILE	=	0x00020003,
	FCS_NOROOM					=	0x00030000,
	FCS_DELGAME					=   0x00030001,
	FCS_NOIDXFILE				=	0x00030002
};

DWORD __stdcall FastCopy_Start(LPCSTR psrc, LPCSTR pdest, LPCSTR pMainSrvIP, LPCSTR pSecondSrvIp,
	DWORD GID, FC_COPY_MODE mode, bool FiOpenp2p, DWORD dwProtSize);
void __stdcall FastCopy_End(DWORD handle);
void __stdcall FastCopy_Cancel(DWORD handle);
void __stdcall FastCopy_GetSpeedEx(DWORD handle, FastCopyStatus* pStatus);
void __stdcall FastCopy_GetSpeed(DWORD handle, FastCopyStatus* pStatus);
//////////////////////////////////////////////////////////////////////////

#endif