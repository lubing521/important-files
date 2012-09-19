#ifndef __plug_inc__
#define __plug_inc__

#pragma once
#pragma warning(disable:4200)

#include "define.h"
#include "irtdatasvr.h"

#import "c:\program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF","adoEOF")

namespace i8desk
{


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

//统一的数据操作接口.
struct IDatabase 
{
	//释放接口
	virtual void Release()			= 0;

	//打开数据库
	virtual bool Open(LPCTSTR lpszConnString, LPCTSTR lpszUser = TEXT(""), LPCTSTR lpszPassword = TEXT("")) = 0;

	//关闭数据库
	virtual void Close()			= 0;

	//得到数据库操作错误的信息
	virtual LPCTSTR GetErrInfo()	= 0;

	//得到对应该的错误代码
	virtual long GetErrCode()		= 0;

	//锁住数据库组件
	virtual void Lock()				= 0;

	//解锁数据库组件
	virtual void UnLock()			= 0;

	//事务管理，开始，提交，回滚事务
	virtual void BeginTrans()		= 0;
	virtual void CommitTrans()		= 0;
	virtual void RollbackTrans()	= 0;

	//执行SQL语句.不需要得到返回结果，如果是插件，删除，修改，可以通过第二个参数得到影响的行。
	//如果不需要第二个参数的值，可以设置空，或者不填（有默认值）
	virtual bool ExecSql(LPCTSTR lpszSql, PLONG RecordsAffected = NULL) = 0;

	//执行SQL语句.需要得到返回的结果集.返回的是ＡＤＯ的Recordset接口接口.如果成功需要自己执行prcd->Close();
	//ex. 
	//	CAutoLock<IDatabase> lock(m_pDatabase);
	//  _RecordsetPrt prcd;
	//  if (m_pDatabase->ExecSql(lpszSql, prcd))
	//	{
	//		while (prcd->IsEof() == VARIANT_FALSE)
	//		{
	//			//get line value.	
	//		}
	//		prcd->Close();
	//		prcd.Release();	//可以不需要，出了作用域，智能指针会自己调用释放接口。
	//	}
	//	else
	//		OutputDebugString(m_pDatabase->GetErrInfo());

	virtual bool ExecSql(LPCTSTR lpszSql, _RecordsetPtr& prcd)			= 0;

	//工具函数:压缩数据库.只能要没有打开数据时才能压缩.
	virtual bool CompactDatabase(LPCTSTR lpszDbFile) = 0;
};

//系统统一的选项操作。（他是存诸在数据库里的一个表里）
struct ISysOpt
{
	//释放接口
	virtual void	Release()										= 0;

	//09是设置成数据库的组件，10是设置成数据库缓存组件返回的选项表指针。(11和10相同)
	virtual void	SetIDatabase(void* pDatabase)					= 0;

	//得到系统选项.如果表里没有这个选项。会自动插入，并写上默认值
	virtual bool	GetOpt(LPCTSTR optName, bool defValue = true)	= 0;
	virtual long	GetOpt(LPCTSTR optName, long defValue = 0)		= 0;
	virtual __int64 GetOpt(LPCTSTR optName, __int64 defValue = 0)	= 0;
	virtual void	GetOpt(LPCTSTR optName, LPTSTR lpszValue, LPCTSTR defValue = TEXT("")) = 0;

	//设置系统选项.如果表里没有这个选项。会自动插入，并写上设置的值
	virtual void	SetOpt(LPCTSTR optName, bool optValue)			= 0;
	virtual void	SetOpt(LPCTSTR optName, long optValue)			= 0;
	virtual void	SetOpt(LPCTSTR optName, __int64 optValue)		= 0;
	virtual void	SetOpt(LPCTSTR optName, LPCTSTR optValue)		= 0;
};

//系统通用写日志的组件。创建组件后，一定要设置自己的日志文件名（真正的文件名是在（设置文件名）-yyyymmdd.log）
enum I8_Log_Priority
{
	LM_DEBUG	= 0,		//调试信息
	LM_INFO		= 1,		//正常的日志输出
	LM_WARNING	= 2,		//表示有可能是有问题，但并不确定
	LM_ERROR	= 3			//表示出错时，软件输出的日志，需要用户手工处理
};

struct ILogger
{
	virtual void Release()	= 0;

	//设置日志文件名.仅需要文件名(不要带路径以及扩展名).比如log,最后生成的文件名是\log\log-yyymmdd.log.
	//内部会自动根据当天的日期生成日志。（即会自动在文件名后添加当天的日期，并加上扩展名.log,并统一放到log目录）
	//建立以插件名字为日志文件名.这样，所有的日志文件名不会重复，也方便查看
	virtual void SetLogFileName(LPCTSTR lpszLogFile)	= 0;

	//设置日志文件最大的大小。(字节为单位)
	virtual void SetMaxLogSize(DWORD dwMaxSize)			= 0;

	//在每条日志前添加日志输出的日期
	virtual void SetAddDateBefore(bool bAddDate)		= 0;

	//在每条日志后添加一个回车换行符
	virtual void SetAddCrLfAfter(bool bAddCrLf)			= 0;

	//设置多个内部属性。
	virtual void SetProptery(LPCTSTR lpszLogFile, DWORD dwMaxLogSize, bool bAddDate) = 0;

	//设置日志输出到控制台的ip,port.内部通过udp发给控制台。//网络序
	virtual void SetConsole(DWORD ip, DWORD port)		= 0;

	//得到组件的属性
	virtual LPCTSTR GetLogFileName()					= 0;
	virtual DWORD	GetMaxLogSize()						= 0;
	virtual bool	GetAddCrLfAfter()					= 0;
	virtual bool	GetAddDateBefore()					= 0;

	//写一条日志到数据
	virtual void __cdecl WriteLog(DWORD level, LPCTSTR lpszFormater, ...)		= 0;

	//清空所有的日志
	virtual void CleanLog()								= 0;
};

BOOL WINAPI CreateDatabase(IDatabase** ppIDatabase);
BOOL WINAPI CreateSysOpt(ISysOpt** ppISysOpt);
BOOL WINAPI CreateLogger(ILogger** ppILogger);


//数据包协议头
#define START_FLAG		0x5E7D
typedef struct pkgheader
{
	WORD  StartFlag;
	WORD  Cmd;
	DWORD Length;
	WORD  Version;
	BYTE  Resv[6];
	BYTE  data[0];
}pkgheader;

//内存池组件，使用INetLayer组件，必须通过该内存池来分配接收，发送缓冲区
struct IMemPool
{
	virtual void  Release() = 0;

	//初始池中分配的节点数,每个结点块的大小,增长数。
	virtual void  SetDefault(DWORD dwdefPoolSize, DWORD dwdefBlockSize, DWORD dwdefIncrease) = 0;
	virtual DWORD GetDefPoolSize() = 0;		
	virtual DWORD GetDefBlockSize() = 0;
	virtual DWORD GetDefIncrease() = 0;

	//分配，释放，释放所有内存池中节点
	virtual char* Alloc(DWORD dwSize=0) = 0;	//如果分配大于默认的节点块大小时，需要告诉需要分配的大小
	virtual void  Free(void* pBlock) = 0;
	virtual void  FreeAll() = 0;

	//得到分配的一个节点的大小
	virtual DWORD GetBufSize(void* pBlock) = 0; 
	//检查内存节点是否从内存池中分配
	virtual BOOL  IsAllocFrom(void* pBlock) = 0;
};

struct INetLayerEvent;

//数据包头解析组件。用于分包处理。如果没有向INetLayer设置一个实例，组件在内部有一个默认实现。
struct IPackageParse
{
	virtual void  Release() = 0;
	virtual DWORD ParasePackage(char* package, DWORD length) = 0;
};

//网络层组件
struct INetLayer
{
	virtual void Release() = 0;

	//设置，取得网络层事件处理接口指针
	virtual void SetINetLayerEvent(INetLayerEvent* pINetLayerEvent) = 0;
	virtual INetLayerEvent* GetINetLayerEvent() = 0;

	//设置，取得内存池接口指针
	virtual void SetIMemPool(IMemPool* pIMemPool) = 0;
	virtual IMemPool* GetIMemPool() = 0;

	//设置，取得包解析接口指针
	virtual void SetIPackageParse(IPackageParse* pPackageParse) = 0;
	virtual IPackageParse* GetIPackageParse() = 0;

	//启动网络层，停止网络层组件
	virtual DWORD Start(WORD wPort = 7918) = 0; //参数：需要监听的端口
	virtual void  Stop() = 0;
	virtual WORD  GetPort() = 0;

	//发送给客户端一个数据包
	virtual void  SendPackage(SOCKET sck, LPSTR pPackage, DWORD length) = 0;

	//服务端主动断开一个客户端
	virtual void  DisConnect(SOCKET sck) = 0;
};

struct INetLayerEvent
{
	virtual void Release() = 0;

	//当客户端连接上来时，产生该事件
	virtual void OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param) = 0;

	//当服务端发客户端发送完成一个数据包时产生该事件
	virtual void OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) = 0; 

	//当服务端接收到客户端一个完整命令包时，产生该事件给应用层处理一个命令
	virtual void OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) = 0;

	//当客户端断开时产生该事件.
	virtual void OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param) = 0;
};

BOOL __stdcall CreateIMemPool(IMemPool** pIMemPool);
BOOL __stdcall CreateINetLayer(INetLayer** pINetLayer);

//服务端插件与插件管理器的接口定义
//forward declare.
struct ISvrPlug;

struct ISvrPlugMgr
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
	virtual ISvrPlug*	FindPlug(LPCTSTR plugName)	= 0;

	//启动插件管理器,他负责加载所有系统的核心组件以及插件。
	virtual bool		Start()						= 0;

	//停止插件管理器,他负责释放所有加载的组件
	virtual bool		Stop()						= 0;
};

//业务插件就是实现一个ISvrPlug接口中，并导出一个创建函数,由主框架来启动
struct ISvrPlug : public INetLayerEvent	//每个插件可以重载INetLayerEvent虚拟数来实现网络事件的处理
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

	//插件内部的初始化工作,当ISvrPlugMgr加载一个插件后会调用该函数
	virtual bool	Initialize(ISvrPlugMgr* pPlugMgr)	= 0;

	//当ISvrPlugMgr退出时会调先调用该函数后，才调用Release()来释放插件。
	virtual bool	UnInitialize() 					= 0;

	//主框架或者其它插件可以通过该函数来相互交互。类似于Windows的消息机制,
	//根据cmd的不同，param1,parma2实际的意义与不同.
	//如果需要在插件内部分配内存返回给调用者。则插件内部通过CoTaskMemFree来分配内存，
	//调用者通过CoTaskMemFree来释放插件分配的内存，因此插件接口不再提供其它内存分配释放内存.
	virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2) = 0;
};

//每个插件导出该函数类型的函数。必须保证函数名是CreateSvrPlug.
typedef ISvrPlug* (WINAPI *PFNCREATESVRPLUG)(DWORD Reserved);

//客户端插件与插件管理器的接口定义
struct ICommunication
{
	//释放插件
	virtual void	Release()									= 0;

	//启动通信组件(内部打开TCP, UDP)
	virtual bool    Start()										= 0;

	//停止通信组件(内部关闭TCP, UDP)
	virtual void	Stop()										= 0;

	//获取TCP连接套接字。返回INVALID_SOCKET表示没有连接上
	virtual SOCKET	GetTcpSocket()								= 0;

	//用TCP方式和服务端进行一次通信
	virtual void	Lock()										= 0;
	virtual bool	ExecCmd(char*pOutData, int nOutLen, char* &pInData, int& nInLen, bool bNeedAck = true) = 0;
	virtual void	Unlock()									= 0;

	//接收到的udp数据的回调函数
	virtual void	UdpRecvNotify(char* pData, int length, sockaddr_in& from)	= 0;
};

//forward decalre.
struct ICliPlug;

struct ICliPlugMgr
{
	//释放插件
	virtual void			Release()				= 0;

	//创建日志组件.调用者要记得调用ILogger->Release().
	virtual ILogger*		CreateLogger()			= 0;

	//得到主框架实现的通信接口指针
	virtual ICommunication*	GetCommunication()		= 0;

	//通过插件名字查找具体系统加载的一个组件.每个插件用一个唯一的名字来标识
	virtual ICliPlug*		FindPlug(LPCTSTR plugName)	= 0;

	//启动插件管理器,他负责加载所有系统的核心组件以及插件。
	virtual bool			Start()					= 0;

	//停止插件管理器,他负责释放所有加载的组件
    virtual void			Stop()					= 0;

    //允许插件向插件管理器发出命令
    virtual DWORD           SendMessage(DWORD cmd, DWORD param1, DWORD param2) = 0;
};


//业务插件就是实现一个IPlug接口中，并导出一个创建函数,由主框架来启动
struct ICliPlug
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

	//插件内部的初始化工作,当ICliPlugMgr加载一个插件后会调用该函数
	virtual bool	Initialize(ICliPlugMgr* pPlugMgr)	= 0;
	//当ICliPlugMgr退出时会调先调用该函数后，才调用Release()来释放插件。
	virtual bool	UnInitialize() 					= 0;

	//主框架或者其它插件可以通过该函数来相互交互。类似于Windows的消息机制,
	//根据cmd的不同，param1,parma2实际的意义与不同.
	//如果需要在插件内部分配内存返回给调用者。则插件内部通过CoTaskMemFree来分配内存，
	//调用者通过CoTaskMemFree来释放插件分配的内存，因此插件接口不再提供其它内存分配释放内存.
	virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2) = 0;

	//接收到的udp数据的回调函数
	virtual void	UdpRecvNotify(char* pData, int length, sockaddr_in& from) = 0;
};


//每个插件导出该函数类型的函数。并且该函数的转出序号是1.而且函数名是CreateClientPlug.
typedef ICliPlug* (WINAPI *PFNCREATECLIENTPLUG)(DWORD Reserved);

class CPkgHelper
{
private:
	LPSTR  m_lpPackage;
	size_t m_nBufSize;
	DWORD  m_dwPosition;
public:
	//for push construct.
	CPkgHelper(LPSTR lpPackage, size_t nBufSize) : m_lpPackage(lpPackage), m_nBufSize(nBufSize),
		m_dwPosition(sizeof(pkgheader))
	{
		GetHeader()->StartFlag	= START_FLAG;
		GetHeader()->Version	= PRO_VERSION;
		GetHeader()->Length		= sizeof(pkgheader);
	}

	CPkgHelper(LPSTR lpPackage, size_t nBufSize, WORD cmd, OBJECT_TYPE type) 
		: m_lpPackage(lpPackage)
		, m_nBufSize(nBufSize)
		, m_dwPosition(sizeof(pkgheader))
	{
		GetHeader()->StartFlag	= START_FLAG;
		GetHeader()->Version	= PRO_VERSION;
		GetHeader()->Length		= sizeof(pkgheader);
		GetHeader()->Cmd		= cmd;
		GetHeader()->Resv[0]	= (BYTE)type;
	}

	template<size_t N>
	explicit CPkgHelper(char (&arr)[N]) 
		: m_lpPackage(arr)
		, m_nBufSize(N)
		, m_dwPosition(sizeof(pkgheader))
	{
		GetHeader()->StartFlag	= START_FLAG;
		GetHeader()->Version	= PRO_VERSION;
		GetHeader()->Length		= sizeof(pkgheader);
	}

	template<size_t N>
	CPkgHelper(char (&arr)[N], WORD cmd, OBJECT_TYPE type) 
		: m_lpPackage(arr)
		, m_nBufSize(N)
		, m_dwPosition(sizeof(pkgheader))
	{
		GetHeader()->StartFlag	= START_FLAG;
		GetHeader()->Version	= PRO_VERSION;
		GetHeader()->Cmd        = cmd;
		GetHeader()->Length		= sizeof(pkgheader);
		GetHeader()->Resv[0]	= type;
	}

	//for pop construct.
	explicit CPkgHelper(LPCSTR lpPackage) 
		: m_lpPackage(const_cast<LPSTR>(lpPackage))
		, m_nBufSize(0)
		, m_dwPosition(sizeof(pkgheader))
	{
		m_nBufSize = GetHeader()->Length;
	}
public:	
	pkgheader* GetHeader()				{ return reinterpret_cast<pkgheader*>(m_lpPackage); }
	LPCSTR	GetBuffer(void) const		{ return m_lpPackage;				}
	void	SetLength(DWORD dwLength)	{ GetHeader()->Length = dwLength;	}
	DWORD	GetLength()					{ return GetHeader()->Length;		}

	void	pushHeader(pkgheader& header)  { *GetHeader() = header;			}
	void	popHeader(pkgheader& header)   { header = *GetHeader();			}

	template <typename T>
	void Push(const T& Elem)
	{
		int nSize = sizeof(T);
		memcpy(m_lpPackage + m_dwPosition, (void*)&Elem, nSize);
		m_dwPosition += nSize;
		GetHeader()->Length = m_dwPosition;
	}

	template <typename T>
	void PushPointer(const T* pointer, int nCount = 1)
	{
		int nSize = sizeof(T) * nCount;
		memcpy(&m_lpPackage[m_dwPosition], (void*)pointer, nSize);
		m_dwPosition += nSize;
		GetHeader()->Length = m_dwPosition;
	}
	
	template <typename T>
	void PushString(const T* pString, int nCharNums)
	{
		Push(nCharNums);
		PushPointer(pString, nCharNums);
		Push(T(0));
	}

	template <typename T, size_t N>
	void PushString(const T (&arr)[N])
	{
		struct StringLenHelper
		{
			size_t len_;

			StringLenHelper(const char *str)
			{ len_ = strlen(str);	}
			StringLenHelper(const wchar_t *str)
			{ len_ = wcslen(str);	}

			size_t operator()() const
			{ return len_;			}
		};
		size_t len = StringLenHelper(arr)();

		PushString(arr, len);
    }
    void PushDWORD(const DWORD& Elem)
    {
        int nSize = sizeof(DWORD);
        memcpy(m_lpPackage + m_dwPosition, (void*)&Elem, nSize);
        m_dwPosition += nSize;
        GetHeader()->Length = m_dwPosition;
    }


	template <typename T>
	T Pop(T& Elem)
	{
		memcpy((void*)&Elem, m_lpPackage + m_dwPosition, sizeof(T));
		m_dwPosition += sizeof(T);
		return Elem;
	}

	template <typename T>
	void PopPointer(T* pointer, int nCount = 1)
	{
		int nSize = nCount * sizeof(T);
		memcpy((void*)pointer, m_lpPackage + m_dwPosition, nSize);
		m_dwPosition += nSize;
	}

	template <typename T>
	DWORD PopString(T* pString, int nCharNums)
	{
		DWORD length = 0;
		Pop(length);
		
		if( nCharNums < (int)length )
			throw std::out_of_range("package out of range");

		PopPointer(pString, length + 1);
		return length;
	}

	template <typename T, size_t N>
	DWORD PopString(T (&arr)[N])
	{
		DWORD length = 0;
		Pop(length);

		if( N < length  )
			throw std::out_of_range("package out of range");

		PopPointer(arr, (length+1));
		return length;
    }
    DWORD PopDWORD()
    {
        DWORD dwData;
        memcpy((void*)&dwData, m_lpPackage + m_dwPosition, sizeof(DWORD));
        m_dwPosition += sizeof(DWORD);
        return dwData;
    }
};

namespace detail
{
	template < typename T >
	struct Select
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const T &val)
		{
			package.Push(val);
			return package;
		}
	};

	template < typename T >
	struct Select<T *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const T *val)
		{
			package.PushPointer(val);
			return package;
		}
	};


	template <>
	struct Select<char *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const char* const val)
		{
			package.PushString(val, strlen(val));
			return package;
		}
	};

	template <>
	struct Select<const char *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const char* const val)
		{
			return Select<char *>::Dispacth(package, val);
		}
	};


	template <>
	struct Select<wchar_t *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const wchar_t* const val)
		{
			package.PushString(val, wcslen(val));
			return package;
		}
	};

	template <>
	struct Select<const wchar_t *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const wchar_t* const val)
		{
			return Select<wchar_t *>::Dispacth(package, val);
		}
	};
}

//CPkgHelper stream operator.
template <typename T>
inline CPkgHelper& operator<<(CPkgHelper& package, const T& Value)
{
	return detail::Select<T>::Dispacth(package, Value);
}


// -----------
template<typename CharT>
inline CPkgHelper& operator<<(CPkgHelper& package, const std::basic_string<CharT>& Value)
{
	package.PushString(Value.c_str(), Value.length());
	return package;
}

template<typename CharT, size_t N>
inline CPkgHelper &operator<<(CPkgHelper &package, const CharT (&arr)[N])
{
	package.PushString(arr);
	return package;
}


template <typename T>
inline CPkgHelper& operator>>(CPkgHelper& package, T& Value)
{
	package.Pop(Value);
	return package;
}

template<typename CharT>
inline CPkgHelper& operator>>(CPkgHelper& package, std::basic_string<CharT>& Value)
{
	int length = 0;
	CharT end = 0;

	package.Pop(length);
	Value.resize(length);
	package.PopPointer<CharT>(&Value[0], length);
	package.Pop(end);
	return package;
}

template<typename CharT, size_t N>
inline CPkgHelper &operator>>(CPkgHelper &package, CharT (&arr)[N])
{
	package.PopString(arr);
	return package;
}

}

#endif