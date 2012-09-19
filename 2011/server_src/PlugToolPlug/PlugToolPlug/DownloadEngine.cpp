#include "stdafx.h"
#include "DownloadEngine.h"

#include "ThreadPoolHelper.h"
#include "LogHelper.h"

#include "../../../include/AsyncIO/iocp/WinException.hpp"








#define DEBUG_NORMALBLOCK new( _NORMAL_BLOCK, __FILE__ , __LINE__ ) 
#ifdef new 
#undef new 
#endif 

#define new DEBUG_NORMALBLOCK 


namespace i8desk
{
	template < typename HandlerT >
	struct ThreadImpl
		: public std::tr1::enable_shared_from_this< ThreadImpl<HandlerT> >
	{
		typedef async::thread::QueueWorkItemPool Pool;
		std::auto_ptr<Pool>	pool_;
		HandlerT handler_;

		explicit ThreadImpl(const HandlerT &handler)
			: handler_(handler)
		{}

		void Run()
		{
			pool_.reset(new Pool(std::tr1::bind(&ThreadImpl<HandlerT>::_RunImpl, shared_from_this())));
			pool_->Call();
		}

		void _RunImpl()
		{
			handler_();
		}
	};
	
	template <typename HandlerT >
	std::tr1::shared_ptr< ThreadImpl<HandlerT> > MakeThreadImpl(const HandlerT &handler)
	{
		std::tr1::shared_ptr< ThreadImpl<HandlerT> > threadImpl(new ThreadImpl<HandlerT>(handler));
		return threadImpl;
	}

	

	struct DownloadEngine::DownloadImpl
	{
		DownloadEngine *engine_;

	public:
		DownloadImpl(DownloadEngine *engine)
			: engine_(engine)
		{}

	public:
		void AddTask(long gid)
		{
			MakeThreadImpl(std::tr1::bind(&DownloadImpl::_Thread, this, gid))->Run();
		}

	private:
		void _Thread(long gid)
		{
			Log(LM_INFO, _T("开始下载插件"));

			stdex::tOstringstream os;
			os << _T("http://211.154.172.139/temp/") << gid << _T(".zip");
			stdex::tString url = os.str();

			const static stdex::tString plugin = utility::GetAppPath() + _T("plugin_tools/"); 
			if( !::PathFileExists(plugin.c_str()) )
				::CreateDirectory(plugin.c_str(), NULL);

			http::Buffer buf;

			try
			{
				// 下载文件
				using namespace std::tr1::placeholders;
				buf = http::DownloadFile(url, std::tr1::bind(&DownloadEngine::_Check, engine_, gid, _1, _2, _3));
								
				stdex::tOstringstream osPath;
				osPath << plugin.c_str() << gid;

				const stdex::tString unZipPath = osPath.str() + _T("/");
				utility::mkpath(unZipPath);

				if( !http::UnZip(buf, unZipPath) )
					throw std::runtime_error("解压失败!");

				// 处理文件
#ifdef _DEBUG
				stdex::tString zipPath = osPath.str() + _T(".zip");
				std::ofstream out(zipPath.c_str(), std::ios::binary | std::ios::out);
				out.write(buf.first.get(), buf.second - 1);
#endif
			
				// 下载完成回调
				engine_->OnComplate(gid);
			}
			catch(std::exception &e)
			{
				Log(LM_ERROR, _T("下载插件: %d 出错"), gid);
			}
			
			Log(LM_INFO, _T("下载插件: %d 完成"), gid);
		}
	};

	DownloadEngine::DownloadEngine()
		: impl_(new DownloadImpl(this))
	{

	}

	DownloadEngine::~DownloadEngine()
	{

	}

	bool DownloadEngine::Start()
	{
		try
		{
			if( !exit_.Create(0, TRUE, 0) )
				throw async::iocp::Win32Exception("CreateEvent");
		}
		catch(std::exception &e)
		{
			stdex::tString err(CA2T(e.what()));
			Log(LM_ERROR, err);
			return false;
		}

		return true;
	}

	bool DownloadEngine::Stop()
	{
		try
		{
			exit_.SetEvent();
		}
		catch(std::exception &e)
		{
			stdex::tString err(CA2T(e.what()));
			Log(LM_ERROR, err);
			return false;
		}

		Log(LM_DEBUG, _T("退出DownloadEngine成功"));

		return true;
	}

	bool DownloadEngine::AddTask(long gid)
	{
		{
			Lock lock(mutex_);

			if( plugToolsInfo_.find(gid) != plugToolsInfo_.end() )
			{
				Log(LM_INFO, _T("插件 %ld正在下载"), gid);
				return false;
			}
		}
		try
		{
			Log(LM_INFO, _T("添加插件 %ld"), gid);
			impl_->AddTask(gid);
		}
		catch(std::exception &e)
		{
			stdex::tString err(CA2T(e.what()));
			Log(LM_ERROR, err);
			return false;
		}

		return true;
	}

	bool DownloadEngine::GetStatus(std::vector<db::tPlugToolStatus> &status)
	{
		Lock lock(mutex_);

		for(PlugToolsInfo::const_iterator iter = plugToolsInfo_.begin(); iter != plugToolsInfo_.end(); ++iter)
		{
			const db::tPlugToolStatus &plugStatus = iter->second;
			status.push_back(plugStatus);
		}

		return true;
	}

	void DownloadEngine::Register(const ComplateCallback &callback)
	{
		complate_ = callback;
	}


	void DownloadEngine::OnComplate(long gid)
	{
		{
			Lock lock(mutex_);
			plugToolsInfo_.erase(gid);
		}

		complate_(gid);
	}

	void DownloadEngine::OnError(long gid)
	{
		Lock lock(mutex_);
		plugToolsInfo_.erase(gid);
	}

	bool DownloadEngine::_Check(long gid, const char *buf, DWORD size, const WinInet::CHttpFile::CInfo &info)
	{
		// 检测是否退出
		if( exit_.WaitForEvent(0) )
			return true;

		// 填写进度信息
		Lock lock(mutex_);

		db::tPlugToolStatus status;
		status.PID				= gid;
		status.Size				= info.GetTotalDataLength();
		status.ReadSize			= info.GetTotalRead();
		status.TransferRate		= (uint64)info.GetDownloadRate();					
		status.AvgTransferRate	= (uint64)info.GetAverageDownloadRate();					
		status.TotalTime		= info.GetTotalTime();									
		status.TimeLeft			= info.GetTimeLeft();					

		plugToolsInfo_[gid] = status;

		return false;
	}


}