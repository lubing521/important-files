#ifndef __I8DESK_PL_CONTROL_HPP
#define __I8DESK_PL_CONTROL_HPP


#include "../../../include/Utility/utility.h"
#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/MultiThread/Thread.hpp"
#include "../../../include/Utility/SmartPtr.hpp"
#include "../../../include/Http/HttpHelper.h"

#include "plparser.h"

namespace i8desk
{

	// -------------------------------------
	// class PLControl

	// PL 解析
	class PLControl
	{
		typedef PLParser::PLComplate				PLComplate;
		typedef async::thread::AutoCriticalSection	SCLock;
		typedef async::thread::AutoLock<SCLock>		AutoLock;

		typedef std::tr1::function<void()>			Callback;
		typedef std::vector<Callback>				Callbacks;

		// 定时更新PL的间隔
		static const size_t REFRESH_PL_INTERVAL = 1000 * 60 * 10;

	private:
		IRTDataSvr		*pRTDataSvr_;
		ISysOpt			*pSysOpt_;

		SCLock			mutex_;		
		async::thread::AutoEvent		exitEvent_;			// 退出事件
		async::thread::AutoEvent		refreshEvent_;		// 刷新事件
		async::thread::ThreadImplEx		thread_;

		std::auto_ptr<PLParser>			plHandler_;			// PL解析器
		ISAXXMLReaderPtr				rdr_;							
		
		Callbacks						callbacks_;			// 解析XML完毕回调

	public:
		PLControl(ISysOpt *sysOpt, IRTDataSvr *rtDataSvr);
		~PLControl();

	private:
		PLControl(const PLControl &);
		PLControl &operator=(const PLControl &);

	public:
		void Start(const PLComplate &plComplate);			// 失败抛出异常
		void Stop();

		void SetForceRefresh();								// 强制更新
		void RegisterCallback(const Callback &callback);	// PL解析完毕进行回调

	private:
		bool _Init(const PLComplate &plComplate);
		DWORD _Thread();

		http::Buffer _DownLoadCGI();						// 下载CGI
		bool _DownloadFileCallback();						// HTTP回调
		
		http::Buffer _DownLoadPl(const stdex::tString &plUrl);			// 下载删除游戏XML
		http::Buffer _UnZipPL(const http::Buffer &inBuf);		// 解压PL


		stdex::tString _GetCGIParam() const;						// 取得CGI的参数
		stdex::tString _ParserCGI(stdex::tString &cgiBuf);			// 解析CGI
	};


}




#endif