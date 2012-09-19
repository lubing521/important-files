#pragma once

enum FC_STATUS
{
		FCS_FINISH				=	0x00000001,
		FCS_ERROR				=	0x00010000,
		FCS_PROGRESS				=	0x00020000,
		FCS_PROGRESS_SCAN			=	0x00020001,
		FCS_PROGRESS_UPDATE			=	0x00020002,
		FCS_PROGRESS_DELMOREFILE	=	0x00020003,
		FCS_NOROOM				=	0x00030000,
		FCS_DELGAME				=   0x00030001
};

enum FC_COPY_MODE
{
	FC_COPY_COMPARE		= 0,
	FC_COPY_QUICK		= 1,
	FC_COPY_FORE		= 2,
	FC_COPY_VERSION		= 3
};
enum FC_FLAG
{
	FC_FLAG_DELETEMOREFILE = 0x01
};
enum FC_STARTTYPE
{
		FC_START_VDISK		= 0x00,			//虚拟磁盘.
		FC_START_LDISK		= 0x01			//本地磁盘
};

struct FastCopyStatus
{
	DWORD   Progress;					//更新的阶段。
	DWORD   StartType;					//当成功更新后，菜单是从虚拟盘启动，还是从本地启动。
	
	DWORD	TotalUpdateFile;			//总共需要更新文件数。
	DWORD	UpdateFile;					//己经更新的文件数。
	
	DWORD   CurProgress;				//当前文件的进度.
	DWORD   TotalProgress;				//整体进度。
	
	DWORD   AverageSpeed;				//平均速度
	DWORD   UsedSecond;					//更新己经用了多少秒。
	
	DWORD   LeftSecond;					//还需要多少秒。

	__int64 TotalFileSize;				//总共需要更新的字节数。//当空间不足时，这个值是告诉菜单需要删除的字节数。他应该是大于等于总需要更新的字节数。
	__int64 UpdateFizeSize;				//己更新字节数。
	
	char 	szCurFile[260];		//当前正在操作的文件.如果有错，则他是错误信息.
};



////////////////接口定义/////////////////////////////
//DWORD WINAPI FastCopy_Start(LPCSTR psrc,			//游戏在服务器上的目录
//							LPCSTR pdest,			//游戏更新到的目的目录
//							LPCSTR pMainSrvIP,		//主服务器ip
//							LPCSTR pSecondSrvIp,	//从服务器ip
//							DWORD GID,				//游戏gid
//							FC_COPY_MODE mode,		//更新模式
//							bool FiOpenp2p,			//是否p2p
//							DWORD dwProtSize );		//dwProtSize 表示是剩余多大空间。
//
//void WINAPI FastCopy_End(DWORD handle);
//
//void WINAPI FastCopy_GetSpeedEx(DWORD handle, FastCopyStatus* status);
//
//void WINAPI FastCopy_Cancel(DWORD handle);

//动态加载定义
typedef DWORD (WINAPI *FASTCOPY_START)(LPCSTR psrc,			//游戏在服务器上的目录
							LPCSTR pdest,			//游戏更新到的目的目录
							LPCSTR pMainSrvIP,		//主服务器ip
							LPCSTR pSecondSrvIp,	//从服务器ip
							DWORD GID,				//游戏gid
							FC_COPY_MODE mode,		//更新模式
							bool FiOpenp2p,			//是否p2p
							DWORD dwProtSize );		//dwProtSize 表示是剩余多大空间。

typedef void (WINAPI *FASTCOPY_END)(DWORD handle);

typedef void (WINAPI *FASTCOPY_GETSPEEDEX)(DWORD handle, FastCopyStatus* status);