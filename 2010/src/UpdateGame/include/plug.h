#ifndef _i8desk_plug_h_
#define _i8desk_plug_h_

#include "DbEngine.h"
#include "NetLayer.h"
#include "define.h"
#include "irtdatasvr.h"

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

	template <class T>
	struct CGuard		
	{
	private:
		T& m_lock;
	public:
		CGuard(T &lock) : m_lock(lock) 
		{
			m_lock.Lock();
		}
		~CGuard()
		{
			m_lock.UnLock();
		}
	};

	template <class T>
	void Release_Interface(T &pt)
	{
		if (pt != NULL)
			pt->Release();
		pt = NULL;
	}

	template <typename T>
	void Delete_Pointer(T* & p)
	{
		if (p != NULL)
		{
			delete p;
			p = NULL;
		}
	}
	template <typename T>
	void Delete_Array(T* &p)
	{
		if (p != NULL)
		{
			delete []p;
			p = NULL;
		}
	}

	//forward declare.
	struct IPlug;

	//主框架程序来实现该接口.
	struct IPlugMgr
	{
		//释放插件
		virtual void		Release()					= 0;

		//得到底层的数据库统一处理接口.
		virtual IDatabase*	GetDatabase()				= 0;

		//得到底层的系统选项接口
		virtual ISysOpt*	GetSysOpt()					= 0;

		//创建日志组件
		virtual ILogger*	CreateLogger()				= 0;

		//通过插件名字查找具体系统加载的一个组件.每个插件用一个唯一的名字来标识
		virtual IPlug*		FindPlug(LPCTSTR plugName)	= 0;

		//启动插件管理器,他负责加载所有系统的核心组件以及插件。
		virtual bool		Start()						= 0;

		//停止插件管理器,他负责释放所有加载的组件
		virtual bool		Stop()						= 0;
	};

	//业务插件就是实现一个IPlug接口中，并导出一个创建函数,由主框架来启动
	struct IPlug : public INetLayerEvent	//每个插件可以重载INetLayerEvent虚拟数来实现网络事件的处理
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

		//如果插件需要框架监听的网络端口，如果不需要监听，则返回0.
		//当框架收到该端口事件时，会回调给该接口的INetLayerEvent对应的虚函数
		virtual DWORD   GetListPort()					= 0;
		
		//插件内部的初始化工作,当IPlugMgr加载一个插件后会调用该函数
		virtual bool	Initialize(IPlugMgr* pPlugMgr)	= 0;

		//当IPlugMgr退出时会调先调用该函数后，才调用Release()来释放插件。
		virtual bool	UnInitialize() 					= 0;

		//主框架或者其它插件可以通过该函数来相互交互。类似于Windows的消息机制,
		//根据cmd的不同，param1,parma2实际的意义与不同.
		//如果需要在插件内部分配内存返回给调用者。则插件内部通过CoTaskMemFree来分配内存，
		//调用者通过CoTaskMemFree来释放插件分配的内存，因此插件接口不再提供其它内存分配释放内存.
		virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2) = 0;
	};


	//每个插件导出该函数类型的函数。并且该函数的转出序号是1.而且函数名是CreatePlug.
	typedef IPlug* (WINAPI *PFNCREATEPLUG)();
}
#endif