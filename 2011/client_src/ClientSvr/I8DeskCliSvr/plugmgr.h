#ifndef _i8desk_plugmgr_h_
#define _i8desk_plugmgr_h_

#include <map>
#include "MultiThread/Lock.hpp"

namespace i8desk
{
	//实现通信组件.
	class CCommunication :public ICommunication
	{
	private:
		ILogger*	m_pLogger;
		stdex::tString		m_SrvIP;
		SOCKET		m_sckTcp, m_sckUdp;
		HANDLE		m_hTcpThread, m_hUdpThread; 
		HANDLE		m_hExit;
        async::thread::AutoCriticalSection m_lockTcp;
	public:
		CCommunication(ILogger* pLogger);
		virtual ~CCommunication();
		virtual void	Release()	{ delete this; }
	public:
		//实现ICommunication接口的函数(UdpRecvNotify除外)
		virtual bool Start();
		virtual void Stop();
		virtual SOCKET	GetTcpSocket() {return m_sckTcp;};
		virtual void Lock()   { m_lockTcp.Lock(); }
		virtual bool ExecCmd(char*pOutData, int nOutLen, char* &pInData, int& nInLen, bool bNeedAck);
		virtual void Unlock() { m_lockTcp.Unlock(); }
	private:
		//创建,关闭TCP,UDP.
		SOCKET _CreateUdpSocket();	//创建成功后返回一个新的socket
		void   _CloseUdpSocket();
		SOCKET _ConnectTcpServer();	//连接成功后返回一个新的socket.
		void   _CloseTcpSocket();

		//检测TCP是否断开，如果己经断开，就要进行重连
		static UINT __stdcall TcpWorkThread(LPVOID lpVoid);

		//接收UDP的数据,并进行插件的回调
		static UINT __stdcall UdpWorkThread(LPVOID lpVoid);
	};

	//实现插件管理器
    typedef BOOL (WINAPI * PFNCREATELOGGER)(ILogger** ppILogger);
	class CPlugMgr : public ICliPlugMgr, public CCommunication
	{
	private:
		ILogger* m_pLogger;
		std::map<stdex::tString,  ICliPlug*> m_mapName2Plugs;		//所有的插件
		typedef std::map<stdex::tString, ICliPlug*>::iterator	NAME2PLUGITER;
        async::thread::AutoCriticalSection m_lock;	
		PFNCREATELOGGER               pfnCreateLogger;
		HMODULE                       m_hFrame;
        BOOL                          m_bLoadLocalPlug;
	public:
		CPlugMgr(ILogger* pLogger):CCommunication(pLogger)	
		{ 
			m_pLogger = pLogger;	
			pfnCreateLogger = NULL;
			m_hFrame = NULL;
            m_bLoadLocalPlug = FALSE;
		}
		virtual	~CPlugMgr()									{						}
		virtual void			Release()					{ delete this;			}
        void LoadLocalPlug();
	public:
		//实现IPlugMgr接口成员函数
		virtual ILogger*		CreateLogger();
		virtual ICommunication*	GetCommunication()			{ return this;			}
		virtual ICliPlug*		FindPlug(LPCTSTR plugName);
		virtual bool			Start();
        virtual void			Stop();
        virtual DWORD           SendMessage(DWORD cmd, DWORD param1, DWORD param2);
	private:
		//在这里实事ICommunication的udp通知函数
        virtual void	UdpRecvNotify(char* pData, int length, sockaddr_in& from);
	};
}
#endif