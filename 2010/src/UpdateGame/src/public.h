#ifndef __public_inc__
#define __public_inc__

#include "../include/plug.h"
#include <list>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")
using namespace i8desk;

#define PLUG_VERSION			0x0200
#define PORT_TCP_UPDATE			5969
#define PORT_UDP_UPDATE			5969
#define PORT_UDP_UI				5970
#define UPDATE_EVENT_NAME		TEXT("_force_stop_update_")

//////////////////////////////////////////////////////////////////////////
//内网更新与主服务器的交互命令

	//取游戏信息:gid + dir(game root dir(include last \\))| 0 + ver + filesize(low dword)
	#define CMD_UPT_GETGMAEINFO		0x0101

	//下载文件块:filesize(i64) + fileoffset(i64) + blksize + filename(not include root).
	//正象时返回文件数据的内容。错误会返回
	#define CMD_UPT_RECVBLOCK		0x0102

	//上报更新进度:ip + gid + speed + progress + needupdatebytes(i64) + leftbytes(i64) | 0.
	#define CMD_UPT_RPTPROGRESS		0x0103

	//更新完成上报:ip + gid + version(0表示异常完成) + dir.| 0
	#define CMD_UPT_RPTFINISH		0x0104

	//索引错误上报:gid | 0
	#define CMD_UPT_INDEXERR		0x0105

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//其它插件与更新服务端插件的通知与查询

	//取得正在进行内网更新的游戏状态
	/*
	<?xml version=\"1.0\" ?>"
	<status>
		<download>
			<task ip="long(net)" type="1" gid="1001", speed="k/s" progress="dd%%" uptsize="kb" leftsize="kb" />
		</download>
	</status>
	*/
	#define CMD_UPT_SM_GETUPDATGAME		0x01

	//取得正在同步的游戏的状态
	#define CMD_UPT_SM_GETSYNCGAME		0x02

	//取得某个客户机有哪些游戏(param1:ip).
	/*
	<?xml version=\"1.0\" ?>"
	<status>
		<gameLst>
			<Task gid="dword" version="dword" />
		</gameLst>
	</status>
	*/
	#define CMD_UPT_SM_IPHAVEGAME		0x03

	//取得某个游戏在哪些计算机上有。(param1:gid).
	/*
	<?xml version=\"1.0\" ?>"
	<status>
		<ipLst>
			<Task ip="long(net)" version="dword" />
		</ipLst>
	</status>
	*/
	#define CMD_UPT_SM_GAMEINIP			0x04

	//通知同步游戏:nofity second server sync game(param2=gid, param2=1(forceupdate))
	#define CMD_UPT_SM_NOTIFYSYNC		0x05

	//通知删除游戏:notify second server delete game.(param1=gid).
	#define CMD_UPT_SM_NOTIFYDELETE		0x06
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//本插件向主服务器取信息或者通知。

	//取得在线客户机列表
	//param1=iplist,param2=pcount.
	#define CMD_UPT_SM_CLIENTONLINE		0x01

	//通知三层强制修改某个游戏
	#define CMD_UPT_SM_NOFIGY3UPFORCE	666
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//同步插件的相关命令
	//取得所有的同步任务  | 0 + taskcount {SID+SyncType+SyncIp+VDiskIp+UpdateIp+Driver+BalanceType+speed);
	#define CMD_SYNC_GETALLTASK		0x0201

	//取得某个同步任务所有的游戏。SID + SyncType | 0 + gamecount {gid+size(k)+svrver+name+svrpath}
	#define CMD_SYNC_GETTASKGAME	0x0202

	//设置游戏版本号.   gid + ver|0. (同步时发现更新量为0，版本号不同时，回写主服务器的游戏版本。
	#define CMD_SYNC_SETGMAEVER		0x0203
//////////////////////////////////////////////////////////////////////////


inline void InitPkgheader(_packageheader* pheader, WORD cmd)
{
	ZeroMemory(pheader, sizeof(pheader));
	pheader->StartFlag = START_FLAG;
	pheader->Cmd = cmd;
	pheader->Length = sizeof(_packageheader);
	pheader->Version = PLUG_VERSION;
}

#define POOL_SIZE	20

//Producer - Consumer - Module.
template<class _Ty>
struct _func_
{
	void operator()(_Ty& obj)
	{
	}
};

template<typename object, typename Func = _func_<object>, size_t poolsize = POOL_SIZE>
class PCMomule
{
private:
	HANDLE	m_hExit;
	CLock	m_lock;
	HANDLE	m_hPop;
	HANDLE	m_hPush;
	std::list<object>	m_list;
public:
	PCMomule(HANDLE  hExit):m_hExit(hExit)
	{
		m_hPush = CreateSemaphore(NULL, poolsize, poolsize, NULL);
		m_hPop  = CreateSemaphore(NULL, 0,		  poolsize, NULL);
	}
	~PCMomule()
	{
		CloseHandle(m_hPop);
		CloseHandle(m_hPush);

		Func f;
		CAutoLock<CLock> lock(&m_lock);
		std::for_each(m_list.begin(), m_list.end(), f);
		m_list.clear();
	}
	size_t GetPoolSize() { return poolsize; }
	bool Push(object &obj)
	{
		HANDLE event[] = {m_hExit, m_hPush};
		DWORD dwRet = WaitForMultipleObjects(_countof(event), event, FALSE, INFINITE);
		if (dwRet != WAIT_OBJECT_0 + 1)
			return false;
		
		CAutoLock<CLock> lock(&m_lock);
		m_list.push_back(obj);
		ReleaseSemaphore(m_hPop, 1, NULL);
		return true;
	}
	bool Pop(object& obj)
	{
		HANDLE event[] = {m_hExit, m_hPop};
		DWORD dwRet = WaitForMultipleObjects(_countof(event), event, FALSE, INFINITE);
		if (dwRet != WAIT_OBJECT_0 + 1)
			return false;

		CAutoLock<CLock> lock(&m_lock);
		obj = m_list.front();
		m_list.pop_front();
		ReleaseSemaphore(m_hPush, 1, NULL);
		return true;
	}
};

class CEvenyOneSD
{
public:
	PVOID Build(PSECURITY_DESCRIPTOR pSD)
	{
		PSID   psidEveryone = NULL;
		PACL   pDACL   = NULL;
		BOOL   bResult = FALSE;
		PACCESS_ALLOWED_ACE pACE = NULL;
		
		try
		{
			SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY;
			SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
			
			if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
				throw GetLastError();

			if (!AllocateAndInitializeSid(&siaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone))
				throw GetLastError();

			DWORD dwAclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(psidEveryone);

			pDACL = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength);
			if (!pDACL) 
				throw GetLastError();

			if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION))
				throw GetLastError();

			if (!AddAccessAllowedAce(pDACL, ACL_REVISION, GENERIC_ALL, psidEveryone))
				throw GetLastError();

			if (!SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE))
				throw GetLastError();
			bResult = TRUE;
		}
		catch (...)  { }

		if (psidEveryone)
		{
			FreeSid(psidEveryone);
			psidEveryone = NULL;
		}
		if (bResult == FALSE) 
		{
			if (pDACL) 
				HeapFree(GetProcessHeap(), 0, pDACL);
			pDACL = NULL;
		}

		return pDACL;
	}
	CEvenyOneSD()
	{
		ptr=NULL;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;
		ptr = Build(&sd);
	}
	virtual ~CEvenyOneSD()
	{
		if(ptr)
		{
			HeapFree(GetProcessHeap(), 0, ptr);
		}
	}
	SECURITY_ATTRIBUTES* GetSA()
	{
		return (ptr != NULL) ? &sa : NULL;
	}
protected:
	PVOID  ptr;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
};

#endif