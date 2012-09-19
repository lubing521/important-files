#ifndef _i8desk_plugmgr_h_
#define _i8desk_plugmgr_h_

#include "../include/plug.h"
#include <map>

namespace i8desk
{
	//实现通信组件.
	class CCommunication :public ICommunication
	{
	private:
		ILogger*	m_pLogger;
		tstring		m_SrvIP;
		SOCKET		m_sckTcp, m_sckUdp;
		HANDLE		m_hTcpThread, m_hUdpThread; 
		HANDLE		m_hExit;
		CLock		m_lockTcp;
		bool		m_bIsFirstPackage;
	public:
		CCommunication(ILogger* pLogger);
		virtual ~CCommunication();
		virtual void	Release()	{ delete this; }
	public:
		//实现ICommunication接口的函数(UdpRecvNotify除外)
		virtual bool Start();
		virtual void Stop();
		virtual bool IsConnected();
		virtual void Lock()   { m_lockTcp.Lock(); }
		virtual bool ExecCmd(char*pOutData, int nOutLen, char* &pInData, int& nInLen, bool bNeedAck);
		virtual void UnLock() { m_lockTcp.UnLock(); }
		virtual void GetLocMachineInfo(tagLocMachineInfo& info);
		virtual bool IsFirstPackage();
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
	class CPlugMgr : public IPlugMgr, public CCommunication
	{
	private:
		ILogger* m_pLogger;
		std::map<tstring,  IPlug*> m_mapName2Plugs;		//所有的插件
		typedef std::map<tstring,     IPlug*>::iterator	NAME2PLUGITER;
		CLock m_lock;	
	public:
		CPlugMgr(ILogger* pLogger):CCommunication(pLogger)	{ m_pLogger = pLogger;	}
		virtual	~CPlugMgr()									{						}
		virtual void			Release()					{ delete this;			}
	public:
		//实现IPlugMgr接口成员函数
		virtual ILogger*		CreateLogger();
		virtual ICommunication*	GetCommunication()			{ return this;			}
		virtual IPlug*			FindPlug(LPCTSTR plugName);
		virtual bool			Start();
		virtual void			Stop();
	private:
		//在这里实事ICommunication的udp通知函数
		virtual void	UdpRecvNotify(char* pData, int length, sockaddr_in& from);
	};
}
#endif