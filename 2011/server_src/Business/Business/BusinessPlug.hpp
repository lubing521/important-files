#ifndef __BUSINESS_PLUG_HPP
#define __BUSINESS_PLUG_HPP

#include <memory>
#include "I8ExInfoMgrS.h"


namespace engine
{
	class DownloadEngine;
}


namespace i8desk
{
	
	struct ILogger;

	class TaskProductor;

	// -----------------------------
	// class BusinessPlug

	// 插件中心插件
	class BusinessPlug
		: public ISvrPlug
	{
	private:
		// 插件管理器 
		ISvrPlugMgr	*plugMgr_;
		// 日志插件
		ILogger		*log_;
		// 数据缓存插件
		IRTDataSvr	*rtDataSvr_;
	
		// 下载引擎
		std::auto_ptr<engine::DownloadEngine> engine_;

		// 检测游戏扩展是否需要下载
        std::auto_ptr<TaskProductor> checker_;

        //游戏扩展信息管理
        std::auto_ptr<CI8ExInfoMgrS> clExInfoMgr_;

	public:
		BusinessPlug();
		~BusinessPlug();

	private:
		BusinessPlug(const BusinessPlug &);
		BusinessPlug &operator=(const BusinessPlug &);

	public:
		//释放插件
		virtual void	Release();

		//得到插件的名字
		virtual LPCTSTR GetPlugName();
		//得到插件的版本0xaabbccdd.
		virtual DWORD  	GetVersion();
		//得到插件开发的公司名
		virtual LPCTSTR GetCompany();
		//得到插件的一些描述
		virtual LPCTSTR GetDescription();

		//如果插件需要框架监听的网络端口，如果不需要监听，则返回0.
		//当框架收到该端口事件时，会回调给该接口的INetLayerEvent对应的虚函数
		virtual DWORD   GetListPort();

		//插件内部的初始化工作,当ISvrPlugMgr加载一个插件后会调用该函数
		virtual bool	Initialize(ISvrPlugMgr* pPlugMgr);
		//当ISvrPlugMgr退出时会调先调用该函数后，才调用Release()来释放插件。
		virtual bool	UnInitialize();

		//主框架或者其它插件可以通过该函数来相互交互。类似于Windows的消息机制,
		//根据cmd的不同，param1,parma2实际的意义与不同.
		//如果需要在插件内部分配内存返回给调用者。则插件内部通过CoTaskMemFree来分配内存，
		//调用者通过CoTaskMemFree来释放插件分配的内存，因此插件接口不再提供其它内存分配释放内存.
		virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2);


		virtual void OnAccept(i8desk::INetLayer*pNetLayer, SOCKET sck, DWORD param);
		virtual void OnSend(i8desk::INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param); 
		virtual void OnRecv(i8desk::INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param);
		virtual void OnClose(i8desk::INetLayer*pNetLayer, SOCKET sck, DWORD param);

	private:
		void _AddTask(long type, long gid, long priority);

	};
}


#endif