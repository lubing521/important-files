#ifndef __UPDATE_GAME_HPP
#define __UPDATE_GAME_HPP


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

enum
{
	// 通知三层强制修改某个游戏
	CMD_UPT_SM_NOFIGY3UPFORCE	= 666,

	// 取得在线客户机列表
	CMD_UPT_SM_CLIENTONLINE		= 0x01
};


// 插件通信
enum
{
	CMD_UPT_SM_GETUPDATGAME	= 0x01,		// 取得正在进行内网更新的游戏状态
	CMD_UPT_SM_GETSYNCGAME	= 0x02,		// 取得正在同步的游戏的状态
	CMD_UPT_SM_IPHAVEGAME	= 0x03,		// 取得某个客户机有哪些游戏(param1:ip).
	CMD_UPT_SM_GAMEINIP		= 0x04,		// 取得某个游戏在哪些计算机上有。(param1:gid).
	CMD_UPT_SM_NOTIFYSYNC	= 0x05,		// 通知同步游戏:nofity second server sync game(param2=gid, param2=1(forceupdate))
	CMD_UPT_SM_NOTIFYDELETE	= 0x06,		// 通知删除游戏:notify second server delete game.(param1=gid).
	CMD_UPT_SM_OPERATE		= 0x07,		// 针对同步任务进行操作
};	

enum SyncOperateState
{
	StartTask,					// 开始任务同步
	StopTask,					// 停止任务同步
	AddGame,					// 添加同步游戏
	DeleteGame,					// 删除同步游戏
	SuspendTask,				// 暂停同步游戏
	ResumeTask,					// 恢复同步游戏
	PutTopGame,					// 置顶同步游戏
	PutBottonGame,				// 置底同步游戏
	NotifySync,					// 通知同步
};

enum UpdateErrorCode //更新错误代码
{
    UPT_ERR_NONE,        //正常
    UPT_ERR_DIRNOTOK,    //目录名不正确
    UPT_ERR_CREATEDIR,   //创建目录失败
    UPT_ERR_INNERERR,    //内部错误
    UPT_ERR_LOADDEVICE,  //加载虚拟设备出错
    UPT_ERR_GETGAMEINFO, //取游戏信息失败
    UPT_ERR_DWNIDX,      //下载索引失败
    UPT_ERR_DISKNOROOM,  //磁盘空间不足
    UPT_ERR_PARSEIDX,    //解析索引失败
    UPT_ERR_CONNFAIL,    //连接服务器失败
    UPT_ERR_DWNFAIL,     //下载数据块失败
    UPT_ERR_CRCERR,      //数据块crc不同
    UPT_ERR_FILEOPEN,    //打开文件失败
    UPT_ERR_FILEWRITE,   //写文件失败
    UPT_ERR_FILEPOPDATA, //接受数据失败
    UPT_ERR_FILECLOSE    //关闭文件失败
};

typedef struct tagUpdateStatus
{
	DWORD           cbSize;					//结构体大小，用于版本控制
	DWORD           dwStatus;				//更新状态
	__int64         qNeedsUpdateBytes;		//需要更新的字节数
	__int64         qUpdatedBytes;			//己经更新的字节数
	DWORD           dwLeftTime;				//估计剩余时间
	DWORD           dwSpeed;				//更新速度k/s
    UpdateErrorCode enErrCode;              //错误代码
	wchar_t         awInfo[1024];			//相关信息，文件名或者错误信息
}tagUpdateStatus;

typedef void (__stdcall *OnConnect_pfn)(void* pParam, DWORD dwIP);

i8desk::ISvrPlug*	__stdcall CreateSvrPlug(DWORD Reserved);
i8desk::ICliPlug*	__stdcall CreateClientPlug(DWORD Reserved);


/*
0表示推送，1表示同步，2表示内网更新.
*/

enum 
{
	Push = 0,
	Sync,
	Update
};
HANDLE	__stdcall UG_StartUpdate(DWORD gid, LPCWSTR tid, LPCWSTR src, LPCWSTR dst, DWORD flag, 
								 LPCWSTR sip, LPCWSTR cip = NULL, DWORD speed = 0, DWORD type = 0);
void	__stdcall UG_StopUpdate(HANDLE handle);
void	__stdcall UG_PauseUpdate(HANDLE handle);
void	__stdcall UG_ResumeUpdate(HANDLE handle);
bool	__stdcall UG_GetUpdateStatus(HANDLE handle, tagUpdateStatus* pStatus);
bool    __stdcall UG_SetConnectCallBack(HANDLE handle, OnConnect_pfn pfnCallBack, void* pCallBackParam);

#endif