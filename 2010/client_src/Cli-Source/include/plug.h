#ifndef _i8desk_client_plug_h_
#define _i8desk_client_plug_h_

#include "DbEngine.h"
#include "NetLayer.h"
#include "define.h"
#include "hy.h"

namespace i8desk
{
	//同步工具类.
	class CLock
	{
	private:
		CRITICAL_SECTION m_lock;
	public:
		CLock()			{ InitializeCriticalSection(&m_lock);	}
		~CLock()		{ DeleteCriticalSection(&m_lock);		}
		void Lock()		{ EnterCriticalSection(&m_lock);		}
		void UnLock()	{ LeaveCriticalSection(&m_lock);		}
	};

	//同步guard工具类  ex:CLock m_lock; CAutoLock<CLock> lock(m_lock);
	template <class T>
	struct CAutoLock		
	{
	private:
		T* m_pT;
	public:
		CAutoLock(T* pt): m_pT(pt) 
		{
			if (m_pT != NULL)
				m_pT->Lock();
		}
		~CAutoLock()
		{
			if (m_pT != NULL)
				m_pT->UnLock();
		}
	};

	//释放接口组件的辅助函数	
	template <class T>
	void Release_Interface(T &pt)
	{
		if (pt != NULL)
			pt->Release();
		pt = NULL;
	}

	//客户端与服务端的通信组件.(包括tcp通信，以及udp通知)
	typedef struct tagLocMachineInfo
	{
		TCHAR szName[MAX_PATH];			//计算机名
		TCHAR szIp[MAX_PATH];			//IP地址
		TCHAR szGate[MAX_PATH];			//网关
		TCHAR szSubMask[MAX_PATH];		//子网掩码
		TCHAR szDNS0[MAX_PATH];			//DNS1地址
		TCHAR szDNS1[MAX_PATH];			//DNS2地址	
		TCHAR szMac[MAX_PATH];			//MAC地址
		DWORD ProtInstal;				//是否安装还原
		TCHAR ProtArea[MAX_PATH];		//还原保护的区域
		TCHAR AllDrivers[MAX_PATH];		//所有分区
		TCHAR ProtVer[MAX_PATH];		//还原版本
		TCHAR MenuVer[MAX_PATH];		//菜单版本
		TCHAR CliSvrVer[MAX_PATH];		//客户端版本

	}tagLocMachineInfo;
	struct ICommunication
	{
		//释放插件
		virtual void	Release()									= 0;

		//启动通信组件(内部打开TCP, UDP)
		virtual bool    Start()										= 0;

		//停止通信组件(内部关闭TCP, UDP)
		virtual void	Stop()										= 0;

		//查看连接是否建立(TCP, UDP)
		virtual bool	IsConnected()								= 0;

		//用TCP方式和服务端进行一次通信
		virtual void	Lock()										= 0;
		virtual bool	ExecCmd(char*pOutData, int nOutLen, char* &pInData, int& nInLen, bool bNeedAck = true) = 0;
		virtual void	UnLock()									= 0;

		//接收到的udp数据的回调函数
		virtual void	UdpRecvNotify(char* pData, int length, sockaddr_in& from)	= 0;

		virtual void    GetLocMachineInfo(tagLocMachineInfo& info)	= 0;

		//add by liubin.2009/09/01.表示是否是连接建立后第一包数据
		virtual bool	IsFirstPackage()							= 0;
	};
	struct IPlug;

	struct IPlugMgr
	{
		//释放插件
		virtual void			Release()				= 0;

		//创建日志组件.调用者要记得调用ILogger->Release().
		virtual ILogger*		CreateLogger()			= 0;

		//得到主框架实现的通信接口指针
		virtual ICommunication*	GetCommunication()		= 0;

		//通过插件名字查找具体系统加载的一个组件.每个插件用一个唯一的名字来标识
		virtual IPlug*			FindPlug(LPCTSTR plugName)	= 0;

		//启动插件管理器,他负责加载所有系统的核心组件以及插件。
		virtual bool			Start()					= 0;

		//停止插件管理器,他负责释放所有加载的组件
		virtual void			Stop()					= 0;
	};

	
	//业务插件就是实现一个IPlug接口中，并导出一个创建函数,由主框架来启动
	struct IPlug
	{
		//释放插件
		virtual void	Release()						= 0;

		//得到插件的名字
		virtual LPCTSTR GetPlugName()					= 0;
		//得到插件的版本0xaabbccdd.
		virtual DWORD  	GetVersion() 					= 0;
		//得到插件开发的公司名
		virtual LPCTSTR GetCompany()					= 0;
		//得到插件的一些描述
		virtual LPCTSTR GetDescription() 				= 0;

		virtual bool    NeedUdpNotity()					= 0;

		//插件内部的初始化工作,当IPlugMgr加载一个插件后会调用该函数
		virtual bool	Initialize(IPlugMgr* pPlugMgr)	= 0;
		//当IPlugMgr退出时会调先调用该函数后，才调用Release()来释放插件。
		virtual bool	UnInitialize() 					= 0;

		//主框架或者其它插件可以通过该函数来相互交互。类似于Windows的消息机制,
		//根据cmd的不同，param1,parma2实际的意义与不同.
		//如果需要在插件内部分配内存返回给调用者。则插件内部通过CoTaskMemFree来分配内存，
		//调用者通过CoTaskMemFree来释放插件分配的内存，因此插件接口不再提供其它内存分配释放内存.
		virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2) = 0;

		//接收到的udp数据的回调函数
		virtual void	UdpRecvNotify(char* pData, int length, sockaddr_in& from) = 0;
	};


	//每个插件导出该函数类型的函数。并且该函数的转出序号是1.而且函数名是CreatePlug.
	typedef IPlug* (WINAPI *PFNCREATEPLUG)();	
}
#endif